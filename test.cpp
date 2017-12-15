#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "parse_json.h"

TEST_CASE("parses option with no input", "parse_json"){
    //option_var myTestOption;
    REQUIRE(get_option_var((char*)"{}").r==.03);
}
TEST_CASE("parses option with some input", "parse_json"){
    REQUIRE(get_option_var((char*)"{\"r\":.04}").r==.04);
}