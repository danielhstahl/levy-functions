'use strict'
const {spawn} = require('child_process')
const https = require('https')
const gaussian=require('gaussian')
const nr = require('newton-raphson-method')
const ratioForUnixAndJSTimeStamp=1000
const numMSInYears=24*60*60*365*ratioForUnixAndJSTimeStamp
const yearsBetweenNowAndTimestamp=timeStamp=>(timeStamp*ratioForUnixAndJSTimeStamp-Date.now())/numMSInYears
const calculatorKeys={
  putpricecarrmadan:0,
  callpricecarrmadan:1,
  putpricefangoost:2,
  putdeltafangoost:3,
  putthetafangoost:4,
  putgammafangoost:5,
  callpricefangoost:6,
  calldeltafangoost:7,
  callthetafangoost:8,
  callgammafangoost:9,
  putpricefsts:10,
  putdeltafsts:11,
  putthetafsts:12,
  putgammafsts:13,
  callpricefsts:14,
  calldeltafsts:15,
  callthetafsts:16,
  callgammafsts:17,
  densityvar:18,
  densityraw:19
}
const calibratorKeys={
  spline:0,
  calibrate:1
}

const totalKeys=[
  "lambda", 
  "muJ",
  "sigJ",
  "v0",
  "sigma",
  "rho",
  "adaV",
  "speed"
]

const calibratorRequiredKeys=body=>{
  const totalKey=Object.assign({}, body, body.variable)
  return totalKeys.find(key=>totalKey[key]===undefined)
}

process.env['PATH']=`${process.env['PATH']}:${process.env['LAMBDA_TASK_ROOT']}`

const gMsg=statusCode=>body=>({
  statusCode,
  headers: {
    "Access-Control-Allow-Origin" : "*", // Required for CORS support to work
    "Access-Control-Allow-Credentials" : true, // Required for cookies, authorization headers with HTTPS 
    'Content-Type': 'application/json',
  },
  body
})
const errMsg=gMsg('400')
const msg=gMsg('200')


const genericSpawn=(binary, options, callback)=>{
  const binaryPath=process.env['LAMBDA_TASK_ROOT']?`${__dirname}/${binary}`:`./bin/${binary}`
  const model=spawn(binaryPath,options)
  let modelOutput=''
  let modelErr=''
  model.stdout.on('data', data=>{
    modelOutput+=data
  })
  model.stderr.on('data', data=>{
    modelErr+=data
  })
  model.on('close', code=>{
    if(modelErr){
      return callback(null, errMsg(modelErr))
    }
    return callback(null, msg(modelOutput))
  })
}
const getParametersOrObject=parameters=>parameters||"{}"
const spawnBinary=binary=>(functionalityIndicator, parms, callback)=>{
  genericSpawn(binary, [functionalityIndicator,getParametersOrObject(parms)], callback)
}

const calculatorSpawn=spawnBinary('calculator')
const calibratorSpawn=spawnBinary('calibrator')
const defaultParametersSpawn=callback=>genericSpawn('defaultParameters', [], callback)


module.exports.calculator=(event, context, callback)=>{
  const {optionType, sensitivity, algorithm}=event.pathParameters
  const key=optionType+sensitivity+algorithm
  const index=calculatorKeys[key]
  calculatorSpawn(index, event.body, callback)
}
module.exports.density=(event, context, callback)=>{
  const {densityType}=event.pathParameters
  const key='density'+densityType
  calculatorSpawn(calculatorKeys[key], event.body, callback)
}
module.exports.defaultParameters=(event, context, callback)=>{
  defaultParametersSpawn(callback)
}

module.exports.calibrator=(event, context, callback)=>{
  const {calibration}=event.pathParameters
  const keyResult=calibratorRequiredKeys(JSON.parse(event.body))
  if(keyResult){
    const err=`Requires additional keys!  Missing ${keyResult}`
    return callback(null,  errMsg(err))
  }
  calibratorSpawn(calibratorKeys[calibration], event.body, callback)
}
module.exports.calculatorKeys=calculatorKeys



const minOpenInterest=5
const liquidOptionPrices=({openInterest})=>openInterest>=minOpenInterest
const getPriceFromBidAsk=({bid, ask})=>(bid+ask)*.5
const getRelevantData=yahooData=>yahooData.optionChain.result[0]
const getRelevantDataForArray=arr=>arr.map(getRelevantData)
const distribution=gaussian(0, 1)
const BS=({S0, strike, T, impliedVolatility, price})=>{
  const sqrtT=impliedVolatility*Math.sqrt(T)
  const logSK=Math.log(S0/strike)
  console.log(impliedVolatility)
  console.log(logSK)
  return {
    getCall:r=>{
      const discount=Math.exp(-r*T)
      const d1=(logSK+(r+impliedVolatility*impliedVolatility*T))/sqrtT
      const d2=d1-sqrtT
      //console.log(S0*distribution.cdf(d1)-strike*discount*distribution.cdf(d2))
      //console.log(price)
      return S0*distribution.cdf(d1)-strike*discount*distribution.cdf(d2)-price
    },
    getRho:r=>{
      const discount=Math.exp(-r*T)
      const d2=(logSK+(r-impliedVolatility*impliedVolatility*T))/sqrtT
      return strike*T*discount*distribution.cdf(d2)
    }
  }
}
const getRFromBS=({S0, strike, T, impliedVolatility, price})=>{
  const currBS=BS({S0, strike, T, impliedVolatility, price})
  const initR=.01
  return nr(currBS.getCall, currBS.getRho, initR, {verbose:true, maxIter:500})
}

const filterOptionData=relevantDataArray=>{
  const S0=getPriceFromBidAsk(relevantDataArray[0].quote)
  const options=relevantDataArray.reduce((aggr, {options})=>[
    ...aggr,
    ...options.reduce(
        (aggr, {calls})=>[
            ...aggr, 
            ...calls.map(({strike, openInterest, bid, ask, expiration, lastTradeDate, impliedVolatility})=>({
              strike,
              S0,
              price:getPriceFromBidAsk({bid, ask}),
              openInterest,
              impliedVolatility,
              timeSinceLastTrade:yearsBetweenNowAndTimestamp(lastTradeDate),
              T:yearsBetweenNowAndTimestamp(expiration),
              cohort:expiration
            }))
          ]
      , [])
  ], [])
  .filter(liquidOptionPrices)
  //console.log(options)
  const mostRecentOptionByCohort=options.reduce((aggr, curr)=>{
    if(curr.impliedVolatility>.0001){
      if(!aggr[curr.cohort]||(
        aggr[curr.cohort].timeSinceLastTrade<curr.timeSinceLastTrade&&
        curr.impliedVolatility>.0001
      )){
        return Object.assign({[curr.cohort]:curr}, aggr)
      }
    }
    return aggr
  }, {})
 // console.log(mostRecentOptionByCohort)
  const rByCohort=Object.entries(mostRecentOptionByCohort).reduce((aggr, [key, option])=>({[key]:getRFromBS(option)}), {})
  const applyR=option=>Object.assign({r:rByCohort[option.cohort]}, option)
  return {S0, options:options.map(applyR)}
}
const getDateQuery=date=>date?`?date=${date}`:''
const getQuery=ticker=>asOfDate=>`https://query1.finance.yahoo.com/v7/finance/options/${ticker}${getDateQuery(asOfDate)}`

const httpGet=query=>new Promise((res, rej)=>{
  https.get(query, resp => {
    let data = '';
    resp.on('data', chunk => {
      data += chunk;
    })
    resp.on('end', () => {
      res(JSON.parse(data))
    })
  }).on('error', err => {
    rej(err)
  })
})
const getOptionsByDate=ticker=>{
  const queryHOC=getQuery(ticker)
  return ({expirationDates})=>Promise.all(expirationDates.map(date=>httpGet(queryHOC(date))))
}
//returns object with S0, array of objects with strike, time to maturity, and price
//we WILL HAVE to update the cpp code to incorporate the effect.  But this will
//resolve issue 7
module.exports.getOptionPrices=(event, context, callback)=>{
  const {ticker}=event.pathParameters
  //todo!  fix this to have a default of no as of date (ie, current)
  httpGet(getQuery(ticker)())
    .then(getRelevantData)
    .then(getOptionsByDate(ticker))
    .then(getRelevantDataForArray)
    .then(filterOptionData)
    .then(data=>callback(null, msg(JSON.stringify(data))))
    .catch(err=>callback(null, errMsg(err.message)))
}

module.exports.BS=BS