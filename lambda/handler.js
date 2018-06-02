'use strict'
const {spawn} = require('child_process')
const https = require('https')
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
      return callback(modelErr, null)
    }
    return callback(null, modelOutput)
  })
}
const getParametersOrObject=parameters=>parameters||"{}"
const spawnBinary=binary=>(functionalityIndicator, parms, callback)=>{
  genericSpawn(binary, [functionalityIndicator,getParametersOrObject(parms)], callback)
}

const calculatorSpawn=spawnBinary('calculator')
const calibratorSpawn=spawnBinary('calibrator')
const defaultParametersSpawn=callback=>genericSpawn('defaultParameters', [], callback)

const transformCallback=callback=>(err, res)=>{
  if(err){
    return callback(null, errMsg(err))
  }
  return callback(null, msg(res))
}
module.exports.calculator=(event, context, callback)=>{
  const {optionType, sensitivity, algorithm}=event.pathParameters
  const key=optionType+sensitivity+algorithm
  const index=calculatorKeys[key]
  calculatorSpawn(index, event.body, transformCallback(callback))
}
module.exports.density=(event, context, callback)=>{
  const {densityType}=event.pathParameters
  const key='density'+densityType
  calculatorSpawn(calculatorKeys[key], event.body, transformCallback(callback))
}
module.exports.defaultParameters=(event, context, callback)=>{
  defaultParametersSpawn(transformCallback(callback))
}

const calibrator=(event, context, callback)=>{
  const {calibration}=event.pathParameters
  if(calibratorKeys[calibration]!==calibratorKeys.spline){
    const keyResult=calibratorRequiredKeys(JSON.parse(event.body))
    if(keyResult){
      const err=`Requires additional keys!  Missing ${keyResult}`
      return callback(null, errMsg(err))
    }
  }
  calibratorSpawn(calibratorKeys[calibration], event.body, transformCallback(callback))
}
module.exports.calculatorKeys=calculatorKeys
module.exports.calibrator=calibrator



const minOpenInterest=5
const liquidOptionPrices=({openInterest})=>openInterest>=minOpenInterest
const getPriceFromBidAsk=({bid, ask})=>(bid+ask)*.5
const getRelevantData=yahooData=>yahooData.optionChain.result[0]
//const getRelevantDataForArray=arr=>arr.map(getRelevantData)
const getExpirationDates=relevantData=>({
  S0:getPriceFromBidAsk(relevantData.quote), 
  expirationDates:relevantData.expirationDates.map(v=>v*ratioForUnixAndJSTimeStamp)
})

const filterSingleMaturityData=relevantData=>{
  const S0=getPriceFromBidAsk(relevantData.quote)
  const T=yearsBetweenNowAndTimestamp(relevantData.options[0].expirationDate)
  const options=relevantData.options[0].calls.filter(liquidOptionPrices).map(
    ({strike, bid, ask})=>({
        strike,
        price:getPriceFromBidAsk({bid, ask})
      })
    )
    .reduce((aggr, {strike, price})=>({
      k:[...aggr.k, strike],
      prices:[...aggr.prices, price]
    }), {k:[], prices:[]})
  return Object.assign({S0, T, r:.004}, options)//.004 is placeholder
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
/*
const getOptionsByDate=ticker=>{
  const queryHOC=getQuery(ticker)
  return ({expirationDates})=>Promise.all(expirationDates.map(date=>httpGet(queryHOC(date))))
}*/

module.exports.getExpirationDates=(event, context, callback)=>{
  const {ticker}=event.pathParameters
  httpGet(getQuery(ticker)())
    .then(getRelevantData)
    .then(getExpirationDates)
    .then(data=>callback(null, msg(JSON.stringify(data))))
    .catch(err=>callback(null, errMsg(err.message)))
}

module.exports.getOptionPrices=(event, context, callback)=>{
  const {ticker, asOfDate}=event.pathParameters
  httpGet(getQuery(ticker)(asOfDate/ratioForUnixAndJSTimeStamp))
    .then(getRelevantData)
    .then(filterSingleMaturityData)
    .then(data=>{
      calibratorSpawn(calibratorKeys.spline, JSON.stringify(data), (err, spline)=>{
        if(err){
          return callback(null, errMsg(err))
        }
        return callback(null, msg(JSON.stringify(Object.assign({}, data, JSON.parse(spline)))))
      })
    })
    .catch(err=>callback(null, errMsg(err.message)))
}

