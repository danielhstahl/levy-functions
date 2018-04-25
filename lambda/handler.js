'use strict'
const spawn = require('child_process').spawn
const middy = require('middy')
const { cors } = require('middy/middlewares')

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

const msg=body=>({
  statusCode:'200',
  body
})
const errMsg=body=>({
  statusCode:'400',
  body
})
const genericSpawn=(binary, options, callback)=>{
  const binaryPath=process.env['LAMBDA_TASK_ROOT']?`./${binary}`:`./bin/${binary}`
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
const spawnBinaryNoFunctionality=binary=>(parms, callback)=>{
  genericSpawn(binary, [getParametersOrObject(parms)], callback)
}
const calculatorSpawn=spawnBinary('calculator')
const calibratorSpawn=spawnBinaryNoFunctionality('calibrator')


module.exports.calculator=middy((event, context, callback)=>{
  const {optionType, sensitivity, algorithm}=event.pathParameters
  const key=algorithm?optionType+sensitivity+algorithm:optionType+sensitivity
  const index=calculatorKeys[key]
  calculatorSpawn(index, event.body, callback)
}).use(cors())

module.exports.calibrator=middy((event, context, callback)=>{
  const keyResult=calibratorRequiredKeys(JSON.parse(event.body))
  if(keyResult){
    const err=`Requires additional keys!  Missing ${keyResult}`
    return callback(null,  errMsg(err))
    //done(callback)(new Error(`Requires additional keys!  Missing ${keyResult}`))
  }
  calibratorSpawn(event.body, callback)
}).use(cors())
module.exports.calculatorKeys=calculatorKeys