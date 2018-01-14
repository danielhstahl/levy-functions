#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "parse_json.h"

TEST_CASE("parses option with no input", "parse_json"){
    //option_var myTestOption;
    auto parsedJson=parse_char((char*)"{}");
    REQUIRE(get_option_var(parsedJson).r==.03);
}
TEST_CASE("parses option with some input", "parse_json"){
    auto parsedJson=parse_char((char*)"{\"r\":0.04}");
    REQUIRE(get_option_var(parsedJson).r==.04);
}
TEST_CASE("parses k with some input", "parse_json"){
    auto parsedJson=parse_char((char*)"{\"k\":[0.04, 0.05]}");
    REQUIRE(get_k_var(parsedJson)[0]==.04);
}