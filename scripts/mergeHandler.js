const request=require('request')
const {promisify} = require("es6-promisify");
const fs=require('fs-extra')

const unzip=require('unzip')
const url='https://api.github.com/repos/phillyfan1138/levy-functions/releases'

const options=url=>({
    url,
    headers: {
        'User-Agent': 'levy-functions'
    }
})

const get=promisify(request.get)
const pipeResponse=(url, tag_name)=>new Promise((resolve, reject)=>{
    request.get(options(url)).on('error', err=>reject(err)).pipe(fs.createWriteStream(`./${tag_name}.zip`)).on('finish', resolve)
})

const extractZip=tag_name=>()=>{
    const path=`./${tag_name}.zip`
    fs.createReadStream(path).pipe(unzip.Extract({ path: './releases' })).on('finish', ()=>{
        fs.unlink(path, ()=>{})
    })
}

const getRelease=({zipball_url, tag_name})=>pipeResponse(zipball_url, tag_name).then(extractZip(tag_name))

fs.remove('./releases', ()=>{
    get(options(url)).then(response=>{
        const releases=JSON.parse(response.body)
        return Promise.all(
            releases.map(getRelease)
        )
    }).catch(err=>{
        console.log(err)
    })
})
