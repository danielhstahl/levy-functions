const request=require('request')
const {promisify} = require("es6-promisify");
const fs=require('fs-extra')

const mkdir=promisify(fs.mkdir)
const get=promisify(request.get)

const url='https://api.github.com/repos/phillyfan1138/levy-functions/releases'

const options=url=>({
    url,
    headers: {
        'User-Agent': 'levy-functions'
    }
})

const pipeResponse=(url, tag_name, name)=>new Promise((resolve, reject)=>{
    request.get(options(url)).on('error', err=>reject(err)).pipe(fs.createWriteStream(`./releases/${tag_name}/${name}`)).on('finish', resolve)
})
/*
const extractZip=tag_name=>()=>{
    const path=`./${tag_name}.zip`
    fs.createReadStream(path).pipe(unzip.Extract({ path: './releases' })).on('finish', ()=>{
        fs.unlink(path, ()=>{})
    })
}*/

//const getRelease=({zipball_url, tag_name})=>pipeResponse(zipball_url, tag_name).then(extractZip(tag_name))
const getAssets=({assets, tag_name})=>mkdir(`./releases/${tag_name}`).then(()=>Promise.all(
    assets.map(({url, name})=>pipeResponse(url, tag_name, name))
))
fs.remove('./releases/*', ()=>{
    get(options(url)).then(response=>{
        const releases=JSON.parse(response.body)
        return Promise.all(
            releases.map(getAssets)
        )
    }).catch(err=>{
        console.log(err)
    })
})
