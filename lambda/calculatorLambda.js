const {spawnBinary, genericSpawn} = require('./spawnUtils')
const {transformCallback}=require('./httpUtils')
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

const calculatorSpawn=spawnBinary('calculator')
const defaultParametersSpawn=callback=>genericSpawn('defaultParameters', [], callback)

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
module.exports.calculatorKeys=calculatorKeys
