'use strict'
const spawn = require('child_process').spawn
const keys=[
  'carrmadanput',
  'carrmadancall',
  'fangoostput',
  'fangoostputdelta',
  'fangoostputtheta',
  'fangoostputgamma',
  'fangoostcall',
  'fangoostcalldelta',
  'fangoostcalltheta',
  'fangoostcallgamma',
  'fstsput',
  'fstsputdelta',
  'fstsputtheta',
  'fstsputgamma',
  'fstscall',
  'fstscalldelta',
  'fstscalltheta',
  'fstscallgamma',
  'VaR',
  'density'
]


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

const spawnBinary=(functionalityIndicator, parms, done)=>{
  const model=spawn('./bin/levyfunctions', [functionalityIndicator, parms?parms:"{}"])
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
keys.forEach((key, index)=>{
  module.exports[key]=(event, context, callback) => {
    spawnBinary(index, event.body, done(callback))
  }
})
module.exports.keys=keys