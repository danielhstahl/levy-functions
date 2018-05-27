SRC=./
INCLUDES=-I $(SRC)rapidjson/include/rapidjson -I $(SRC)cfdistutilities -I $(SRC)CharacteristicFunctions -I $(SRC)FangOost -I $(SRC)FFTOptionPricing -I $(SRC)FunctionalUtilities -I $(SRC)RungeKutta -I $(SRC)GaussNewton -I $(SRC)TupleUtilities -I $(SRC)AutoDiff  -I $(SRC)HullWhite -I $(SRC)BSImpliedVolatility -I $(SRC)nelder_mead -I $(SRC)cuckoo_search -I $(SRC)MonteCarlo 

GCCVAL=g++

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	GCCVAL=g++-7
endif

calculator:calculator.o
	$(GCCVAL) -std=c++14 -O3  -pthread calculator.o $(INCLUDES) -o calculator -fopenmp -static-libstdc++
calculator.o: calculator.cpp parse_json.h get_cf.h
	$(GCCVAL) -std=c++14 -O3  -pthread -c calculator.cpp  $(INCLUDES) -fopenmp 

calibrator:calibrator.o
	$(GCCVAL) -std=c++17 -O3  -pthread calibrator.o $(INCLUDES) -o calibrator -fopenmp -static-libstdc++
calibrator.o: calibrator.cpp parse_json.h get_cf.h
	$(GCCVAL) -std=c++17 -O3  -pthread -c calibrator.cpp  $(INCLUDES) -fopenmp

defaultParameters:defaultParameters.o
	$(GCCVAL) -std=c++14 -O3  -pthread defaultParameters.o $(INCLUDES) -o defaultParameters -fopenmp -static-libstdc++
defaultParameters.o: defaultParameters.cpp parse_json.h get_cf.h
	$(GCCVAL) -std=c++14 -O3  -pthread -c defaultParameters.cpp  $(INCLUDES) -fopenmp 

clean:
	-rm *.o test calculator calibrator defaultParameters
