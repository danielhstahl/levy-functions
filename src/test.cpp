#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "parse_json.h"
#include "get_cf.h" //modelParams

TEST_CASE("parses option with no input", "parse_json"){
    //option_var myTestOption;
    auto parsedJson=parse_char((char*)"{}");
    REQUIRE(get_ranged_variable(parsedJson, modelParams, "r")==.03);
}

TEST_CASE("parses k with some input", "parse_json"){
    auto parsedJson=parse_char((char*)"{\"k\":[0.04, 0.05]}");
    REQUIRE(get_k_var<std::vector<double>>(parsedJson)[0]==.04);
}

TEST_CASE("creates an index where it exists", "parse_json"){
    auto parsedJson=parse_char((char*)"{\"k\":5.0}");
    std::unordered_map<std::string, int> mapKeyToIndex;
    int index=updateIndex(parsedJson, "k", 0, &mapKeyToIndex);
    REQUIRE(index==1);
    REQUIRE(mapKeyToIndex["k"]==0);
}
TEST_CASE("does not create an index where it doesn't exists", "parse_json"){
    auto parsedJson=parse_char((char*)"{\"k\":5.0}");
    std::unordered_map<std::string, int> mapKeyToIndex;
    int index=updateIndex(parsedJson, "r", 0, &mapKeyToIndex);
    REQUIRE(index==0);
    REQUIRE(mapKeyToIndex.find("r")==mapKeyToIndex.end());
}

TEST_CASE("has empty map if no matches", "parse_json"){
    auto parsedJson=parse_char((char*)"{\"k\":5.0}");
    std::vector<std::string> possibleParameters({"C"});
    auto mapKeyToIndex=constructKeyToIndex(parsedJson, possibleParameters);
    REQUIRE(mapKeyToIndex.size()==0);
}
TEST_CASE("has subset of map with some matches", "parse_json"){
    auto parsedJson=parse_char((char*)"{\"C\":5.0, \"Y\":3.0}");
    std::vector<std::string> possibleParameters({"C", "Y"});
    auto mapKeyToIndex=constructKeyToIndex(parsedJson, possibleParameters);
    REQUIRE(mapKeyToIndex.at("C")==0);
    REQUIRE(mapKeyToIndex.at("Y")==1);
}
TEST_CASE("get double when exists", "parse_json"){
    auto staticJson=parse_char((char*)"{\"C\":5.0, \"Y\":3.0}");
    auto variableJson=parse_char((char*)"{\"sigma\":5.0, \"rho\":3.0}");
    std::vector<std::string> possibleParameters({"C", "Y", "sigma", "rho"});
    auto mapKeyToIndex=constructKeyToIndex(variableJson, possibleParameters);
    for(auto& v:mapKeyToIndex){
        std::cout<<v.first<<", "<<v.second<<std::endl;
    }
    REQUIRE(
        getArgOrConstant(
            "C", 
            std::vector<double>({2.0, 6.0}), 
            staticJson,
            mapKeyToIndex
        )==5.0
    );
    REQUIRE(
        getArgOrConstant(
            "sigma", 
            std::vector<double>({2.0, 6.0}), 
            staticJson,
            mapKeyToIndex
        )==2.0
    );
}