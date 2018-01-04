#include "FunctionalUtilities.h"
#include "OptionPricing.h"
#include "CFDistUtilities.h"
#include "BSImpliedVolatility.h"
#include "get_cf.h"
#include "parse_json.h"

const int carrmadanput=0;
const int carrmadancall=1;
const int fangoostput=2;
const int fangoostputdelta=3;
const int fangoostcall=4;
const int fangoostcalldelta=5;
const int fstsput=6;
const int fstsputdelta=7;
const int fstscall=8;
const int fstscalldelta=9;

const int VaR=10;
const int density=11;

template<typename CF>
void carr_madan_put(const CF& cf, int numU,  double S0, double r, double T){
    auto ada=.25;
    json_print_density(optionprice::CarrMadanPut(
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
void carr_madan_call(const CF& cf, int numU,  double S0, double r, double T){
    auto ada=.25;
    auto prices=optionprice::CarrMadanCall(
        numU,  
        ada,
        S0, 
        r, T,
        cf
    );
    auto strikes=optionprice::getCarrMadanStrikes(
        ada, S0, numU
    );
    json_print_options(prices, strikes, IV::getAllIVByStrike(strikes, prices, S0, exp(-r*T), T, .5));
}
/**auto payoff=[&](const auto& assetValue){
        return assetValue>K?assetValue-K:0.0;
    };
    auto getAssetPrice=[&](const auto& logAsset){
        return K*exp(logAsset);
    };*/
template<typename CF>
void fsts_call(const CF& cf, int numU,double K, double r, double T, double xMax){
    auto prices=optionprice::FSTSPrice(
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
    json_print_options(prices, assets, IV::getAllIVByAsset(assets, prices, K, exp(-r*T), T, .5));
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
void fsts_put(const CF& cf, int numU, double K, double r, double T, double xMax){
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
            return assetValue>K?assetValue-K:0.0;
        },
        cf
    ), optionprice::getStrikeUnderlying(
        -xMax, xMax, K, numU
    ));
}

template<typename CF, typename ParsedJ>
void fangoost_call(const CF& cf, const ParsedJ& parsedJson, int numU, double S0, double r, double T, double xMax){
    auto strikes=get_k_var(parsedJson).k;
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    auto prices=optionprice::FangOostCallPrice(
        S0, strikes,
        r, T,
        numU,  
        cf
    );
    json_print_options(prices, strikes, IV::getAllIVByStrike(strikes, prices, S0, exp(-r*T), T, .5));
}
template<typename CF, typename ParsedJ>
void fangoost_call_delta(const CF& cf, const ParsedJ& parsedJson, int numU, double S0, double r, double T, double xMax){

    auto strikes=get_k_var(parsedJson).k;
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

template<typename CF, typename ParsedJ>
void fangoost_put(const CF& cf, const ParsedJ& parsedJson, int numU, double S0,  double r, double T, double xMax){
    auto strikes=get_k_var(parsedJson).k;
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_density(optionprice::FangOostPutPrice(
        S0, strikes,
        r, T,
        numU,  
        cf
    ), strikes);
}
template<typename CF, typename ParsedJ>
void fangoost_put_delta(const CF& cf, const ParsedJ& parsedJson, int numU, double S0, double r, double T, double xMax){
    auto strikes=get_k_var(parsedJson).k;
    strikes.push_front(exp(xMax)*S0);
    strikes.push_back(exp(-xMax)*S0);
    json_print_density(optionprice::FangOostPutDelta(
        S0, strikes,
        r, T, 
        numU,
        cf
    ), strikes);
}
template<typename CF, typename ParsedJ>
void get_var(const CF& cf, const ParsedJ& parsedJson, int numU, double xMax){
    auto dist_var=get_dist_variables(parsedJson);
    auto prec=.0000001;
    auto var=cfdistutilities::computeVaR(dist_var.quantile, prec, -xMax, xMax, numU, cf);
    auto ES=cfdistutilities::computeES(
        dist_var.quantile,
        -xMax, xMax, var, numU, cf, true
    );
    json_print_var(var, ES);
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
auto get_cgmy_vol(double sigma, double c, double y, double m, double g, double t){
    return sqrt(t*(sigma*sigma+c*tgamma(2.0-y)*(1.0/pow(m, 2.0-y)+1.0/pow(g, 2.0-y))));
}

int main(int argc, char* argv[]){
    if(argc>2){
        auto parsedJson=parse_char(argv[2]);
        auto options=get_option_var(parsedJson);
        auto cgmyCF=cf(
            options.r,
            options.sigma,
            options.T,
            options.S0,
            options.C,
            options.G,
            options.M,
            options.Y,
            options.speed,
            options.v0,
            options.adaV,
            options.rho
        );
        /**NOTE that this is a big assumption about the
         * domain for these distributions.
         * Be careful!*/
        double xMaxDensity=get_cgmy_vol(options.sigma, options.C, options.Y, options.M, options.G, options.T)*5.0;
        double xMaxOptions=xMaxDensity*2.0;
        int numU=pow(2, options.numU);
        int key=std::stoi(argv[1]);
        switch(key){
            case carrmadanput: {
                carr_madan_put(
                    cgmyCF, numU, 
                    options.S0, 
                    options.r, 
                    options.T
                ); 
                break;
            }
            case carrmadancall: {
                carr_madan_call(
                    cgmyCF, numU, 
                    options.S0, 
                    options.r, 
                    options.T
                ); 
                break;
            }
            case fstscall: {
                fsts_call(
                    cgmyCF, numU, 
                     options.S0,
                    options.r,
                    options.T, xMaxOptions
                );
                break;
            }
            case fstscalldelta: {
                fsts_call_delta(
                    cgmyCF, numU, 
                    options.S0,
                    options.r, options.T,
                    xMaxOptions
                );
                break;
            }
            case fstsput: {
                fsts_put(
                    cgmyCF, numU, 
                    options.S0,
                    options.r, options.T,
                    xMaxOptions
                );
                break;
            }
            case fstsputdelta: {
                fsts_put_delta(
                    cgmyCF, numU, 
                    options.S0,
                    options.r, options.T,
                    xMaxOptions
                );
                break;
            }
            case fangoostcall: {
                fangoost_call(
                    cgmyCF, parsedJson, 
                    numU, 
                    options.S0, options.r,
                    options.T, xMaxOptions
                );
                break;
            }
            case fangoostcalldelta: {
                fangoost_call_delta(
                    cgmyCF, parsedJson, 
                    numU, 
                    options.S0, options.r,
                    options.T, xMaxOptions
                );
                break;
            }
            case fangoostput: {
                fangoost_put(
                    cgmyCF, parsedJson, 
                    numU,
                    options.S0, 
                    options.r,
                    options.T, xMaxOptions
                );
                break;
            }
            case fangoostputdelta: {
                fangoost_put_delta(
                    cgmyCF, parsedJson, 
                    numU,
                    options.S0, 
                    options.r,
                    options.T, xMaxOptions
                );
                break;
            }
            case VaR: {
                get_var(
                    cgmyCF, parsedJson, 
                    numU, xMaxDensity
                );
                break;
            }
            case density: {
                get_density(
                    cgmyCF, numU, 
                    xMaxDensity
                );
                break;
            }
        }



    }


    return 0;
}