#!/bin/bash
function cloneAndCheckout {
	git clone https://github.com/phillyfan1138/$1
	cd $1
	cd ..
}
function editMake {

	sed -i "1s/^/STATIC=-static-libstdc++\n/" makefile 
}
function compile {
	cd $1
	editMake
	make 
	cp $1 ../bin
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

#git clone https://github.com/miloyip/rapidjson

#rm -rf FunctionalUtilities
#rm -rf CharacteristicFunctions
#rm -rf FangOost
#rm -rf cfdistutilities
#rm -rf FFTOptionPricing
#rm -rf rapidjson
#serverless deploy -v
