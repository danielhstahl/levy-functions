'use strict'
const spawn = require('child_process').spawn;

const keys={
  carrmadanput:0,
  carrmadancall:1,
  fangoostput:2,
  fangoostputdelta:3,
  fangoostcall:4,
  fangoostcalldelta:5,
  fstsput:6,
  fstsputdelta:7,
  fstscall:8,
  fstscalldelta:9,
  VaR:10,
  density:11
}

process.env['PATH']=`${process.env['PATH']}:${process.env['LAMBDA_TASK_ROOT']}`
const isParseble=val=>{
  try{
      val=parseFloat(val)
  }catch(err){
      return val
  }
  return val
}

const done = cb=>(err, res) => cb(null, {
  statusCode: err ? '400' : '200',
  body: err ? err.message : res,
  headers: {
      "Access-Control-Allow-Origin" : "*", // Required for CORS support to work
      "Access-Control-Allow-Credentials" : true, // Required for cookies, authorization headers with HTTPS 
      'Content-Type': 'application/json',
  },
});

const spawnBinary=(functionalityIndicator, parms, done)=>{
  const model=spawn('./bin/levyfunctions', [functionalityIndicator, parms?parms:"{}"])
  let modelOutput='';
  let modelErr='';
  model.stdout.on('data', data=>{
    modelOutput+=data;
  })
  model.stderr.on('data', data=>{
      modelErr+=data;
  })
  model.on('close', code=>{
      if(modelErr){
          return done(new Error(modelErr), "");
      }
      return done(null, modelOutput);
  })
}
Object.keys(keys).forEach(key=>{
  module.exports[key]=(event, context, callback) => {
    spawnBinary(keys[key], event.body, done(callback))
  }
})
/*
module.exports.fangoostcall = (event, context, callback) => {
  spawnBinary(fangoostcall, event.body, done(callback))
};
module.exports.fangoostput = (event, context, callback) => {
  spawnBinary(fangoostput, event.body, done(callback))
};
module.exports.fangoostcalldelta = (event, context, callback) => {
  spawnBinary(fangoostcalldelta, event.body, done(callback))
};
module.exports.fangoostputdelta = (event, context, callback) => {
  spawnBinary(fangoostputdelta, event.body, done(callback))
};
module.exports.carrmadanput = (event, context, callback) => {
  spawnBinary(carrmadanput, event.body, done(callback))
};
module.exports.carrmadancall = (event, context, callback) => {
  spawnBinary(carrmadancall, event.body, done(callback))
};
module.exports.fstsput = (event, context, callback) => {
  spawnBinary(fstsput, event.body, done(callback))
};
module.exports.fstscall = (event, context, callback) => {
  spawnBinary(fstscall, event.body, done(callback))
};
module.exports.fstsputdelta = (event, context, callback) => {
  spawnBinary(fstsputdelta, event.body, done(callback))
};
module.exports.fstscalldelta = (event, context, callback) => {
  spawnBinary(fstscalldelta, event.body, done(callback))
};
module.exports.VaR = (event, context, callback) => {
  spawnBinary(VaR, event.body, done(callback))
};
module.exports.density = (event, context, callback) => {
  spawnBinary(density, event.body, done(callback))
};
*/