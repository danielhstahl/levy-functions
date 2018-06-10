const {spawnBinary} = require('./spawnUtils')
const {transformCallback, errMsg} = require('./httpUtils')
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
const calibratorSpawn=spawnBinary('calibrator')

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
module.exports.calibrator=calibrator