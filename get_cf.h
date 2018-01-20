#include "CharacteristicFunctions.h"

auto cf(
    double r,
    double T, 
    double S0
){
    //trivially copyable...this is the SV3 of the following paper:
    //https://pdfs.semanticscholar.org/67cd/b553e2624c79a960ff79d0dfe6e6833690a7.pdf 
    return [=](
        double C, 
        double G,
        double M,
        double Y,
        double sigma,
        double v0,
        double speed,
        double adaV,
        double rho
    ){
        return [=](const auto& u){
            return exp(r*T*u+
            chfunctions::cirLogMGF(
                -chfunctions::cgmyLogRNCF(u, C, G, M, Y, 0.0, sigma),
                speed, 
                speed-adaV*rho*u*sigma,
                adaV,
                T, 
                v0
            ));
        };
    };
    
}
