const yaml = require('js-yaml')
const {promisify} = require("es6-promisify");
const readFile = promisify(require('fs-extra').readFile)
const writeFile = promisify(require('fs-extra').writeFile)
const loadYml=data=>new Promise((resolve, reject)=>{
    yaml.safeLoadAll(data, asJson=>{
        resolve(asJson)
    })
})
readFile('../serverless.yml', 'utf8').then(loadYml).then(doc=>{
    //console.log(doc)
    const newFncs=Object.keys(doc.functions).reduce((aggr, funcName)=>{
        return {...aggr, [funcName+'v1']:doc.functions[funcName]}
    }, {})
    const yml=yaml.safeDump({...doc, functions:newFncs})

    return writeFile('./testYML.yml', yml)

}).catch(err=>{
    console.log(err)
})

