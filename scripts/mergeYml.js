const yaml = require('js-yaml')
const {promisify} = require("es6-promisify")
const fs=require('fs-extra')
const readFile = promisify(fs.readFile)
const writeFile = promisify(fs.writeFile)
const readDirectory=promisify(fs.readdir)

const loadYml=data=>new Promise((resolve, reject)=>{
    yaml.safeLoadAll(data, asJson=>{
        resolve(asJson)
    })
})

const updateYMLFunctions=fileName=>doc=>Object.keys(doc.functions).reduce((aggr, funcName)=>({
    ...aggr, 
    [funcName+fileName]:{
        ...doc.functions[funcName],
        handler:doc.functions[funcName].handler.replace('handler', `handler${fileName}`),
        events:doc.functions[funcName].events.map(event=>({
            ...event, 
            http:{
                ...event.http,
                path:event.http.path.replace('version', fileName)
            }
        }))
    }
}), {})

const getYML=name=>readFile(`./releases/${name}/serverless.yml`, 'utf8') 

const aggregateYMLJson=arrOfYMLJson=>arrOfYMLJson.reduce((aggr, doc)=>({...aggr, ...doc}), {})
const convertJsonToServerlessYML=(rest, functions)=>ymlJson=>yaml.safeDump({
    ...rest,
    functions:{
        ...functions,
        ...ymlJson
    }
})

const writeToFile=ymlString=>writeFile('./releases/serverless.yml', ymlString)

const getPreviousFunctions=folderNames=>Promise.all(
        folderNames.map(name=>getYML(name).then(loadYml).then(updateYMLFunctions(name)))
    ).then(aggregateYMLJson)

readFile('./serverless.yml', 'utf8')
    .then(loadYml)
    .then(
        ({functions, ...rest})=>readDirectory('./releases')
        .then(getPreviousFunctions)
        .then(convertJsonToServerlessYML(rest, functions))
        .then(writeToFile)
    ).catch(err=>{
        console.log(err)
    })

