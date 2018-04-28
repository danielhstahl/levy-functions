const request=require('request')
const {promisify} = require("es6-promisify");
const fs=require('fs-extra')

const mkdir=promisify(fs.mkdir)
const get=promisify(request.get)

const url='https://api.github.com/repos/phillyfan1138/levy-functions/releases'
//const url='https://api.github.com/repos/phillyfan1138/levy-functions/releases/latest'

const options=url=>({
    url,
    headers: {
        'User-Agent': 'levy-functions'
    }
})
const makeExecutable=(path, resolve, reject)=>()=>{
    fs.chmod(path, 0755, err=>{
        if(err){
            reject(err)
        }
        else{
            resolve()
        }
    })
}
const pipeResponse=(url, tag_name, name)=>new Promise((resolve, reject)=>{
    const path=`./releases/${tag_name}/${name}`
    request.get(options(url)).on('error', reject).pipe(fs.createWriteStream(path)).on('finish', makeExecutable(path, resolve, reject))
})

const getAssets=({assets, tag_name})=>mkdir(`./releases/${tag_name}`).then(()=>Promise.all(
    assets.map(({browser_download_url, name})=>pipeResponse(browser_download_url, tag_name, name))
))
let numV=0
fs.remove('./releases', ()=>{
    get(options(url)).then(response=>{
        const releases=JSON.parse(response.body)
        if(Array.isArray(releases)){
            numV=releases.length+1
            return mkdir('./releases').then(()=>Promise.all(
                releases.map(getAssets)
            ))
        }
        else{
            numV=1
            return Promise.resolve()
        }
    })
    .then(()=>Promise.all([
        fs.copy('./serverless.yml', `./releases/v${numV}/serverless.yml`),
        fs.copy('./lambda/handler.js', `./releases/v${numV}/handler.js`),
        fs.copy('./bin/calculator', `./releases/v${numV}/calculator`),
        fs.copy('./bin/defaultParameters', `./releases/v${numV}/defaultParameters`),
        fs.copy('./bin/calibrator', `./releases/v${numV}/calibrator`)
    ]))
    .catch(err=>{
        console.log(err)
    })
})
