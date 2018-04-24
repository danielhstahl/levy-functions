'use strict'
const spawn = require('child_process').spawn

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

const totalKeys=[
  "C", 
  "G",
  "M",
  "Y",
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

const done = cb=>(err, res) => cb(null, {
  statusCode: err ? '400' : '200',
  body: err ? err.message : res,
  headers: {
    "Access-Control-Allow-Origin" : "*", // Required for CORS support to work
    "Access-Control-Allow-Credentials" : true, // Required for cookies, authorization headers with HTTPS 
    'Content-Type': 'application/json',
  }
})

const genericSpawn=(binary, options, done)=>{
  const model=spawn(`./bin/${binary}`,options)
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
      return done(new Error(modelErr), "")
    }
    return done(null, modelOutput)
  })
}
const getParametersOrObject=parameters=>parameters||"{}"
const spawnBinary=binary=>(functionalityIndicator, parms, done)=>{
  genericSpawn(binary, [functionalityIndicator,getParametersOrObject(parms)], done)
}
const spawnBinaryNoFunctionality=binary=>(parms, done)=>{
  genericSpawn(binary, [getParametersOrObject(parms)], done)
}
const calculatorSpawn=spawnBinary('calculator')
const calibratorSpawn=spawnBinaryNoFunctionality('calibrator')


module.exports.calculator=(event, context, callback)=>{
  const {optionType, sensitivity, algorithm}=event.pathParameters
  const key=algorithm?optionType+sensitivity+algorithm:optionType+sensitivity
 
  const index=calculatorKeys[key]
  calculatorSpawn(index, event.queryStringParameters, done(callback))
}

module.exports.calibrator=(event, context, callback)=>{
  const keyResult=calibratorRequiredKeys(JSON.parse(event.body))
  if(keyResult){
    return done(callback)(new Error(`Requires additional keys!  Missing ${keyResult}`))
  }
  calibratorSpawn(event.body, done(callback))
}

module.exports.fullmodel=(event, context, callback) => {
  const keyResult=calibratorRequiredKeys(JSON.parse(event.body))
  if(keyResult){
    return done(callback)(new Error(`Requires additional keys!  Missing ${keyResult}`))
  }
  calibratorSpawn(event.body, done(callback))
}
module.exports.calculatorKeys=calculatorKeys