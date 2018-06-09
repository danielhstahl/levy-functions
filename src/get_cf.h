#ifndef GET_CF_LEVY_FNC
#define GET_CF_LEVY_FNC
#include "CharacteristicFunctions.h"
#include "utils.h"


auto cfLogBase(
    double T
){
    return [=](
        const auto& u,
        double lambda, 
        double muJ, double sigJ,
        double sigma, double v0, 
        double speed,double adaV, 
        double rho
    ){
        
        return chfunctions::cirLogMGF(
            -chfunctions::mertonLogRNCF(u, lambda, muJ, sigJ, 0.0, sigma),
            speed, 
            speed-adaV*rho*u*sigma,
            adaV,
            T,
            v0
        );
        
    };
}

auto cf(
    double r,
    double T
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
        auto cfLogTmp=cfLogBase(T);
        return [=, cfLog=std::move(cfLogTmp)](const auto& u){
            return exp(r*T*u+
                cfLog(u, lambda, muJ, sigJ, sigma, v0, speed, adaV, rho)
            );
        };
    };
}

const std::unordered_map<std::string, std::tuple<swarm_utils::upper_lower<double>, double> > modelParams({
    {"lambda", std::make_tuple(swarm_utils::upper_lower<double>(0.0, 2.0), 1.0)}, 
    {"muJ", std::make_tuple(swarm_utils::upper_lower<double>(-1.0, 1.0), 0.0)}, 
    {"sigJ", std::make_tuple(swarm_utils::upper_lower<double>(0.0, 2.0), .3)}, 
    {"sigma", std::make_tuple(swarm_utils::upper_lower<double>(0.0, 1.0), .3)}, 
    {"v0", std::make_tuple(swarm_utils::upper_lower<double>(0.2, 1.8), 1.0)}, 
    {"speed", std::make_tuple(swarm_utils::upper_lower<double>(0.0, 3.0), .4)}, 
    {"adaV", std::make_tuple(swarm_utils::upper_lower<double>(0.0, 3.0), .5)}, 
    {"rho", std::make_tuple(swarm_utils::upper_lower<double>(-1.0, 1.0), -.4)},
    {"r", std::make_tuple(swarm_utils::upper_lower<double>(0.0, .4), .03)},
    {"S0", std::make_tuple(swarm_utils::upper_lower<double>(0.0, 1000000.0), 50.0)},
    {"T", std::make_tuple(swarm_utils::upper_lower<double>(0.0, 1000000.0), .25)},
    {"numU", std::make_tuple(swarm_utils::upper_lower<double>(5, 10), 7)},
    {"quantile", std::make_tuple(swarm_utils::upper_lower<double>(0, 1), .01)},
});

#endif