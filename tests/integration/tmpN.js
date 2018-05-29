const handler=require('../../lambda/handler')
const createEvent=(data, parameters)=>({
    body:JSON.stringify(data),
    pathParameters:parameters
})
const parameters={
    "numU":8,
    "r":0.003,
    "T":1,
    "S0":178.46,
    
    "variable":{
        "C":0.1,
        "M":2.5,
        "G":4,
        "Y":1.3,
        "v0":1,
        "speed":.1,
        "delta":.5
    },
    constraints:{

    },
    "k":[95,130,150,160,165,170,175,185,190,195,200,210,240,250],
    "prices":[85,51.5,35.38,28.3,25.2,22.27,19.45,14.77,12.75,11,9.35,6.9,2.55,1.88]
}
const event=createEvent(parameters, {calibration:'calibrate'})
console.time("calibrator")
handler.calibrator(event, {}, (err, val)=>{
    console.log(val.body)
    console.timeEnd("calibrator")
})