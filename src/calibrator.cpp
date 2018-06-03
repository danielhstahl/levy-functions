#include "FunctionalUtilities.h"
#include "OptionPricing.h"
#include "CFDistUtilities.h"
#include "OptionCalibration.h"
#include "get_cf.h"
#include "parse_json.h"
#include "cuckoo.h"
#include "firefly.h"
#include <chrono>
const std::array<std::string, 8> possibleCalibrationParameters({
    "lambda", "muJ", "sigJ", "sigma", "v0", "speed", "adaV", "rho"
});

template<typename Array>
auto removeFirstAndLastElement(Array&& arr){
    Array sub(arr.begin()+1,arr.end()-1);
    return sub;
}

auto getU(int N){
    double du= 2.0*M_PI/N;
    return futilities::for_each_parallel(1, N, [&](const auto& index){
        return index*du;
    });
}

template<typename Array1, typename Array2>
auto generateConstParameters(
    const Array1& prices, const Array2& strikes, double S0
){
    const int N=1024;
    const auto maxStrike=strikes.back()*10.0;
    const auto minStrike=S0/maxStrike;
    return std::make_tuple(N, minStrike, maxStrike);
}


template<typename Array1, typename Array2>
auto generateSplineCurves(
    const Array1& prices, const Array2& strikes, 
    double S0, double r, double T, int numNodes
){
    const auto parameters=generateConstParameters(prices, strikes, S0);
    const int N=std::get<0>(parameters);
    const auto minStrike=std::get<1>(parameters);
    const auto maxStrike=std::get<2>(parameters);
    const auto discount=exp(-r*T);
    auto s=optioncal::getOptionSpline(strikes, prices, S0, discount, minStrike, maxStrike);
    double minLogStrike=log(minStrike);
    double maxLogStrike=log(maxStrike/S0);
    double logdk=(maxLogStrike-minLogStrike)/(double)(numNodes-1);
    json_print_multiple_obj(
        "curve",
        [&](){
            json_print_spline(
                numNodes, 
                log(minStrike), 
                log(maxStrike/S0),
                logdk,
                [&](const auto x){
                    return x-r*T;
                },
                [&](const auto x){
                    return optioncal::maxZeroOrNumber(s(exp(x)));
                }
            );
        }, 
        "points",
        [&](){
            json_print_spline(
                strikes,
                [&](const auto x, int i){
                    return log(x/S0)-r*T;
                },
                [&](const auto x, int i){
                    return prices[i]/S0-optioncal::maxZeroOrNumber(1.0-x*discount/S0);
                }
            );
        });

}




template<typename CF, typename Array1, typename Array2, typename Array3>
auto genericCallCalibrator_cuckoo(
    CF&& logCF, const Array1& ul, Array2&& prices, Array3&& strikes, 
    double S0, double r, double T
){
    const auto parameters=generateConstParameters(prices, strikes, S0);
    const int N=std::get<0>(parameters);
    const int numU=15;//15 seems like a reasonable number in tests
    const auto minStrike=std::get<1>(parameters);
    const auto maxStrike=std::get<2>(parameters);
    const auto uArray=getU(numU);
    const auto estimateOfPhi=optioncal::generateFOEstimate(
        strikes, 
        prices,  S0, 
        r, T,
        minStrike, maxStrike
    );

    auto phis=estimateOfPhi(N, uArray);
    auto objFn=optioncal::getObjFn_arr(
        std::move(phis),
        std::move(logCF),
        std::move(uArray)
    ); //returns function which takes param vector
    const int nestSize=25;
    const int totalMC=10000;
   // const int totalMC=1000; 
    const double tol=.000001;//tolerance for squared error
    return cuckoo::optimize(objFn, ul, nestSize, totalMC, tol, std::chrono::system_clock::now().time_since_epoch().count());
} 
constexpr int splineChoice=0;
constexpr int calibrateChoice=1;

int main(int argc, char* argv[]){
    if(argc>2){
        auto parsedJson=parse_char(argv[2]);
        auto prices=get_prices_var(parsedJson);
        const int n=prices.size();
        const auto T=get_ranged_variable(parsedJson, modelParams, "T");
        const auto r=get_ranged_variable(parsedJson, modelParams, "r");
        const auto S0=get_ranged_variable(parsedJson, modelParams, "S0");
        auto strikes=get_k_var<std::vector<double>>(parsedJson);
        int key=std::stoi(argv[1]);
        switch(key){
            case splineChoice:{ 
                const int numIndices=256;
                generateSplineCurves(prices, strikes, S0, r, T, numIndices);
                break;
            }
            case calibrateChoice:{
                const auto& jsonVariable=parsedJson["variable"];
                const auto mapKeyToIndexVariable=constructKeyToIndex(jsonVariable, possibleCalibrationParameters);
                const auto mapStatic=constructStaticKeyToValue(parsedJson, possibleCalibrationParameters);
                const auto mapKeyToExistsStatic=std::get<0>(mapStatic);
                const auto mapKeyToValueStatic=std::get<1>(mapStatic); 
                //const auto [mapKeyToExistsStatic, mapKeyToValueStatic]=constructStaticKeyToValue(parsedJson, possibleCalibrationParameters);
                auto getArgOrConstantCurry=[&](const auto& key, const auto& args){
                    return getArgOrConstant(key, args, mapKeyToIndexVariable, mapKeyToExistsStatic, mapKeyToValueStatic);
                };
                auto cfI=cfLogBase(T);
                auto cfHOC=[
                    getArgOrConstantCurry=std::move(getArgOrConstantCurry), 
                    cfI=std::move(cfI)
                ](const auto& args){
                    auto getField=[&](const auto& key){
                        return getArgOrConstantCurry(key, args);
                    };
                    return [
                        lambda=getField("lambda"),
                        muJ=getField("muJ"),
                        sigJ=getField("sigJ"),
                        sigma=getField("sigma"),
                        v0=getField("v0"),
                        speed=getField("speed"),
                        adaV=getField("adaV"),
                        rho=getField("rho"),
                        cfI=std::move(cfI)
                    ](const auto& u){
                        return cfI(
                            u,
                            lambda, 
                            muJ, 
                            sigJ, 
                            sigma, 
                            v0, 
                            speed, 
                            adaV, 
                            rho
                        );
                    };
                };
                json_print_calibrated_params<swarm_utils::optparms, swarm_utils::fnval>(
                    mapKeyToIndexVariable, 
                    genericCallCalibrator_cuckoo(
                        std::move(cfHOC),                    
                        getConstraints(
                            jsonVariable, 
                            possibleCalibrationParameters, 
                            modelParams, 
                            parsedJson["constraints"]
                        ),
                        std::move(prices), 
                        std::move(strikes),
                        S0, r, T
                    ), 
                    n
                );          
            }
        }
    }
    return 0;
}