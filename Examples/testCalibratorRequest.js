const results=require('./exampleCalibrator.json')
const request=require('request')
request({
    method:'POST',
    url:'https://74ekexhct2.execute-api.us-east-1.amazonaws.com/dev/v2/calibrator',
    body:JSON.stringify(results)
}, (err, res)=>{
    if(err){
        console.log(err)
    }
    else{
        console.log(res)
    }
    
})