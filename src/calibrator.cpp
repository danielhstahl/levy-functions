#include "FunctionalUtilities.h"
#include "OptionPricing.h"
#include "CFDistUtilities.h"
#include "OptionCalibration.h"
#include "get_cf.h"
#include "parse_json.h"
#include "cuckoo.h"
#include <chrono>

const std::array<std::string, 8> possibleParameters({
    "lambda", "muJ", "sigJ", "sigma", "v0", "speed", "adaV", "rho"
});
const std::unordered_map<std::string, cuckoo::upper_lower<double> > fullModelConstraints({
    {"lambda", cuckoo::upper_lower<double>(0.0, 2.0)}, //c
    {"muJ", cuckoo::upper_lower<double>(-1.0, 1.0)}, //g
    {"sigJ", cuckoo::upper_lower<double>(0.0, 2.0)}, //m
    {"sigma", cuckoo::upper_lower<double>(0.0, 1.0)}, //sigma
    {"v0", cuckoo::upper_lower<double>(0.2, 1.8)}, //v0
    {"speed", cuckoo::upper_lower<double>(0.0, 1.0)}, //speed
    {"adaV", cuckoo::upper_lower<double>(0.0, 1.0)}, //adaV,
    {"rho", cuckoo::upper_lower<double>(-1.0, 1.0)} //rho
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
template<typename CF, typename Array1, typename Array2, typename Array3>
auto genericCallCalibrator_cuckoo(
    CF&& logCF, const Array1& ul, const Array2& prices, Array3&& strikes, 
    double S0, double r, double T
){
    const int N=1024;
    const auto uArray=getU(15);
    
    const auto maxStrike=strikes.back()*10.0;
    const auto minStrike=S0/maxStrike;
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
    return cuckoo::optimize(objFn, ul, 25, 500, std::chrono::system_clock::now().time_since_epoch().count());
}



int main(int argc, char* argv[]){
    if(argc>1){
        auto parsedJson=parse_char(argv[1]);
        auto options=get_static_vars(parsedJson);
        auto prices=get_prices_var(parsedJson);

        const auto& jsonVariable=parsedJson["variable"];
        auto modelConstraints=getConstraints(jsonVariable, possibleParameters, fullModelConstraints);
        const std::unordered_map<std::string, int> mapKeyToIndex=constructKeyToIndex(jsonVariable, possibleParameters);
        auto getArgOrConstantCurry=[&](const auto& key, const auto& args){
            return getArgOrConstant(key, args, parsedJson, mapKeyToIndex);
        };
        auto T=options.T;
        auto cfHOC=[
            getArgOrConstantCurry=std::move(getArgOrConstantCurry), 
            T
        ](const auto& u, const auto& args){
            auto getField=[&](const auto& key){
                return getArgOrConstantCurry(key, args);
            };
            return cfLogBase(
                u, 
                T,
                getField("lambda"), 
                getField("muJ"), 
                getField("sigJ"), 
                getField("sigma"), 
                getField("v0"), 
                getField("speed"), 
                getField("adaV"), 
                getField("rho")
            );
        };

        json_print_calibrated_params<cuckoo::optparms, cuckoo::fnval>(
            mapKeyToIndex, 
            genericCallCalibrator_cuckoo(
                std::move(cfHOC),                    
                modelConstraints,
                prices, get_k_var_vector(parsedJson),
                options.S0, options.r, T
            ), 
            prices.size()
        );          
    }
}