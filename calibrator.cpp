#include "FunctionalUtilities.h"
#include "OptionPricing.h"
#include "CFDistUtilities.h"
#include "CalibrateOptions.h"
#include "get_cf.h"
#include "parse_json.h"
#include "neldermead.h"
#include "cuckoo.h"
#include <chrono>

const cuckoo::upper_lower<double> sigmaUL=cuckoo::upper_lower<double>(0.0, 1.0);
const cuckoo::upper_lower<double> speedUL=cuckoo::upper_lower<double>(0.0, 1.0);
const cuckoo::upper_lower<double> adaVUL=cuckoo::upper_lower<double>(0.0, 1.0);
const cuckoo::upper_lower<double> rhoUL=cuckoo::upper_lower<double>(-1.0, 1.0);
const std::unordered_map<std::string, cuckoo::upper_lower<double> > fullModelConstraints({
    {"C", cuckoo::upper_lower<double>(0.0, 2.0)}, //c
    {"G", cuckoo::upper_lower<double>(0.0, 20)}, //g
    {"M", cuckoo::upper_lower<double>(0.0, 20)}, //m
    {"Y", cuckoo::upper_lower<double>(-3.0, 2.0)}, //y
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


template<typename CF, typename ConstraintFn, typename Array1, typename Array2>
auto genericCallCalibrator_neldermead(
    const CF& cf, const ConstraintFn& constraintFn, 
    Array1&& guess, const Array1& prices, Array2&& strikes, 
    double S0, double r, double T, double xMax, int numU
){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    int nMinus1=strikes.size()-1;
    return calibrateoptions::l2normNelderMeadVector(
        //these initial guesses may be a reasonable first guess but that this is highly dependent on the problem.   
        guess, 
        [&](const auto& localStrikes, const auto& args){
            return removeFirstAndLastElement(optionprice::FangOostCallPrice(
                S0, localStrikes, r, T, numU, 
                cf(args)
            ));
        }, 
        constraintFn,
        prices, strikes
    );
}
template<typename CF, typename Array1, typename Array2, typename Array3>
auto genericCallCalibrator_cuckoo(
    const CF& cf, const Array1& ul, const Array2& prices, Array3&& strikes, 
    double S0, double r, double T, double xMax, int numU
){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    int nMinus1=strikes.size()-1;
    return calibrateoptions::l2normCuckooVector(
        [&](const auto& localStrikes, const auto& args){
            return removeFirstAndLastElement(optionprice::FangOostCallPrice(
                S0, localStrikes, r, T, numU, 
                cf(args)
            ));
        }, 
        ul,
        prices, strikes, 100, std::chrono::system_clock::now().time_since_epoch().count()
    );
}

template<typename RpJson>
std::vector<cuckoo::upper_lower<double> > getConstraints(const RpJson& json){
    std::vector<cuckoo::upper_lower<double> > modelConstraints;
    for (
        auto itr = json.MemberBegin();itr != json.MemberEnd(); ++itr
    ){
        modelConstraints.push_back(fullModelConstraints.at(itr->name.GetString()));
    }
    return modelConstraints;
}

int main(int argc, char* argv[]){
    if(argc>1){
        auto parsedJson=parse_char(argv[1]);
        auto options=get_static_vars(parsedJson);
        auto cgmyCFHOC=cf(
            options.r,
            options.T,
            options.S0
        );
        auto prices=get_prices_var(parsedJson);
        /**NOTE that this is a big assumption about the
         * domain for these distributions.
         * Be careful!*/
        double xMax=10.0;//this should be plenty large
        int numU=pow(2, options.numU);
        const auto& jsonVariable=parsedJson["variable"];
        const auto& jsonStatic=parsedJson["static"];
        auto modelConstraints=getConstraints(jsonVariable);
        const std::unordered_map<std::string, int> mapKeyToIndex=constructKeyToIndex(jsonVariable);

        auto getArgOrConstantCurry=[&](const auto& key, const auto& args){
            return getArgOrConstant(key, args, jsonStatic, mapKeyToIndex);
        };
        json_print_calibrated_params<cuckoo::optparms, cuckoo::fnval>(
            mapKeyToIndex, 
            genericCallCalibrator_cuckoo(
                [&](const auto& args){
                    auto getField=[&](const auto& key){
                        return getArgOrConstantCurry(key, args);
                    };
                    return cgmyCFHOC(
                        getField("C"), 
                        getField("G"), 
                        getField("M"), 
                        getField("Y"), 
                        getField("sigma"), 
                        getField("v0"), 
                        getField("speed"), 
                        getField("adaV"), 
                        getField("rho")
                    );
                },                    
                modelConstraints,
                prices, get_k_var(parsedJson),
                options.S0, options.r, options.T, xMax, numU
            ), 
            prices.size()
        );          
    }
}