#include "FunctionalUtilities.h"
#include "OptionPricing.h"
#include "CFDistUtilities.h"
#include "CalibrateOptions.h"
#include "get_cf.h"
#include "parse_json.h"
#include "neldermead.h"
#include "cuckoo.h"
#include <chrono>
constexpr int fullmodel=0;
constexpr int hestonmodel=1;
constexpr int bsmodel=2;
const cuckoo::upper_lower<double> sigmaUL=cuckoo::upper_lower<double>(0.0, 1.0);
const cuckoo::upper_lower<double> speedUL=cuckoo::upper_lower<double>(0.0, 1.0);
const cuckoo::upper_lower<double> adaVUL=cuckoo::upper_lower<double>(0.0, 1.0);
const cuckoo::upper_lower<double> rhoUL=cuckoo::upper_lower<double>(-1.0, 1.0);
const std::array<cuckoo::upper_lower<double>, 9> fullModelConstraints({
    cuckoo::upper_lower<double>(0.0, 2.0), //c
    cuckoo::upper_lower<double>(0.0, 20), //g
    cuckoo::upper_lower<double>(0.0, 20), //m
    cuckoo::upper_lower<double>(-3.0, 2.0), //y
    sigmaUL,
    cuckoo::upper_lower<double>(0.2, 1.8), //v0
    speedUL,
    adaVUL,
    rhoUL
});
const std::array<cuckoo::upper_lower<double> , 5> hestonConstraints({
    sigmaUL,
    cuckoo::upper_lower<double>(0.2, 1.8), //v0
    speedUL,
    adaVUL,
    rhoUL
});
const std::array<cuckoo::upper_lower<double>, 1> bsConstraints({
    sigmaUL
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
        prices, strikes, 100, std::chrono::system_clock::now().time_since_epoch().count() //seed
    );
}

int main(int argc, char* argv[]){
    if(argc>2){
        auto parsedJson=parse_char(argv[2]);
        auto options=get_option_var(parsedJson);
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
        int key=std::stoi(argv[1]);
        //auto started = std::chrono::high_resolution_clock::now();
        switch(key){
            case fullmodel:{
                /*json_print_calibrated_params<neldermead::optparms, neldermead::fnval>(
                    std::vector<std::string>({"C", "G", "M", "Y", "sigma", "v0", "speed", "adaV", "rho"}), 
                    genericCallCalibrator_neldermead(
                        [&](const auto& args){
                            return cgmyCFHOC(
                                args[0], args[1], args[2], //c, g, m
                                args[3], args[4], args[5],//y, sigma, v0
                                args[6], args[7], args[8] //speed, adaV, rho
                            );
                        },                    
                        [](const auto& args){ //C has to be positive.  Sigma has to be positive
                            return args[0]<0||args[4]<0||args[6]<0||args[7]<0||args[8]<-1||args[8]>0||args[5]<.5||args[5]>1.5;//rho can technically be between -1 and 1 but I have a strong prior that its negative
                        },
                        std::vector<double>({.2, 2, 2, .4, .2, 1.0, .3, .2, -.2}), //guess
                        prices, get_k_var(parsedJson),
                        options.S0, options.r, options.T, xMax, numU
                    ), 
                    prices.size()
                );*/
                json_print_calibrated_params<cuckoo::optparms, cuckoo::fnval>(
                    std::vector<std::string>({"C", "G", "M", "Y", "sigma", "v0", "speed", "adaV", "rho"}), 
                    genericCallCalibrator_cuckoo(
                        [&](const auto& args){
                            return cgmyCFHOC(
                                args[0], args[1], args[2], //c, g, m
                                args[3], args[4], args[5],//y, sigma, v0
                                args[6], args[7], args[8] //speed, adaV, rho
                            );
                        },                    
                        fullModelConstraints,
                        prices, get_k_var(parsedJson),
                        options.S0, options.r, options.T, xMax, numU
                    ), 
                    prices.size()
                );
                
                break;
            }
            case hestonmodel:{
                /*json_print_calibrated_params<neldermead::optparms, neldermead::fnval>(
                    std::vector<std::string>({"sigma", "v0", "speed", "adaV", "rho"}), 
                    genericCallCalibrator_neldermead(
                        [&](const auto& args){
                            return cgmyCFHOC(
                                0, 2.0, 2.0, .5, 
                                args[0], args[1], args[2],//sigma, v0, speed
                                args[3], args[4] //adaV, rho
                            );
                        },                    
                        [](const auto& args){ //
                            return args[0]<0||args[2]<0||args[3]<0||args[4]<-1||args[4]>1||args[1]<.5||args[1]>1.5;//rho can technically be between -1 and 1 but I have a strong prior that its negative
                        },
                        std::vector<double>({.2, 1, .2, .2, -.2}), //guess
                        prices, get_k_var(parsedJson),
                        options.S0, options.r, options.T, xMax, numU
                    ),
                    prices.size()
                );*/
                json_print_calibrated_params<cuckoo::optparms, cuckoo::fnval>(
                    std::vector<std::string>({"sigma", "v0", "speed", "adaV", "rho"}), 
                    genericCallCalibrator_cuckoo(
                        [&](const auto& args){
                            return cgmyCFHOC(
                                0, 2.0, 2.0, .5, 
                                args[0], args[1], args[2],//sigma, v0, speed
                                args[3], args[4] //adaV, rho
                            );
                        },                    
                        hestonConstraints,
                        prices, get_k_var(parsedJson),
                        options.S0, options.r, options.T, xMax, numU
                    ), 
                    prices.size()
                );
                break;
            }
            case bsmodel:{
                json_print_calibrated_params<neldermead::optparms, neldermead::fnval>(
                    std::vector<std::string>({"sigma"}), 
                    genericCallCalibrator_neldermead(
                        [&](const auto& args){
                            return cgmyCFHOC(
                                0, 2.0, 2.0, .5, 
                                args[0], 1.0, .5, //sigma
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
                /*json_print_calibrated_params<cuckoo::optparms, cuckoo::fnval>(
                    std::vector<std::string>({"sigma"}), 
                    genericCallCalibrator_cuckoo(
                        [&](const auto& args){
                            return cgmyCFHOC(
                                0, 2.0, 2.0, .5, 
                                args[0], 1.0, .5, //sigma
                                0.0, .5 
                            );
                        },                    
                        bsConstraints,
                        prices, get_k_var(parsedJson),
                        options.S0, options.r, options.T, xMax, numU
                    ), 
                    prices.size()
                );*/
                break;
            }
        }
        //auto done = std::chrono::high_resolution_clock::now();
        //std::cout << "Speed: "<<std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count()<<std::endl;
    }
}