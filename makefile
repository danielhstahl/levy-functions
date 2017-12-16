INCLUDES=-I ./rapidjson/include/rapidjson -I ./cfdistutilities -I ./CharacteristicFunctions -I ./FangOost -I ./FFTOptionPricing -I ./FunctionalUtilities -I ./RungeKutta -I ./GaussNewton -I ./TupleUtilities -I ./AutoDiff
GCCVAL=g++

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
	GCCVAL=g++-7
endif

levyfunctions:main.o
	$(GCCVAL) -std=c++14 -O3  -pthread main.o $(INCLUDES) -o levyfunctions -fopenmp -static-libstdc++
main.o: main.cpp parse_json.h
	$(GCCVAL) -std=c++14 -O3  -pthread -c main.cpp  $(INCLUDES) -fopenmp 
clean:
	-rm *.o test levyfunctions

test: test.cpp parse_json.h
	g++ -std=c++14 -pthread test.cpp $(INCLUDES) -o test -fopenmp