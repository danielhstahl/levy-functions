#!/bin/bash
rm -rf FunctionalUtilities
rm -rf CharacteristicFunctions
rm -rf FangOost
rm -rf cfdistutilities
rm -rf FFTOptionPricing
rm -rf RungeKutta
rm -rf TupleUtilities
rm -rf AutoDiff
rm -rf GaussNewton
rm -rf HullWhite
rm -rf nelder_mead
rm -rf BSImpliedVolatility
rm -rf cuckoo_search
rm -rf MonteCarlo
function cloneAndCheckout {
	git clone https://github.com/phillyfan1138/$1
	cd $1
	cd ..
}

rm -rf bin
mkdir bin

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

make calculator
make calibrator
make test
./test

cp calculator ./bin
cp calibrator ./bin
#serverless deploy -v
