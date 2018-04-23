| [Linux][lin-link]  | 
| :---------------:  | 
| ![lin-badge]       | 

[lin-badge]: https://travis-ci.org/phillyfan1138/levy-functions.svg?branch=master "Travis build status"
[lin-link]:  https://travis-ci.org/phillyfan1138/levy-functions "Travis build status"


# levy-functions: v1

These are a set of lambda functions for pricing options when assets follow an extended CGMY process with stochastic time clock correlated with the diffusion portion of the asset process.  See http://engineering.nyu.edu/files/jfetchgepaper.pdf and https://pdfs.semanticscholar.org/67cd/b553e2624c79a960ff79d0dfe6e6833690a7.pdf.

## endpoints

* Fang-Oosterlee algorithm for call POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/v1/dev/call/fangoost
* Fang-Oosterlee algorithm for put POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/v1/dev/put/fangoost
* Carr-Madan algorithm for put  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v1/put/carrmadan
* Carr-Madan algorithm for call  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v1/call/carrmadan
* FSTS algorithm for put  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v1/put/fsts
* FSTS algorithm for call  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v1/call/fsts
* VaR and ES for the CGMY process  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v1/density/var
* Density for the CGMY process  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v1/density/raw
* Calibration for call prices  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v1/call/calibration

# levy-functions: v2

These are a set of lambda functions for pricing options when assets follow an extended Merton Jump Diffusion process with stochastic time clock correlated with the diffusion portion of the asset process. 

* Fang-Oosterlee algorithm for call POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/v2/dev/call/fangoost
* Fang-Oosterlee algorithm for put POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/v2/dev/put/fangoost
* Carr-Madan algorithm for put  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v2/put/carrmadan
* Carr-Madan algorithm for call  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v2/call/carrmadan
* FSTS algorithm for put  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v2/put/fsts
* FSTS algorithm for call  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v2/call/fsts
* VaR and ES for the CGMY process  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v2/density/var
* Density for the CGMY process  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v2/density/raw
* Calibration for call prices  POST - https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v2/call/calibration


# example
The [Examples](./Examples) folder contains example scripts.  Run after `./compileAndPackage`.

The `example.json` file is a series of parameters for the Heston model using parameters from [Fang-Oosterlee](http://ta.twi.tudelft.nl/mf/users/oosterle/oosterlee/COS.pdf) page 16.  Reference price is 5.785155450.

To use this file, run `test.sh` from within the examples directory.



# lambda

The lambda functions are defined in the [lambda](./lambda) folder.

# c++

The C++ code is in the [src](./src) folder.