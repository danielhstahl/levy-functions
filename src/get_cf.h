#include "CharacteristicFunctions.h"

template<typename U>
auto cfLogBase(const U& u, 
    double T,
    double lambda, double muJ, double sigJ,
    double sigma, double v0, double speed,
    double adaV, double rho
){
    return chfunctions::cirLogMGF(
        -chfunctions::mertonLogRNCF(u, lambda, muJ, sigJ, 0.0, sigma),
        speed, 
        speed-adaV*rho*u*sigma,
        adaV,
        T,
        v0
    );
}

auto cf(
    double r,
    double T, 
    double S0
){
    //trivially copyable...this is the SV3 of the following paper:
    //https://pdfs.semanticscholar.org/67cd/b553e2624c79a960ff79d0dfe6e6833690a7.pdf 
    return [=](
        double lambda,
        double muJ, 
        double sigJ,
        double sigma,
        double v0,
        double speed,
        double adaV,
        double rho
    ){
        return [=](const auto& u){
            return exp(r*T*u+
                cfLogBase(u, T, lambda, muJ, sigJ, sigma, v0, speed, adaV, rho)
            );
        };
    };
    
}
