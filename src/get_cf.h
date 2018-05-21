#ifndef GET_CF_LEVY_FNC
#define GET_CF_LEVY_FNC
#include "CharacteristicFunctions.h"
#include "cuckoo.h"


auto cfLogBase(
    double T
){
    return [=](
        double lambda, 
        double muJ, double sigJ,
        double sigma, double v0, 
        double speed,double adaV, 
        double rho
    ){
        return [=](const auto& u){
            return chfunctions::cirLogMGF(
                -chfunctions::mertonLogRNCF(u, lambda, muJ, sigJ, 0.0, sigma),
                speed, 
                speed-adaV*rho*u*sigma,
                adaV,
                T,
                v0
            );
        };
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
        auto cfLogTmp=cfLogBase(T)(lambda, muJ, sigJ, sigma, v0, speed, adaV, rho);
        return [r, T, cfLog=std::move(cfLogTmp)](const auto& u){
            return exp(r*T*u+
                cfLog(u)
            );
        };
    };
    
}



/**should be the same as cf when delta=0*/
auto cfLogGeneric(
    double T
){
    return [=](
        double lambda, 
        double muJ, double sigJ,
        double sigma, double v0, 
        double speed, double adaV, 
        double rho, double delta
    ){

        auto numODE=40;//hopefully this is sufficient
        //double speedTmp=speed;//copy here in order to move to move
        //const T& rho, const T& K, const T& H, const T& l
        auto alpha=chfunctions::AlphaOrBeta_move(0.0, speed, 0.0, 0.0);
        return [=, alpha=std::move(alpha), numODE=std::move(numODE)](const auto& u){
            //const T& rho, const T& K, const T& H, const T& l
            auto beta=chfunctions::AlphaOrBeta_move(
                -chfunctions::mertonLogRNCF(u, lambda, muJ, sigJ, 0.0, sigma), 
                -(speed+delta*lambda-u*rho*sigma*adaV),
                adaV*adaV, 
                lambda
            );
            
            auto expCF=chfunctions::exponentialCFBeta(u, delta);
            return chfunctions::logAffine(
                rungekutta::compute_efficient_2d(
                    T, numODE, 
                    std::vector<std::complex<double> >({0, 0}),
                    [&](
                        double t, 
                        const std::complex<double>& x1,  
                        const std::complex<double>& x2
                    ){
                        auto cfPart=(chfunctions::exponentialCFBeta(
                            x1, 
                            delta
                        )-1.0)*chfunctions::gaussCF(u, muJ, sigJ);
                        return std::vector<std::complex<double> >({
                            beta(x1, cfPart),
                            alpha(x1, cfPart)
                        });
                    }
                ),
                v0
            );
        };
    };
}

auto cfGeneric(
    double r,
    double T
){
    return [=](
        double lambda, 
        double muJ, double sigJ,
        double sigma, double v0, 
        double speed,double adaV, 
        double rho,
        double delta
    ){
        auto logCF=cfLogGeneric(T)(lambda, muJ, sigJ, sigma ,v0, speed, adaV, rho, delta);
        return [r, T, logCF=std::move(logCF)](const auto& u){
            return exp(r*u*T+logCF(u));
        };
    };
}
const std::unordered_map<std::string, std::tuple<cuckoo::upper_lower<double>, double> > modelParams({
    {"lambda", std::make_tuple(cuckoo::upper_lower<double>(0.0, 2.0), 1.0)}, 
    {"muJ", std::make_tuple(cuckoo::upper_lower<double>(-1.0, 1.0), 0.0)}, 
    {"sigJ", std::make_tuple(cuckoo::upper_lower<double>(0.0, 2.0), .3)}, 
    {"sigma", std::make_tuple(cuckoo::upper_lower<double>(0.0, 1.0), .3)}, 
    {"v0", std::make_tuple(cuckoo::upper_lower<double>(0.2, 1.8), 1.0)}, 
    {"speed", std::make_tuple(cuckoo::upper_lower<double>(0.0, 3.0), .4)}, 
    {"adaV", std::make_tuple(cuckoo::upper_lower<double>(0.0, 3.0), .5)}, 
    {"rho", std::make_tuple(cuckoo::upper_lower<double>(-1.0, 1.0), -.4)},
    {"r", std::make_tuple(cuckoo::upper_lower<double>(0.0, .4), .03)},
    {"S0", std::make_tuple(cuckoo::upper_lower<double>(0.0, 1000000.0), 50.0)},
    {"T", std::make_tuple(cuckoo::upper_lower<double>(0.0, 1000000.0), .25)},
    {"numU", std::make_tuple(cuckoo::upper_lower<double>(5, 10), 7)},
    {"delta", std::make_tuple(cuckoo::upper_lower<double>(0, 2), 1)}
});

#endif