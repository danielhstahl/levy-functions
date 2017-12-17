#include <iostream>
#include <cmath>
#include "document.h" //rapidjson
#include "writer.h" //rapidjson
#include "stringbuffer.h" //rapidjson
#include <deque>
struct option_variables{
    int numU=6;//gets raised to power of 2: 2^numU
    double r=.03;
    double T=.25;
    double S0=50;
    double sigma=.2;
    double C=1.0;
    double G=1.4;
    double M=2.5;
    double Y=.6;
    double speed=.4;
    double v0=1.05;
    double adaV=.2;
    double rho=-.5;
};
struct dist_variables{
    double quantile=.01;
};
struct discrete_k_variables{
    std::deque<double> k;
};
template<typename T>
T between_values(const T& val, const T& lower, const T& upper){
    return val<lower?lower:(val>upper?upper:val);
}
const double maxLarge=1000000.0;
rapidjson::Document parse_char(char* json){
    rapidjson::Document parms;
    parms.Parse(json);
    return parms;
}

discrete_k_variables get_k_var(const rapidjson::Document& parms){
    discrete_k_variables local_k;
    if(parms.FindMember("k")!=parms.MemberEnd()){
        for (auto& v : parms["k"].GetArray()) {
            local_k.k.emplace_back(v.GetDouble());
        }
    }
    return local_k;
}
dist_variables get_dist_variables(const rapidjson::Document& parms){
    dist_variables local_dist;
    if(parms.FindMember("quantile")!=parms.MemberEnd()){
        local_dist.quantile=between_values(parms["quantile"].GetDouble(), 0.0, 1.0);
    }
    return local_dist;
}


option_variables get_option_var(const rapidjson::Document& parms){
    option_variables local_option;
    if(parms.FindMember("numU")!=parms.MemberEnd()){
        local_option.numU=between_values(parms["numU"].GetInt(), 16, 1024);
    }
    if(parms.FindMember("r")!=parms.MemberEnd()){
        local_option.r=between_values(parms["r"].GetDouble(), 0.0, 1.0);
    }
    if(parms.FindMember("T")!=parms.MemberEnd()){
        local_option.T=between_values(parms["T"].GetDouble(), 0.0, 25.0);
    }
    if(parms.FindMember("S0")!=parms.MemberEnd()){
        local_option.S0=between_values(parms["S0"].GetDouble(), 0.0, maxLarge);
    }
    if(parms.FindMember("sigma")!=parms.MemberEnd()){
        local_option.sigma=between_values(parms["sigma"].GetDouble(), 0.0, 5.0);
    }
    if(parms.FindMember("C")!=parms.MemberEnd()){
        auto result=
        local_option.C=between_values(parms["C"].GetDouble(), 0.0, maxLarge);

    }
    if(parms.FindMember("G")!=parms.MemberEnd()){
        local_option.G=between_values(parms["G"].GetDouble(), 0.0, maxLarge);
    }
    if(parms.FindMember("M")!=parms.MemberEnd()){
        local_option.M=between_values(parms["M"].GetDouble(), 0.0, maxLarge);
    }
    if(parms.FindMember("Y")!=parms.MemberEnd()){
        local_option.Y=between_values(parms["Y"].GetDouble(), 0.0, 2.0);
    }
    if(parms.FindMember("speed")!=parms.MemberEnd()){
        local_option.speed=between_values(parms["speed"].GetDouble(), 0.0, 1.5);
    }
    if(parms.FindMember("v0")!=parms.MemberEnd()){
        local_option.v0=between_values(parms["v0"].GetDouble(), 0.0, 2.0);
    }
    if(parms.FindMember("adaV")!=parms.MemberEnd()){
        local_option.adaV=between_values(parms["adaV"].GetDouble(), 0.0, 2.0);
    }
    if(parms.FindMember("rho")!=parms.MemberEnd()){
        local_option.rho=between_values(parms["rho"].GetDouble(), -1.0, 1.0);
    }
    return local_option;
}

template<typename Array1, typename Array2, typename Array3>
void json_print_options(const Array1& arr1, const Array2& arr2, const Array3& arr3){
    auto n=arr1.size();
    std::cout<<"[";
    for(int i=0; i<n-1;++i){
        std::cout<<"{\"value\":"<<arr1[i]<<",\"atPoint\":"<<arr2[i]<<",\"iv\":"<<arr3[i]<<"},";
    }
    std::cout<<"{\"value\":"<<arr1[n-1]<<",\"atPoint\":"<<arr2[n-1]<<",\"iv\":"<<arr3[n-1]<<"}]";
}
template<typename Array1, typename Array2>
void json_print_density(const Array1& arr1, const Array2& arr2){
    auto n=arr1.size();
    std::cout<<"[";
    for(int i=0; i<n-1;++i){
        std::cout<<"{\"value\":"<<arr1[i]<<",\"atPoint\":"<<arr2[i]<<"},";
    }
    std::cout<<"{\"value\":"<<arr1[n-1]<<",\"atPoint\":"<<arr2[n-1]<<"}]";
}

/*template<typename Array1, typename Array2>
void json_print_options(const Array1& prices, const Array2& atPoints){
    std::cout<<"{\"values\":";
    print_array(prices);
    std::cout<<", \"atPoints\":";
    print_array(atPoints);
    std::cout<<"}"<<std::endl;
}*/

void json_print_var(double var, double es){
    std::cout<<"{\"VaR\":"<<var<<",\"ES\":"<<es<<"}"<<std::endl;
}

