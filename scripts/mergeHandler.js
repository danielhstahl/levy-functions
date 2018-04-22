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

const pipeResponse=(url, tag_name, name)=>new Promise((resolve, reject)=>{
    request.get(options(url)).on('error', err=>reject(err)).pipe(fs.createWriteStream(`./releases/${tag_name}/${name}`)).on('finish', resolve)
})

const getAssets=({assets, tag_name})=>mkdir(`./releases/${tag_name}`).then(()=>Promise.all(
    assets.map(({browser_download_url, name})=>pipeResponse(browser_download_url, tag_name, name))
))
let numV=0
fs.remove('./releases', ()=>{
    get(options(url)).then(response=>{
        const releases=JSON.parse(response.body)
        numV=releases.length+1
        return mkdir('./releases').then(()=>Promise.all(
            releases.map(getAssets)
        ))
    })
    .then(()=>Promise.all([
        fs.copy('./serverless.yml', `./releases/v${numV}/serverless.yml`),
        fs.copy('./lambda/handler.js', `./releases/v${numV}/handler.js`),
        fs.copy('./bin/calculator', `./releases/v${numV}/calculator`),
        fs.copy('./bin/calibrator', `./releases/v${numV}/calibrator`)
    ]))
    .catch(err=>{
        console.log(err)
    })
})
