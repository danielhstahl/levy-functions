'use strict'
const spawn = require('child_process').spawn;

process.env['PATH']=`${process.env['PATH']}:${process.env['LAMBDA_TASK_ROOT']}`
const isParseble=val=>{
  try{
      val=parseFloat(val)
  }catch(err){
      return val
  }
  return val
}
const parseToFloat=obj=>{
  return Object.keys(obj).reduce((prev, curr)=>{
      prev[curr]=isParseble(obj[curr])
      return prev
  },{})
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
  const model=spawn('./bin/levyfunctions', [functionalityIndicator, parms?JSON.stringify(parms):"{}"])
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


module.exports.fangoostcall = (event, context, callback) => {
  console.log(event)
  spawnBinary(3, event, done(callback))
};
module.exports.fangoostput = (event, context, callback) => {
  spawnBinary(2, event, done(callback))
};
module.exports.carrmadanput = (event, context, callback) => {
  spawnBinary(0, event, done(callback))
};
module.exports.carrmadancall = (event, context, callback) => {
  spawnBinary(1, event, done(callback))
};
module.exports.fstsput = (event, context, callback) => {
  spawnBinary(4, event, done(callback))
};
module.exports.fstscall = (event, context, callback) => {
  spawnBinary(5, event, done(callback))
};
module.exports.VaR = (event, context, callback) => {
  spawnBinary(6, event, done(callback))
};
module.exports.density = (event, context, callback) => {
  spawnBinary(7, event, done(callback))
};
