INCLUDES=-I ./rapidjson/include/rapidjson -I ./cfdistutilities -I ./CharacteristicFunctions -I ./FangOost -I ./FFTOptionPricing -I ./FunctionalUtilities -I ./RungeKutta -I ./GaussNewton -I ./TupleUtilities -I ./AutoDiff  -I ./HullWhite -I ./BSImpliedVolatility -I ./nelder_mead -I ./cuckoo_search -I ./MonteCarlo
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
	$(GCCVAL) -std=c++14 -O3  -pthread calibrator.o $(INCLUDES) -o calibrator -fopenmp -static-libstdc++
calibrator.o: calibrator.cpp parse_json.h get_cf.h
	$(GCCVAL) -std=c++14 -O3  -pthread -c calibrator.cpp  $(INCLUDES) -fopenmp 

clean:
	-rm *.o test calculator calibrator

test: test.cpp parse_json.h
	$(GCCVAL) -std=c++14 -pthread test.cpp $(INCLUDES) -o test -fopenmp