#!/bin/bash
function cloneAndCheckout {
	git clone https://github.com/phillyfan1138/$1
	cd $1
	cd ..
}

rm -rf bin
mkdir bin

cloneAndCheckout FunctionalUtilities 
cloneAndCheckout CharacteristicFunctions 
cloneAndCheckout FangOost 
cloneAndCheckout cfdistutilities 
cloneAndCheckout FFTOptionPricing 
cloneAndCheckout RungeKutta 
cloneAndCheckout AutoDiff 
cloneAndCheckout TupleUtilities 
cloneAndCheckout GaussNewton 

git clone https://github.com/miloyip/rapidjson
make 
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
cp levyfunctions ./bin
#serverless deploy -v
