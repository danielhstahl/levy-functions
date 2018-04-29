const yaml = require('js-yaml')
const {promisify} = require("es6-promisify")
const fs=require('fs-extra')
const readFile = promisify(fs.readFile)
const writeFile = promisify(fs.writeFile)
const readDirectory=promisify(fs.readdir)
const {exec}=require('child_process')
const loadYml=data=>new Promise((resolve, reject)=>{
    yaml.safeLoadAll(data, asJson=>{
        resolve(asJson)
    })
})

const updateYMLFunctions=fileName=>doc=>Object.keys(doc.functions).reduce((aggr, funcName)=>({
    ...aggr, 
    [funcName+fileName]:{
        ...doc.functions[funcName],
        handler:doc.functions[funcName].handler.replace('lambda', fileName),
        events:doc.functions[funcName].events.map(event=>({
            ...event, 
            http:{
                ...event.http,
                path:event.http.path.replace('version', fileName)
            }
        }))
    }
}), {})


const getPackageInfo=folderNames=>({
    service:'levy-functions',
    provider:{
        name:'aws',
        runtime:'nodejs8.10'
    },
    package:{
        include:folderNames
    }
})

const getYML=name=>readFile(`./releases/${name}/serverless.yml`, 'utf8') 
const parseFunctions=folderNames=>({
    fnc:Promise.all(
        folderNames.map(name=>getYML(name).then(loadYml).then(updateYMLFunctions(name)))
    ),
    package:getPackageInfo(folderNames)
})
const aggregateYMLJson=({fnc, package})=>fnc.then(ymlObj=>({...package, functions:ymlObj.reduce((aggr, doc)=>({...aggr, ...doc}), {})}))
const convertJsonToServerlessYML=ymlJson=>yaml.safeDump(ymlJson).replace(/'/g, '')

const writeToFile=ymlString=>writeFile('./releases/serverless.yml', ymlString)

readDirectory('./releases')
    .then(parseFunctions)
    .then(aggregateYMLJson)
    .then(convertJsonToServerlessYML)
    .then(writeToFile)
    .catch(err=>{
        console.log(err)
    })

