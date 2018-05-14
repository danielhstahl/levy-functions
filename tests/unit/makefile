SRC=../../src
INCLUDES=-I $(SRC)/rapidjson/include/rapidjson -I $(SRC)/cfdistutilities -I $(SRC)/CharacteristicFunctions -I $(SRC)/FangOost -I $(SRC)/FFTOptionPricing -I $(SRC)/FunctionalUtilities -I $(SRC)/RungeKutta -I $(SRC)/GaussNewton -I $(SRC)/TupleUtilities -I $(SRC)/AutoDiff  -I $(SRC)/HullWhite -I $(SRC)/BSImpliedVolatility -I $(SRC)/nelder_mead -I $(SRC)/cuckoo_search -I $(SRC)/MonteCarlo -I $(SRC) 
GCCVAL=g++

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	GCCVAL=g++-7
endif

test: test.cpp $(SRC)/parse_json.h $(SRC)/get_cf.h
	$(GCCVAL) -std=c++14 -pthread test.cpp $(INCLUDES) -o test -fopenmp

clean:
	-rm *.o test 

