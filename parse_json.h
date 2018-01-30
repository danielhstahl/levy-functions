#include <iostream>
#include <cmath>
#include "document.h" //rapidjson
#include "writer.h" //rapidjson
#include "stringbuffer.h" //rapidjson
#include <deque>
#include <unordered_map>

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

std::deque<double> get_k_var(const rapidjson::Document& parms){
    std::deque<double> local_k;
    if(parms.FindMember("k")!=parms.MemberEnd()){
        for (auto& v : parms["k"].GetArray()) {
            local_k.emplace_back(v.GetDouble());
        }
    }
    return local_k;
}
std::vector<double> get_prices_var(const rapidjson::Document& parms){
    std::vector<double> local_prices;
    if(parms.FindMember("prices")!=parms.MemberEnd()){
        for (auto& v : parms["prices"].GetArray()) {
            local_prices.emplace_back(v.GetDouble());
        }
    }
    return local_prices;
}

double get_quantile(const rapidjson::Document& parms){
    double local_quantile;
    if(parms.FindMember("quantile")!=parms.MemberEnd()){
        local_quantile=between_values(parms["quantile"].GetDouble(), 0.0, 1.0);
    }
    return local_quantile;
}

option_variables get_static_vars(const rapidjson::Document& parms){
    option_variables local_option;
    if(parms.FindMember("numU")!=parms.MemberEnd()){
        local_option.numU=between_values(parms["numU"].GetInt(), 5, 10);
    }
    if(parms.FindMember("r")!=parms.MemberEnd()){
        local_option.r=between_values(parms["r"].GetDouble(), 0.0, maxLarge);
    }
    if(parms.FindMember("T")!=parms.MemberEnd()){
        local_option.T=between_values(parms["T"].GetDouble(), 0.0, maxLarge);
    }
    if(parms.FindMember("S0")!=parms.MemberEnd()){
        local_option.S0=between_values(parms["S0"].GetDouble(), 0.0, maxLarge);
    }
    return std::move(local_option);
}

template<typename JsonParm>
int updateIndex(const JsonParm& parms, const std::string& key, int index, std::unordered_map<std::string, int>* mapKeyToIndex){
    if(parms.HasMember(key.c_str())){
        mapKeyToIndex->insert(std::make_pair(key, index));
        return index+1;
    }
    return index;
    
}

template< typename JsonVariable>
std::unordered_map<std::string, int> constructKeyToIndex(JsonVariable&& variableVar){
    std::unordered_map<std::string, int> mapKeyToIndex;
    int index=0;
    index=updateIndex(variableVar, "C", index, &mapKeyToIndex);
    index=updateIndex(variableVar, "G", index, &mapKeyToIndex);
    index=updateIndex(variableVar, "M", index, &mapKeyToIndex);
    index=updateIndex(variableVar, "Y", index, &mapKeyToIndex);
    index=updateIndex(variableVar, "sigma", index, &mapKeyToIndex);
    index=updateIndex(variableVar, "v0", index, &mapKeyToIndex);
    index=updateIndex(variableVar, "speed", index, &mapKeyToIndex);
    index=updateIndex(variableVar, "adaV", index, &mapKeyToIndex);
    index=updateIndex(variableVar, "rho", index, &mapKeyToIndex);
    return mapKeyToIndex;
}


template<typename JsonStatic>
double getArgOrConstant(
    const std::string& key, 
    const std::vector<double>& args, 
    JsonStatic&& staticVar,
    const std::unordered_map<std::string, int>& mapKeyToIndex
){
    return 
        staticVar.FindMember(key.c_str())!=staticVar.MemberEnd()?
        staticVar[key.c_str()].GetDouble():
        args[mapKeyToIndex.at(key)];
}


option_variables get_option_var(const rapidjson::Document& parms){
    
    option_variables local_option=get_static_vars(parms);
    if(parms.FindMember("sigma")!=parms.MemberEnd()){
        local_option.sigma=between_values(parms["sigma"].GetDouble(), 0.0, maxLarge);
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
        local_option.Y=between_values(parms["Y"].GetDouble(), -2.0, 2.0);
    }
    if(parms.FindMember("speed")!=parms.MemberEnd()){
        local_option.speed=between_values(parms["speed"].GetDouble(), 0.0, maxLarge);
    }
    if(parms.FindMember("v0")!=parms.MemberEnd()){
        local_option.v0=between_values(parms["v0"].GetDouble(), 0.0, maxLarge);
    }
    if(parms.FindMember("adaV")!=parms.MemberEnd()){
        local_option.adaV=between_values(parms["adaV"].GetDouble(), 0.0, maxLarge);
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

template<int optparms, int fnval, typename Array1, typename TupleOfArrayAndValue>
void json_print_calibrated_params(const Array1& paramNames, TupleOfArrayAndValue&& optimResults, int totalOptions){
    auto params=std::get<optparms>(optimResults);
    auto fnVal=std::get<fnval>(optimResults);
    //auto n=paramNames.size();
    int i=0;
    std::cout<<"{";
    for( const auto& node : paramNames ) {
        std::cout<<"\""<<node.first<<"\":"<<params[node.second]<<",";
        ++i;
    }
    std::cout<<"\"mse\":"<<sqrt(fnVal/totalOptions)<<"}";
}


void json_print_var(double var, double es){
    std::cout<<"{\"VaR\":"<<var<<",\"ES\":"<<es<<"}"<<std::endl;
}

