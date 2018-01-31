const request=require('request')
const body1=require('./exampleCalibrator.json')
const body2=require('./exampleCalculator.json')
const url1='https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/call/calibration/full'
const url2='https://ni6jd9f0z4.execute-api.us-east-1.amazonaws.com/dev/put/price/fangoost'
const url3='https://us-central1-optiondashboard.cloudfunctions.net/fangoostput'
request.post(url1, {timeout:120000, body:JSON.stringify(body1)}, (e, response, body)=>{
    console.log(e)
   // console.log(response)
    console.log(body)
})