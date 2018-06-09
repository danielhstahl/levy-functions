#ifndef PARSE_JSON_LEVY_FNC
#define PARSE_JSON_LEVY_FNC

#include <iostream>
#include <cmath>
#include "utils.h"
#include "document.h" //rapidjson
#include "writer.h" //rapidjson
#include "stringbuffer.h" //rapidjson
#include <deque>
#include "get_cf.h"//for modelParams
#include <unordered_map>

template<typename T>
T between_values(const T& val, const T& lower, const T& upper){
    return val<lower?lower:(val>upper?upper:val);
}
rapidjson::Document parse_char(char* json){
    rapidjson::Document parms;
    parms.Parse(json);
    return parms;
}

template<typename vecT>
vecT get_k_var(const rapidjson::Document& parms){
    vecT local_k;
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

constexpr int RANGE=0;
constexpr int DEFAULTVALUE=1;
template<typename CustomMap>
auto get_ranged_variable(const rapidjson::Document& parms, const CustomMap& defaultParameters, const std::string& key){
    const auto paramRanges=defaultParameters.at(key);
    if(parms.HasMember(key.c_str())){
        const auto cuckooRange=std::get<RANGE>(paramRanges);
        return between_values(parms[key.c_str()].GetDouble(), cuckooRange.lower, cuckooRange.upper);
    }
    else{
        return std::get<DEFAULTVALUE>(paramRanges);
    }
}

template<typename Arr1, typename Json>
constexpr bool hasAllVariables(const Json& json, Arr1&& arr){
    return json.HasMember(arr);
}
template<typename Arr1, typename Json, typename ...Arrs>
constexpr bool hasAllVariables(const Json& json, Arr1&& arr, Arrs&&... arrs){
    return hasAllVariables(json, std::move(arr))&&hasAllVariables(json, arrs...);
}


template<typename RpJson, typename Array1, typename Array2, typename Object>
std::vector<swarm_utils::upper_lower<double> > getConstraints(const RpJson& json, const Array1& possibleParameters, const Array2& fullModelConstraints, Object&& optionalConstraints ){
    std::vector<swarm_utils::upper_lower<double> > modelConstraints;
    for(auto& v:possibleParameters){
        if(json.HasMember(v.c_str())){
            if(optionalConstraints.HasMember(v.c_str())){
                const auto& constraint=optionalConstraints[v.c_str()];
                modelConstraints.emplace_back(swarm_utils::upper_lower<double>(constraint["lower"].GetDouble(), constraint["upper"].GetDouble()));
            }
            else{
                modelConstraints.emplace_back(std::get<0>(fullModelConstraints.at(v)));
            }
        }
    }
    return modelConstraints;
}

template<typename JsonParm>
int updateIndex(const JsonParm& parms, const std::string& key, int index, std::unordered_map<std::string, int>* mapKeyToIndex){
    if(parms.HasMember(key.c_str())){
        mapKeyToIndex->insert(std::make_pair(key, index));
        return index+1;
    }
    return index;
    
}

template< typename JsonVariable, typename Array>
std::unordered_map<std::string, int> constructKeyToIndex(JsonVariable&& variableVar, const Array& possibleParameters ){
    std::unordered_map<std::string, int> mapKeyToIndex;
    int index=0;
    for(auto& v:possibleParameters){
        index=updateIndex(variableVar, v, index, &mapKeyToIndex);
    }
    return mapKeyToIndex;
}

template< typename JsonVariable, typename Array>
auto constructStaticKeyToValue(JsonVariable&& staticVar, const Array& possibleParameters ){
    std::unordered_map<std::string, double> mapKeyToValue;
    std::unordered_map<std::string, bool> mapKeyToExists;
    for(auto& v:possibleParameters){
        if(staticVar.HasMember(v.c_str())){
            mapKeyToExists.emplace(v, true);
            mapKeyToValue.emplace(v, staticVar[v.c_str()].GetDouble());
        }
        else{
            mapKeyToExists.emplace(v, false);
        }
    }
    return std::make_tuple(mapKeyToExists, mapKeyToValue);
}

double getArgOrConstant(
    const std::string& key, 
    const std::vector<double>& args, 
    const std::unordered_map<std::string, int>& mapKeyToIndexVariable,
    const std::unordered_map<std::string, bool>& mapKeyToExistsStatic,
    const std::unordered_map<std::string, double>& mapKeyToValueStatic
){
    return 
        mapKeyToExistsStatic.at(key)?
        mapKeyToValueStatic.at(key):
        args[mapKeyToIndexVariable.at(key)];
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
    std::cout<<"{";
    for( const auto& node : paramNames ) {
        std::cout<<"\""<<node.first<<"\":"<<params[node.second]<<",";
    }
    std::cout<<"\"mse\":"<<fnVal<<"}";
}

void json_print_default_parameters(){
    std::cout<<"{";
    int n=modelParams.size();
    int i=0;
    for(auto& v:modelParams){
        i++;
        auto ranges=std::get<RANGE>(v.second);
        if(i==n){
            std::cout<<"\""<<v.first<<"\":"<<"{\"upper\":"<<ranges.upper<<", \"lower\":"<<ranges.lower<<"}";
        }
        else{
            std::cout<<"\""<<v.first<<"\":"<<"{\"upper\":"<<ranges.upper<<", \"lower\":"<<ranges.lower<<"},";
        }
        
    }
    std::cout<<"}";
}


void json_print_var(double var, double es){
    std::cout<<"{\"VaR\":"<<var<<",\"ES\":"<<es<<"}"<<std::endl;
}
template<typename Domain, typename Range>
void json_print_spline(int n, double min, double max, double dx, Domain&& fnDomain, Range&& fnRange){
    std::cout<<"[";
    for(int i=0; i<(n-1); ++i){
        double x=min+i*dx;
        std::cout<<"{\"logStrike\":"<<fnDomain(x)<<", \"transformedOption\":"<<fnRange(x)<<"},";
    }
    double x=min+(n-1)*dx;
    std::cout<<"{\"logStrike\":"<<fnDomain(x)<<", \"transformedOption\":"<<fnRange(x)<<"}]";
}
template<typename Domain, typename Range>
void json_print_spline(const std::vector<double>& domain, Domain&& fnDomain, Range&& fnRange){
    std::cout<<"[";
    int n=domain.size();
    for(int i=0; i<(n-1); ++i){
        double x=domain[i];
        std::cout<<"{\"logStrike\":"<<fnDomain(x, i)<<", \"transformedOption\":"<<fnRange(x, i)<<"},";
    }
    double x=domain.back();
    std::cout<<"{\"logStrike\":"<<fnDomain(x, n-1)<<", \"transformedOption\":"<<fnRange(x,  n-1)<<"}]";
}

template<typename Fn, typename Str>
void json_print_multiple_obj_helper(Str&& key, Fn&& fn){
    std::cout<<"\""<<key<<"\":";
    fn();
    std::cout<<"}";
}
template<typename Fn, typename Str, typename ...Fns>
void json_print_multiple_obj_helper(Str&& key, Fn&& fn, Fns&&... fns){
    std::cout<<"\""<<key<<"\":";
    fn();
    std::cout<<", ";
    json_print_multiple_obj_helper(fns...);
}
template<typename ...Fns>
void json_print_multiple_obj(Fns&&... fns){
    std::cout<<"{";
    json_print_multiple_obj_helper(fns...);
}

#endif