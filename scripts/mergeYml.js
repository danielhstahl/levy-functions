const yaml = require('js-yaml')
const {promisify} = require("es6-promisify");
const readFile = promisify(require('fs-extra').readFile)

readFile('test.yml', 'utf8').then(result=>{
    yaml.safeLoad()
})

