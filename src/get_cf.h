#ifndef GET_CF_LEVY_FNC
#define GET_CF_LEVY_FNC
#include "CharacteristicFunctions.h"
#include "cuckoo.h"

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
        return [=](const auto& u){
            return exp(r*T*u+
                cfLogBase(u, T, lambda, muJ, sigJ, sigma, v0, speed, adaV, rho)
            );
        };
    };
    
}



/**should be the same as cf when delta=0*/
auto cfLogGeneric(
    double r,
    double T
){
    return [=](
        double lambda, 
        double muJ, double sigJ,
        double sigma, double v0, 
        double speed,double adaV, 
        double rho, double q, 
        double delta
    ){
        
        auto numODE=40;//hopefully this is sufficient
         
        double speedTmp=speed;//copy here in order to move to move
        //const T& rho, const T& K, const T& H, const T& l
        auto alpha=chfunctions::AlphaOrBeta_move(0.0, std::move(speedTmp), 0.0, 0.0);
        return [=, alpha=std::move(alpha), numODE=std::move(numODE)](const auto& u){
            //const T& rho, const T& K, const T& H, const T& l
            auto beta=chfunctions::AlphaOrBeta_move(
                -chfunctions::mertonLogRNCF(u, lambda, muJ, sigJ, 0.0, sigma), 
                -(speed+(delta*lambda)/q-u*rho*sigma*adaV),
                adaV*adaV, 
                std::move(lambda)
            );
            
            auto expCF=chfunctions::exponentialCF(u, q);
            return chfunctions::logAffine(
                rungekutta::computeFunctional_move(T, numODE, std::vector<std::complex<double> >({0, 0}),
                    [&](double t, const std::vector<std::complex<double> >& x){
                        auto cfPart=chfunctions::exponentialCF(
                            u+x[0]*delta, 
                            q
                        )-expCF;
                        return std::vector<std::complex<double> >({
                            beta(x[0], cfPart),
                            alpha(x[0], cfPart)
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
        double rho, double q, 
        double delta
    ){
        auto logCF=cfLogGeneric(r, T)(lambda, muJ, sigJ, sigma ,v0, speed, adaV, rho, q, delta);
        return [=, logCF=std::move(logCF)](const auto& u){
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
    {"q", std::make_tuple(cuckoo::upper_lower<double>(0, 20), 5)},
    {"delta", std::make_tuple(cuckoo::upper_lower<double>(0, 2), 1)}
});

#endif