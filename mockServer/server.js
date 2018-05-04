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
        const {calibration}=req.params
        return calibration==='spline'?{"curve":[{"logStrike":-2.64268, "transformedOption":0.000870664},{"logStrike":-2.55888, "transformedOption":0.000941777},{"logStrike":-2.47508, "transformedOption":0.00101833},{"logStrike":-2.39128, "transformedOption":0.00110072},{"logStrike":-2.30748, "transformedOption":0.00118938},{"logStrike":-2.22368, "transformedOption":0.00128479},{"logStrike":-2.13989, "transformedOption":0.00138749},{"logStrike":-2.05609, "transformedOption":0.0014981},{"logStrike":-1.97229, "transformedOption":0.00161734},{"logStrike":-1.88849, "transformedOption":0.00174605},{"logStrike":-1.80469, "transformedOption":0.00188525},{"logStrike":-1.72089, "transformedOption":0.00203618},{"logStrike":-1.63709, "transformedOption":0.0022004},{"logStrike":-1.55329, "transformedOption":0.00237985},{"logStrike":-1.46949, "transformedOption":0.00257702},{"logStrike":-1.38569,"transformedOption":0.0027951},{"logStrike":-1.30189, "transformedOption":0.00303824},{"logStrike":-1.21809, "transformedOption":0.00331183},{"logStrike":-1.13429, "transformedOption":0.00362291},{"logStrike":-1.05049, "transformedOption":0.00398075},{"logStrike":-0.966693, "transformedOption":0.00439749},{"logStrike":-0.882894, "transformedOption":0.00488907},{"logStrike":-0.799094, "transformedOption":0.00547648},{"logStrike":-0.715295, "transformedOption":0.00618728},{"logStrike":-0.631496, "transformedOption":0.00705766},{"logStrike":-0.547696, "transformedOption":0.00816899},{"logStrike":-0.463897, "transformedOption":0.00976392},{"logStrike":-0.380097, "transformedOption":0.0122311},{"logStrike":-0.296298, "transformedOption":0.0165672},{"logStrike":-0.212499, "transformedOption":0.0292143},{"logStrike":-0.128699, "transformedOption":0.0477138},{"logStrike":-0.0448997, "transformedOption":0.077118},{"logStrike":0.0388997, "transformedOption":0.0801849},{"logStrike":0.122699, "transformedOption":0.0487089},{"logStrike":0.206499, "transformedOption":0.0278058},{"logStrike":0.290298, "transformedOption":0.0146153},{"logStrike":0.374097, "transformedOption":0.00904864},{"logStrike":0.457897, "transformedOption":0.00647944},{"logStrike":0.541696, "transformedOption":0.00454096},{"logStrike":0.625496, "transformedOption":0.00311272},{"logStrike":0.709295, "transformedOption":0.0020859},{"logStrike":0.793094, "transformedOption":0.00136604},{"logStrike":0.876894, "transformedOption":0.000874138},{"logStrike":0.960693, "transformedOption":0.000546619},{"logStrike":1.04449, "transformedOption":0.000334142},{"logStrike":1.12829, "transformedOption":0.000199807},{"logStrike":1.21209, "transformedOption":0.000116993},{"logStrike":1.29589, "transformedOption":6.71716e-05},{"logStrike":1.37969, "transformedOption":3.78833e-05},{"logStrike":1.46349, "transformedOption":2.10316e-05},{"logStrike":1.54729, "transformedOption":1.15219e-05},{"logStrike":1.63109, "transformedOption":6.24534e-06},{"logStrike":1.71489, "transformedOption":3.35847e-06},{"logStrike":1.79869, "transformedOption":1.79614e-06},{"logStrike":1.88249, "transformedOption":9.56996e-07},{"logStrike":1.96629, "transformedOption":5.08186e-07},{"logStrike":2.05009, "transformedOption":2.68448e-07},{"logStrike":2.13389, "transformedOption":1.40294e-07},{"logStrike":2.21768, "transformedOption":7.17472e-08},{"logStrike":2.30148, "transformedOption":3.52322e-08},{"logStrike":2.38528, "transformedOption":1.61156e-08},{"logStrike":2.46908, "transformedOption":6.554e-09},{"logStrike":2.55288, "transformedOption":2.21171e-09},{"logStrike":2.63668, "transformedOption":5.6035e-10}], "points":[{"logStrike":-0.633488, "transformedOption":0.00703478},{"logStrike":-0.31983, "transformedOption":0.0148525},{"logStrike":-0.176729, "transformedOption":0.0362584},{"logStrike":-0.112191, "transformedOption":0.0524528},{"logStrike":-0.081419, "transformedOption":0.0630155},{"logStrike":-0.051566, "transformedOption":0.0745308},{"logStrike":-0.0225785, "transformedOption":0.0866625},{"logStrike":0.0329913, "transformedOption":0.0827636},{"logStrike":0.0596596, "transformedOption":0.0714446},{"logStrike":0.0856351, "transformedOption":0.0616385},{"logStrike":0.110953, "transformedOption":0.0523927},{"logStrike":0.159743, "transformedOption":0.0386641},{"logStrike":0.293274, "transformedOption":0.0142889},{"logStrike":0.334096, "transformedOption":0.0105346}]}:{"rho":-0.19246,"sigma":0.244064,"speed":2.7586,"v0":1.01803,"adaV":2.58261,"mse":8.69005e-05}
    },
    density:req=>{
        return [{"value":-398.227,"atPoint":7.59411e+06},{"value":109.545,"atPoint":95},{"value":107.437,"atPoint":100},{"value":96.7123,"atPoint":130},{"value":91.1387,"atPoint":150},{"value":88.7082,"atPoint":160},{"value":87.5658,"atPoint":165},{"value":86.4664,"atPoint":170},{"value":85.4065,"atPoint":175},{"value":83.392,"atPoint":185},{"value":82.4319,"atPoint":190},{"value":81.4998,"atPoint":195},{"value":80.5938,"atPoint":200},{"value":78.8528,"atPoint":210},{"value":74.1016,"atPoint":240},{"value":72.6501,"atPoint":250},{"value":178.456,"atPoint":0.00419377}] 
    },
    defaultParameters:req=>{
        return {"T":{"upper":1e+06, "lower":0},"S0":{"upper":1e+06, "lower":0},"rho":{"upper":1, "lower":-1},"r":{"upper":0.4, "lower":0},"speed":{"upper":3, "lower":0},"v0":{"upper":1.8, "lower":0.2},"sigJ":{"upper":2, "lower":0},"adaV":{"upper":3, "lower":0},"sigma":{"upper":1, "lower":0},"muJ":{"upper":1, "lower":-1},"numU":{"upper":10, "lower":5},"lambda":{"upper":2, "lower":0}}
    },

}

const getYMLFunctions=doc=>Object.keys(doc.functions).forEach(key=>{
    const {handler, events}=doc.functions[key]
    const {method, path}=events[0].http
    const cleanedPath=path.replace('version', '').replace(/{/g, ':').replace(/}/g, '')
    //console.log(cleanedPath)
    app[method](cleanedPath, (req, res)=>{
        console.log(cleanedPath)
        console.log(req.params)
        res.send(handlers[handler.split('.')[1]](req))
    })
})
const port=3001
getYML()
    .then(loadYml)
    .then(getYMLFunctions).then(()=>{
        app.listen(port, () => console.log(`Example app listening on port ${port}!`))
    })


