#include "FunctionalUtilities.h"
#include "OptionPricing.h"
#include "CFDistUtilities.h"
#include "OptionCalibration.h"
#include "get_cf.h"
#include "parse_json.h"
#include "cuckoo.h"
#include <chrono>
const std::array<std::string, 9> possibleCalibrationParameters({
    "lambda", "muJ", "sigJ", "sigma", "v0", "speed", "adaV", "rho", "delta"
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
    CF&& logCF, const Array1& ul, const Array2& prices, Array3&& strikes, 
    double S0, double r, double T
){
    const auto parameters=generateConstParameters(prices, strikes, S0);
    const auto minStrike=std::get<1>(parameters);
    const auto maxStrike=std::get<2>(parameters);
    strikes.push_front(maxStrike);
    strikes.push_back(minStrike);
    int numU=256;
    int nM1=strikes.size()-1;
    auto objFn=[
        logCF=std::move(logCF), 
        strikes=std::move(strikes),
        S0, r, T, numU
    ](const auto& params){
        return futilities::sum(optionprice::FangOostCallPrice(
            S0, strikes,
            r, T,
            numU,  
            cfHOC(params)
        ), [&](const auto& v, const auto& index){
            return (index==0||index==nM1)?0.0:futilities::const_power(v-strikes[index], 2);
        })
    };

    const int nestSize=25;
   // const int totalMC=1500;
    const int totalMC=1000;
    const double tol=.000001;//tolerance for squared error
    return cuckoo::optimize(objFn, ul, nestSize, totalMC, tol, std::chrono::system_clock::now().time_since_epoch().count());
}
constexpr int splineChoice=0;
constexpr int calibrateChoice=1;


int main(int argc, char* argv[]){
    if(argc>2){
        auto parsedJson=parse_char(argv[2]);
        auto prices=get_prices_var(parsedJson);
        const auto T=get_ranged_variable(parsedJson, modelParams, "T");
        const auto r=get_ranged_variable(parsedJson, modelParams, "r");
        const auto S0=get_ranged_variable(parsedJson, modelParams, "S0");
        auto strikes=get_k_var<std::deque<double>>(parsedJson);
        int key=std::stoi(argv[1]);
        switch(key){
            case splineChoice:{
                const int numIndices=256;
                //generateSplineCurves(prices, strikes, S0, r, T, numIndices);
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
                auto cfI=cfGeneric(r, T);
                auto cfBaseI=cf(r, T);
                auto cfHOC=[
                    getArgOrConstantCurry=std::move(getArgOrConstantCurry), 
                    cfI=std::move(cfI),
                    cfBaseI=std::move(cfBaseI),
                    useNumericMethod=hasAllVariables(jsonVariable, "lambda", "delta")
                ](const auto& args){
                    /*auto getField=[&](const auto& key){
                        return getArgOrConstantCurry(key, args);
                    };*/
                    return [getField=[&](const auto& key){
                        return getArgOrConstantCurry(key, args);
                    }](const auto& u){
                        return useNumericMethod?cfI(
                            getField("lambda"), 
                            getField("muJ"), 
                            getField("sigJ"), 
                            getField("sigma"), 
                            getField("v0"), 
                            getField("speed"), 
                            getField("adaV"), 
                            getField("rho"),
                            getField("delta")
                        )(u):cfBaseI(
                            getField("lambda"), 
                            getField("muJ"), 
                            getField("sigJ"), 
                            getField("sigma"), 
                            getField("v0"), 
                            getField("speed"), 
                            getField("adaV"), 
                            getField("rho")
                        )(u);
                    };
                };
                json_print_calibrated_params<cuckoo::optparms, cuckoo::fnval>(
                    mapKeyToIndexVariable, 
                    genericCallCalibrator_cuckoo(
                        std::move(cfHOC),                    
                        getConstraints(
                            jsonVariable, 
                            possibleCalibrationParameters, 
                            modelParams, 
                            parsedJson["constraints"]
                        ),
                        prices, 
                        std::move(strikes),
                        S0, r, T
                    ), 
                    prices.size()
                );          
            }
        }
    }
    return 0;
}