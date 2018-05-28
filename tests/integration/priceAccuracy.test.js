const handler=require('../../lambda/handler')
const createEvent=(data, parameters)=>({
    body:JSON.stringify(data),
    pathParameters:parameters
})
it('correctly returns heston price', (done)=>{
    //http://ta.twi.tudelft.nl/mf/users/oosterle/oosterlee/COS.pdf pg 15
    const r=0.0
    const T=1.0
    const S0=100
    const b=.0398
    const a=1.5768
    const c=.5751
    const rho=-.5711
    const v0=.0175

    //convert parameters
    const sigma=Math.sqrt(b)
    const speed=a
    const v0Hat=v0/b
    const adaV=c/Math.sqrt(b)

    const parameters={
        numU:8,
        r,
        T,
        S0,
        sigma, 
        lambda:0,
        muJ:0,
        sigJ:0,
        speed,
        v0:v0Hat,
        adaV,
        rho,
        delta:0,
        k:[100]
    }
    const event=createEvent(parameters, {
        optionType:'call',
        sensitivity:'price',
        algorithm:'fangoost'
    })
    return handler.calculator(event, {}, (err, val)=>{
        console.log(val.body)
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal[1].value).toBeCloseTo(5.78515545, 3)
        done()
    })
})
it('correctly returns merton price', (done)=>{
    //https://www.upo.es/personal/jfernav/papers/Jumps_JOD_.pdf pg 8
    const r=.1
    const T=.5
    const S0=38
    const sigJ=Math.sqrt(.05)
    const sigma=sigJ
    const muJ=-sigJ*sigJ*.5
    const k=35
    const lambda=1
    
    const parameters={
        numU:8,
        r,
        T,
        S0,
        sigma, 
        lambda,
        muJ,
        sigJ,
        speed:0,
        v0:1,
        adaV:0,
        rho:0,
        delta:0,
        k:[k]
    }
    const event=createEvent(parameters, {
        optionType:'call',
        sensitivity:'price',
        algorithm:'fangoost'
    })
    return handler.calculator(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal[1].value).toBeCloseTo(5.9713, 3)
        done()
    })
})


var start = process.hrtime();

it('calls calibrator handler and finishes in under 20 seconds', (done)=>{
    const parameters={
        "numU":8,
        "r":0.003,
        "T":1,
        "S0":178.46,
        "delta":0, //if this is included, will take longer than 20 seconds
        "variable":{
            "sigma":0.4,
            "v0":0.9,
            "speed":0.5,
            "adaV":0.4,
            "rho":-0.4,
            "lambda":0.1,
            "muJ":2.5,
            "sigJ":0.3
        },
        constraints:{

        },
        "k":[95,130,150,160,165,170,175,185,190,195,200,210,240,250],
        "prices":[85,51.5,35.38,28.3,25.2,22.27,19.45,14.77,12.75,11,9.35,6.9,2.55,1.88]
    }
    console.time("calibrator")
    let start=process.hrtime()
    const event=createEvent(parameters, {calibration:'calibrate'})
    handler.calibrator(event, {}, (err, val)=>{
        console.log(val.body)
        const twentySeconds=20
        expect(process.hrtime(start)[0]).toBeLessThan(twentySeconds)
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.sigma).toBeDefined()
        expect(parsedVal.speed).toBeDefined()
        expect(parsedVal.adaV).toBeDefined()
        expect(parsedVal.rho).toBeDefined()
        console.timeEnd("calibrator")
        done()
    })
}, 100000)


it('correctly calls calibrator handler and matches call prices with fake data', (done)=>{
    const parameters={
        numU:8,
        r:.03,
        T:1.0,
        S0:178.46,
        sigma:.2, 
        lambda:.5,
        muJ:-.05,
        sigJ:.1,
        speed:.3,
        v0:.9,
        adaV:.2,
        rho:-.5,
        k:[95,130,150,160,165,170,175,185,190,195,200,210,240,250]
    }
    const event=createEvent(parameters, {
        optionType:'call',
        sensitivity:'price',
        algorithm:'fangoost'
    })
    return handler.calculator(event, {}, (err, val)=>{
        console.log(val.body)
        const parsedVal=JSON.parse(val.body)
        
        const calParameters={
            "numU":8,
            "r":0.003,
            "T":1,
            "S0":178.46,
            "variable":{
                "sigma":0.4,
                "v0":0.9,
                "speed":0.5,
                "adaV":0.4,
                "rho":-0.4,
                "lambda":0.1,
                "muJ":2.5,
                "sigJ":0.3
            },
            constraints:{
    
            },
            "k":parameters.k,
            "prices":parsedVal.filter((v, i)=>i!==0&&i!==(parsedVal.length-1)).map((v, i)=>v.value)
        }
        const event=createEvent(calParameters, {calibration:'calibrate'})
        handler.calibrator(event, {}, (err, val)=>{
            console.log(val.body)
            const calibrateVal=JSON.parse(val.body)
            const calculatorParameters={
                ...calParameters,
                ...calibrateVal
            }
            //console.log(calculatorParameters)
            const calculatorEvent=createEvent(calculatorParameters, {
                optionType:'call',
                sensitivity:'price',
                algorithm:'fangoost'
            })
            return handler.calculator(calculatorEvent, {}, (err, val)=>{
                const calcVal=JSON.parse(val.body)
                const criteriaDiff=1 //less than a dollar off
                console.log(calcVal)
                
                //console.log(parameters.prices)
                const prices=calculatorParameters.prices
                console.log(prices)
                calcVal.filter((v, i)=>i!==0&&i!==(calcVal.length-1)).map((v, i)=>{
                    const diff=Math.abs(v.value-prices[i])
                    expect(diff).toBeLessThan(criteriaDiff)
                })
                
                done()
            })
        })
    })
    
    
}, 100000)

