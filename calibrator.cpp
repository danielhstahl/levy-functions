#include "FunctionalUtilities.h"
#include "OptionPricing.h"
#include "CFDistUtilities.h"
#include "CalibrateOptions.h"
#include "get_cf.h"
#include "parse_json.h"

constexpr int fullmodel=0;
constexpr int hestonmodel=1;
constexpr int bsmodel=2;

template<typename CF, typename ConstraintFn, typename Array1, typename Array2>
auto genericCallCalibrator(
    const CF& cf, const ConstraintFn& constraintFn, 
    Array1&& guess, const Array1& prices, Array2&& strikes, 
    double S0, double r, double T, double xMax, int numU
){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    return calibrateoptions::l2normvector(
        //these initial guesses may be a reasonable first guess but that this is highly dependent on the problem.   
        guess, 
        [&](const auto& localStrikes, const auto& args){
            return optionprice::FangOostCallPrice(
                S0, localStrikes, r, T, numU, 
                cf(args)
            );
        }, 
        constraintFn, 
        [&](const auto& index){ //don't count first or last strike
            return index==0||index==(strikes.size()-1)?0.0:1.0;
        }, 
        prices, strikes
    );
}

int main(int argc, char* argv[]){
    if(argc>2){
        auto parsedJson=parse_char(argv[2]);
        auto options=get_option_var(parsedJson);
        auto cgmyCFHOC=cf(
            options.r,
            options.T,
            options.S0,
            options.v0
        );
        auto prices=get_prices_var(parsedJson);
        /**NOTE that this is a big assumption about the
         * domain for these distributions.
         * Be careful!*/
        double xMax=10.0;//this should be plenty large
        int numU=pow(2, options.numU);
        int key=std::stoi(argv[1]);
        switch(key){
            case fullmodel:{
                json_print_calibrated_params(
                    std::vector<std::string>({"C", "G", "M", "Y", "sigma", "speed", "adaV", "rho"}), 
                    genericCallCalibrator(
                        [&](const auto& args){
                            return cgmyCFHOC(
                                args[0], args[1], args[2], //c, g, m
                                args[3], args[4], args[5], //y, sigma, speed
                                args[6], args[7] //adaV, rho
                            );
                        },                    
                        [](const auto& args){ //C has to be positive.  Sigma has to be positive
                            return args[0]<0||args[4]<0||args[5]<0||args[6]<0||args[7]<-1||args[7]>1;
                        },
                        std::vector<double>({.2, 2, 2, .4, .2, .3, .2, .2}), //guess
                        prices, get_k_var(parsedJson),
                        options.S0, options.r, options.T, xMax, numU
                    ), 
                    prices.size()
                );
                break;
            }
            case hestonmodel:{
                json_print_calibrated_params(
                    std::vector<std::string>({"sigma", "speed", "adaV", "rho"}), 
                    genericCallCalibrator(
                        [&](const auto& args){
                            return cgmyCFHOC(
                                0, 2.0, 2.0, .5, 
                                args[0], args[1], //sigma, speed
                                args[2], args[3] //adaV, rho
                            );
                        },                    
                        [](const auto& args){ //
                            return args[0]<0||args[1]<0||args[2]<0||args[3]<-1||args[3]>1;
                        },
                        std::vector<double>({.2, .2, .2, .2}), //guess
                        prices, get_k_var(parsedJson),
                        options.S0, options.r, options.T, xMax, numU
                    ),
                    prices.size()
                );
                break;
            }
            case bsmodel:{
                json_print_calibrated_params(
                    std::vector<std::string>({"sigma"}), 
                    genericCallCalibrator(
                        [&](const auto& args){
                            return cgmyCFHOC(
                                0, 2.0, 2.0, .5, 
                                args[0], .5, //sigma
                                0.0, .5 
                            );
                        },                    
                        [](const auto& args){ //C has to be positive.  Sigma has to be positive
                            return args[0]<0;
                        },
                        std::vector<double>({.2}), //guess
                        prices, get_k_var(parsedJson),
                        options.S0, options.r, options.T, xMax, numU
                    ),
                    prices.size()
                );
                break;
            }
        }
    }
}