#!/bin/bash
rm -rf bin
mkdir bin


function cloneAndCheckout {
	git clone https://github.com/phillyfan1138/$1
	cd $1
	cd ..
}

cd src
rm -rf FunctionalUtilities
rm -rf CharacteristicFunctions
rm -rf FangOost
rm -rf cfdistutilities
rm -rf FFTOptionPricing
rm -rf rapidjson
rm -rf RungeKutta
rm -rf TupleUtilities
rm -rf AutoDiff
rm -rf GaussNewton
rm -rf HullWhite
rm -rf nelder_mead
rm -rf BSImpliedVolatility
rm -rf cuckoo_search
rm -rf MonteCarlo

cloneAndCheckout FunctionalUtilities 
cloneAndCheckout nelder_mead 
cloneAndCheckout cuckoo_search
cloneAndCheckout MonteCarlo
cloneAndCheckout CharacteristicFunctions 
cloneAndCheckout FangOost 
cloneAndCheckout cfdistutilities 
cloneAndCheckout FFTOptionPricing 
cloneAndCheckout RungeKutta 
cloneAndCheckout AutoDiff 
cloneAndCheckout TupleUtilities 
cloneAndCheckout BSImpliedVolatility 
cloneAndCheckout GaussNewton 
cloneAndCheckout HullWhite 

git clone https://github.com/miloyip/rapidjson

make calculator
make calibrator
make defaultParameters
cd ..
cd tests
cd unit
make test
./test
cd ..
cd ..
cd src

rm -rf FunctionalUtilities
rm -rf CharacteristicFunctions
rm -rf FangOost
rm -rf cfdistutilities
rm -rf FFTOptionPricing
rm -rf rapidjson
rm -rf RungeKutta
rm -rf TupleUtilities
rm -rf AutoDiff
rm -rf GaussNewton
rm -rf HullWhite
rm -rf nelder_mead
rm -rf BSImpliedVolatility
rm -rf cuckoo_search
rm -rf MonteCarlo


cd ..
cp ./src/calculator ./bin
cp ./src/calibrator ./bin
cp ./src/defaultParameters ./bin
