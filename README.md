## levy-functions

These are a set of lambda functions for pricing options when assets follow an extended CGMY process with stochastic time clock correlated with the diffusion portion of the asset process.  See http://engineering.nyu.edu/files/jfetchgepaper.pdf and https://pdfs.semanticscholar.org/67cd/b553e2624c79a960ff79d0dfe6e6833690a7.pdf.

## endpoints

* Fang-Oosterlee algorithm for call POST - https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/call/fangoost
* Fang-Oosterlee algorithm for put POST - https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/put/fangoost
* Carr-Madan algorithm for put  POST - https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/put/carrmadan
* Carr-Madan algorithm for call  POST - https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/call/carrmadan
* FSTS algorithm for put  POST - https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/put/fsts
* FSTS algorithm for call  POST - https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/call/fsts
* VaR and ES for the CGMY process  POST - https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/density/var
* Density for the CGMY process  POST - https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/density/raw