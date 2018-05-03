const yaml = require('js-yaml')
const app=require('express')()
const {readFile}=require('fs-extra')
const bodyParser=require('body-parser')
app.use(bodyParser.json())

const loadYml=data=>new Promise((resolve, reject)=>{
    yaml.safeLoadAll(data, asJson=>{
        resolve(asJson)
    })
})
const getYML=()=>readFile(`${__dirname}/../serverless.yml`, 'utf8') 

const handlers={
    calculator:req=>{
        const {optionType, sensitivity, algorithm}=req.params
        return [{"value":-398.227,"atPoint":7.59411e+06},{"value":109.545,"atPoint":95},{"value":107.437,"atPoint":100},{"value":96.7123,"atPoint":130},{"value":91.1387,"atPoint":150},{"value":88.7082,"atPoint":160},{"value":87.5658,"atPoint":165},{"value":86.4664,"atPoint":170},{"value":85.4065,"atPoint":175},{"value":83.392,"atPoint":185},{"value":82.4319,"atPoint":190},{"value":81.4998,"atPoint":195},{"value":80.5938,"atPoint":200},{"value":78.8528,"atPoint":210},{"value":74.1016,"atPoint":240},{"value":72.6501,"atPoint":250},{"value":178.456,"atPoint":0.00419377}]
    },
    calibrator:req=>{
        return {"rho":-0.19246,"sigma":0.244064,"speed":2.7586,"v0":1.01803,"adaV":2.58261,"mse":8.69005e-05}
    },
    density:req=>{
        return [{"value":-398.227,"atPoint":7.59411e+06},{"value":109.545,"atPoint":95},{"value":107.437,"atPoint":100},{"value":96.7123,"atPoint":130},{"value":91.1387,"atPoint":150},{"value":88.7082,"atPoint":160},{"value":87.5658,"atPoint":165},{"value":86.4664,"atPoint":170},{"value":85.4065,"atPoint":175},{"value":83.392,"atPoint":185},{"value":82.4319,"atPoint":190},{"value":81.4998,"atPoint":195},{"value":80.5938,"atPoint":200},{"value":78.8528,"atPoint":210},{"value":74.1016,"atPoint":240},{"value":72.6501,"atPoint":250},{"value":178.456,"atPoint":0.00419377}] 
    },
    defaultParameters:req=>{
        return {"T":{"upper":1e+06, "lower":0},"S0":{"upper":1e+06, "lower":0},"rho":{"upper":1, "lower":-1},"r":{"upper":0.4, "lower":0},"speed":{"upper":3, "lower":0},"v0":{"upper":1.8, "lower":0.2},"sigJ":{"upper":2, "lower":0},"adaV":{"upper":3, "lower":0},"sigma":{"upper":1, "lower":0},"muJ":{"upper":1, "lower":-1},"numU":{"upper":10, "lower":5},"lambda":{"upper":2, "lower":0}}
    }
}

const getYMLFunctions=doc=>Object.keys(doc.functions).forEach(key=>{
    const {handler, events}=doc.functions[key]
    const {method, path}=events[0].http
    const cleanedPath=path.replace('version', '').replace(/{/g, ':').replace(/}/g, '')
    app[method](cleanedPath, (req, res)=>{
        res.send(handlers[handler.split('.')[1]](req))
    })
})

getYML()
    .then(loadYml)
    .then(getYMLFunctions).then(()=>{
        app.listen(3001, () => console.log('Example app listening on port 3001!'))
    })


