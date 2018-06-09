#include "FunctionalUtilities.h"
#include "OptionPricing.h"
#include "CFDistUtilities.h"
#include "BSImpliedVolatility.h"
#include "get_cf.h"
#include "parse_json.h"

constexpr int carrmadanput=0;
constexpr int carrmadancall=1;
constexpr int fangoostput=2;
constexpr int fangoostputdelta=3;
constexpr int fangoostputtheta=4;
constexpr int fangoostputgamma=5;
constexpr int fangoostcall=6;
constexpr int fangoostcalldelta=7;
constexpr int fangoostcalltheta=8;
constexpr int fangoostcallgamma=9;
constexpr int fstsput=10;
constexpr int fstsputdelta=11;
constexpr int fstsputtheta=12;
constexpr int fstsputgamma=13;
constexpr int fstscall=14;
constexpr int fstscalldelta=15;
constexpr int fstscalltheta=16;
constexpr int fstscallgamma=17;
constexpr int VaR=18;
constexpr int density=19;

template<typename CF>
void carr_madan_call(const CF& cf, int numU,  double S0, double r, double T){
    auto ada=.25;
    json_print_density(optionprice::CarrMadanCall(
        numU,  
        ada,
        S0, 
        r, T, 
        cf
    ), optionprice::getCarrMadanStrikes(
        ada, S0, numU
    ));
}
template<typename CF>
void carr_madan_put(const CF& cf, int numU,  double S0, double r, double T){
    auto ada=.25;
    auto prices=optionprice::CarrMadanPut(
        numU,  
        ada,
        S0, 
        r, T,
        cf
    );
    auto strikes=optionprice::getCarrMadanStrikes(
        ada, S0, numU
    );
    json_print_options(prices, strikes, IV::getAllIVByStrikePut(strikes, prices, S0, exp(-r*T), T, .5));
}
template<typename CF>
void fsts_put(const CF& cf, int numU,double K, double r, double T, double xMax){
    auto prices=optionprice::FSTSPrice(
        numU, 
        xMax, 
        r, T,
        [&](const auto& logAsset){
            return K*exp(logAsset);
        },
        [&](const auto& assetValue){
            return assetValue<K?K-assetValue:0.0;
        },
        cf
    );
    auto assets=optionprice::getStrikeUnderlying(
        -xMax, xMax, K, numU
    );
    json_print_options(prices, assets, IV::getAllIVByAssetPut(assets, prices, K, exp(-r*T), T, .5));
}
template<typename CF>
void fsts_call_delta(const CF& cf, int numU, double K, double r, double T, double xMax){
    auto prices=optionprice::FSTSDelta(
        numU, 
        xMax, 
        r, T,
        [&](const auto& logAsset){
            return K*exp(logAsset);
        },
        [&](const auto& assetValue){
            return assetValue>K?assetValue-K:0.0;
        },
        cf
    );
    auto assets=optionprice::getStrikeUnderlying(
        -xMax, xMax, K, numU
    );
    json_print_density(prices, assets);
}
template<typename CF>
void fsts_call_theta(const CF& cf, int numU, double K, double r, double T, double xMax){
    auto prices=optionprice::FSTSTheta(
        numU, 
        xMax, 
        r, T,
        [&](const auto& logAsset){
            return K*exp(logAsset);
        },
        [&](const auto& assetValue){
            return assetValue>K?assetValue-K:0.0;
        },
        cf
    );
    auto assets=optionprice::getStrikeUnderlying(
        -xMax, xMax, K, numU
    );
    json_print_density(prices, assets);
}
template<typename CF>
void fsts_call_gamma(const CF& cf, int numU, double K, double r, double T, double xMax){
    auto prices=optionprice::FSTSGamma(
        numU, 
        xMax, 
        r, T,
        [&](const auto& logAsset){
            return K*exp(logAsset);
        },
        [&](const auto& assetValue){
            return assetValue>K?assetValue-K:0.0;
        },
        cf
    );
    auto assets=optionprice::getStrikeUnderlying(
        -xMax, xMax, K, numU
    );
    json_print_density(prices, assets);
}
template<typename CF>
void fsts_call(const CF& cf, int numU, double K, double r, double T, double xMax){
    json_print_density(optionprice::FSTSPrice(
        numU,  
        xMax,
        r, T,
        [&](const auto& logAsset){
            return K*exp(logAsset);
        },
        [&](const auto& assetValue){
            return assetValue>K?assetValue-K:0.0;
        },
        cf
    ), optionprice::getStrikeUnderlying(
        -xMax, xMax, K, numU
    ));
}
template<typename CF>
void fsts_put_delta(const CF& cf, int numU, double K, double r, double T, double xMax){
    json_print_density(optionprice::FSTSDelta(
        numU,  
        xMax,
        r, T,
        [&](const auto& logAsset){
            return K*exp(logAsset);
        },
        [&](const auto& assetValue){
            return assetValue<K?K-assetValue:0.0;
        },
        cf
    ), optionprice::getStrikeUnderlying(
        -xMax, xMax, K, numU
    ));
}
template<typename CF>
void fsts_put_theta(const CF& cf, int numU, double K, double r, double T, double xMax){
    json_print_density(optionprice::FSTSTheta(
        numU,  
        xMax,
        r, T,
        [&](const auto& logAsset){
            return K*exp(logAsset);
        },
        [&](const auto& assetValue){
            return assetValue<K?K-assetValue:0.0;
        },
        cf
    ), optionprice::getStrikeUnderlying(
        -xMax, xMax, K, numU
    ));
}
template<typename CF>
void fsts_put_gamma(const CF& cf, int numU, double K, double r, double T, double xMax){
    json_print_density(optionprice::FSTSGamma(
        numU,  
        xMax,
        r, T,
        [&](const auto& logAsset){
            return K*exp(logAsset);
        },
        [&](const auto& assetValue){
            return assetValue<K?K-assetValue:0.0;
        },
        cf
    ), optionprice::getStrikeUnderlying(
        -xMax, xMax, K, numU
    ));
}

template<typename CF, typename Array>
void fangoost_put(const CF& cf, Array&& strikes, int numU, double S0, double r, double T, double xMax){
    //auto strikes=get_k_var(parsedJson).k;
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    auto prices=optionprice::FangOostPutPrice(
        S0, strikes,
        r, T,
        numU,  
        cf
    );
    json_print_options(prices, strikes, IV::getAllIVByStrikePut(strikes, prices, S0, exp(-r*T), T, .5));
}
template<typename CF, typename Array>
void fangoost_call_delta(const CF& cf, Array&& strikes, int numU, double S0, double r, double T, double xMax){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_density(
        optionprice::FangOostCallDelta(
            S0, strikes,
            r, T,
            numU,  
            cf
        ), 
        strikes
    );
}

template<typename CF, typename Array>
void fangoost_call_theta(const CF& cf, Array&& strikes, int numU, double S0, double r, double T, double xMax){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_density(
        optionprice::FangOostCallTheta(
            S0, strikes,
            r, T,
            numU,  
            cf
        ), 
        strikes
    );
}
template<typename CF, typename Array>
void fangoost_call_gamma(const CF& cf, Array&& strikes, int numU, double S0, double r, double T, double xMax){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_density(
        optionprice::FangOostCallGamma(
            S0, strikes,
            r, T,
            numU,  
            cf
        ), 
        strikes
    );
}

template<typename CF, typename Array>
void fangoost_call(const CF& cf, Array&& strikes, int numU, double S0,  double r, double T, double xMax){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_density(optionprice::FangOostCallPrice(
        S0, strikes,
        r, T,
        numU,  
        cf
    ), strikes);
}
template<typename CF, typename Array>
void fangoost_put_delta(const CF& cf, Array&& strikes, int numU, double S0, double r, double T, double xMax){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_density(optionprice::FangOostPutDelta(
        S0, strikes,
        r, T, 
        numU,
        cf
    ), strikes);
}
template<typename CF, typename Array>
void fangoost_put_theta(const CF& cf, Array&& strikes, int numU, double S0, double r, double T, double xMax){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_density(optionprice::FangOostPutTheta(
        S0, strikes,
        r, T, 
        numU,
        cf
    ), strikes);
}
template<typename CF, typename Array>
void fangoost_put_gamma(const CF& cf, Array&& strikes, int numU, double S0, double r, double T, double xMax){
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_density(optionprice::FangOostPutGamma(
        S0, strikes,
        r, T, 
        numU,
        cf
    ), strikes);
}
/**const Number& alpha, const Number& prec, const Number& xMin, const Number& xMax, const Index& numU, CF&& cf*/
template<typename CF>
void get_var(const CF& cf, double quantile, int numU, double xMax){
    const auto prec=.0000001;
    const auto extremeVals=cfdistutilities::computeES(
        quantile, prec, -xMax, xMax, 
        numU, cf
    );
    const auto VaR=std::get<cfdistutilities::VAR>(extremeVals);
    const auto ES=std::get<cfdistutilities::ES>(extremeVals);
    json_print_var(VaR, ES);
}
template<typename CF>
void get_density(const CF& cf, int numU, double xMax){
    int numX=128;
    json_print_density(
        fangoost::computeInv(
            numX, numU, -xMax, xMax, cf
        ), 
        fangoost::computeXRange(numX, -xMax, xMax)
    );
}

auto get_jump_diffusion_vol(double sigma, double lambda, double muJ, double sigJ, double T){
    return sqrt((sigma*sigma+lambda*(muJ*muJ+sigJ*sigJ))*T);
}

int main(int argc, char* argv[]){ 
    if(argc>2){
        auto parsedJson=parse_char(argv[2]);
        const auto T=get_ranged_variable(parsedJson, modelParams, "T");
        const auto r=get_ranged_variable(parsedJson, modelParams, "r");
        const auto S0=get_ranged_variable(parsedJson, modelParams, "S0");
        const auto lambda=get_ranged_variable(parsedJson, modelParams, "lambda");
        const auto muJ=get_ranged_variable(parsedJson, modelParams, "muJ");
        const auto sigJ=get_ranged_variable(parsedJson, modelParams, "sigJ");
        const auto sigma=get_ranged_variable(parsedJson, modelParams, "sigma");
        const int numU=pow(2, (int)get_ranged_variable(parsedJson, modelParams, "numU"));
        const auto instantiatedCf=cf(
            r,
            T
        )(
            lambda,
            muJ,
            sigJ,
            sigma,
            get_ranged_variable(parsedJson, modelParams, "v0"),
            get_ranged_variable(parsedJson, modelParams, "speed"),
            get_ranged_variable(parsedJson, modelParams, "adaV"),
            get_ranged_variable(parsedJson, modelParams, "rho")
        );
        /**NOTE that this is a big assumption about the
         * domain for these distributions.
         * Be careful!*/
        double xMaxDensity=get_jump_diffusion_vol(sigma, lambda, muJ, sigJ, T)*5.0;
        double xMaxOptions=xMaxDensity*2.0; 
        int key=std::stoi(argv[1]);
        switch(key){
            case carrmadanput: {
                carr_madan_put(
                    instantiatedCf, numU, 
                    S0, 
                    r, 
                    T
                ); 
                break;
            }
            case carrmadancall: {
                carr_madan_call(
                    instantiatedCf, numU, 
                    S0, 
                    r, 
                    T
                ); 
                break;
            }
            case fstscall: {
                fsts_call(
                    instantiatedCf, numU, 
                    S0,
                    r,
                    T, xMaxOptions
                );
                break;
            }
            case fstscalldelta: {
                fsts_call_delta(
                    instantiatedCf, numU, 
                    S0,
                    r, T,
                    xMaxOptions
                );
                break;
            }
            case fstscalltheta: {
                fsts_call_theta(
                    instantiatedCf, numU, 
                    S0,
                    r, T,
                    xMaxOptions
                );
                break;
            }
            case fstscallgamma: {
                fsts_call_gamma(
                    instantiatedCf, numU, 
                    S0,
                    r, T,
                    xMaxOptions
                );
                break;
            }
            case fstsput: {
                fsts_put(
                    instantiatedCf, numU, 
                    S0,
                    r, T,
                    xMaxOptions
                );
                break;
            }
            case fstsputdelta: {
                fsts_put_delta(
                    instantiatedCf, numU, 
                    S0,
                    r, T,
                    xMaxOptions
                );
                break;
            }
            case fstsputtheta: {
                fsts_put_theta(
                    instantiatedCf, numU, 
                    S0,
                    r, T,
                    xMaxOptions
                );
                break;
            }
            case fstsputgamma: {
                fsts_put_gamma(
                    instantiatedCf, numU, 
                    S0,
                    r, T,
                    xMaxOptions
                );
                break;
            }
            case fangoostcall: {
                fangoost_call(
                    instantiatedCf, get_k_var<std::deque<double> >(parsedJson), 
                    numU, 
                    S0, r,
                    T, xMaxOptions
                );
                break;
            }
            case fangoostcalldelta: {
                fangoost_call_delta(
                    instantiatedCf, get_k_var<std::deque<double> >(parsedJson), 
                    numU, 
                    S0,r,
                    T, xMaxOptions
                );
                break;
            }
            case fangoostcalltheta: {
                fangoost_call_theta(
                    instantiatedCf, get_k_var<std::deque<double> >(parsedJson), 
                    numU, 
                    S0, r,
                    T, xMaxOptions
                );
                break;
            }
            case fangoostcallgamma: {
                fangoost_call_gamma(
                    instantiatedCf, get_k_var<std::deque<double> >(parsedJson), 
                    numU, 
                    S0, r,
                    T, xMaxOptions
                );
                break;
            }
            case fangoostput: {
                fangoost_put(
                    instantiatedCf, get_k_var<std::deque<double> >(parsedJson), 
                    numU,
                    S0, 
                    r,
                    T, xMaxOptions
                );
                break;
            }
            case fangoostputdelta: {
                fangoost_put_delta(
                    instantiatedCf, get_k_var<std::deque<double> >(parsedJson), 
                    numU,
                    S0, 
                    r,
                    T, xMaxOptions
                );
                break;
            }
            case fangoostputtheta: {
                fangoost_put_theta(
                    instantiatedCf, get_k_var<std::deque<double> >(parsedJson), 
                    numU,
                    S0, 
                    r,
                    T, xMaxOptions
                );
                break;
            }
            case fangoostputgamma: {
                fangoost_put_gamma(
                    instantiatedCf, get_k_var<std::deque<double> >(parsedJson), 
                    numU,
                    S0, 
                    r,
                    T, xMaxOptions
                );
                break;
            }
            case VaR: {
                get_var(
                    instantiatedCf, get_ranged_variable(parsedJson, modelParams, "quantile"), 
                    numU, xMaxDensity
                );
                break;
            }
            case density: {
                get_density(
                    instantiatedCf, numU, 
                    xMaxDensity
                );
                break;
            }
        }



    }


    return 0;
}