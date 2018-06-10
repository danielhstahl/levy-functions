const handler=require('../../lambda/calibratorLambda')
const calibratorParams={
    "numU":8,
    "r":0.003,
    "T":1,
    "S0":178.46,
    "lambda":0,
    "muJ":2.5,
    "sigJ":0.3,
    "variable":{
        "sigma":0.4,
        "v0":0.9,
        "speed":0.5,
        "adaV":0.4,
        "rho":-0.4
    },
    "constraints":{
        "sigma": {
            "upper":2.0,
            "lower":0.0
        }
    },
    "k":[95,130,150,160,165,170,175,185,190,195,200,210,240,250],
    "prices":[85,51.5,35.38,28.3,25.2,22.27,19.45,14.77,12.75,11,9.35,6.9,2.55,1.88]
}
const createEvent=(data, parameters, queryStringParameters)=>({
    body:JSON.stringify(data),
    pathParameters:parameters,
    queryStringParameters
})

it('correctly calls calibrator handler for full model', (done)=>{
    console.time("calibrator")
    const event=createEvent(calibratorParams, {calibration:'calibrate'})
    handler.calibrator(event, {}, (err, val)=>{
        console.log(val.body)
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.sigma).toBeDefined()
        expect(parsedVal.speed).toBeDefined()
        expect(parsedVal.adaV).toBeDefined()
        expect(parsedVal.rho).toBeDefined()
        console.timeEnd("calibrator")
        done()
    })
}, 40000)

it('correctly calls calibrator handler for spline', (done)=>{
    const event=createEvent(calibratorParams, {calibration:'spline'})
    handler.calibrator(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.curve).toBeDefined()
        expect(parsedVal.points).toBeDefined()
        done()
    })
})
it('correctly sends error  for full model', (done)=>{
    const args={
        "numU":8,
        "r":0.003,
        "T":1,
        "S0":178.46,
        "sigma":0.4,
        "v0":0.9,
        "speed":0.5,
        "variable":{
            "lambda":0,
            "muJ":2.5,
            "sigJ":4,
        },
        "k":[95,100,130,150,160,165,170,175,185,190,195,200,210,240,250],
        "prices":[85,78.7,51.5,35.38,28.3,25.2,22.27,19.45,14.77,12.75,11,9.35,6.9,2.55,1.88]
    }
    const event=createEvent(args, {calibration:'calibrate'})
    handler.calibrator(event, {}, (err, val)=>{
        expect(val.body).toEqual("Requires additional keys!  Missing rho")
        done()
    })
}, 40000) //takes a while, so 20 seconds
