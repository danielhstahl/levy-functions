const handler=require('./handler')
const calibratorParams=require('./exampleCalibrator.json')
const createEvent=data=>({
    body:JSON.stringify(data)
})
it('correctly calls calculator handlers', ()=>{
    const event=createEvent({
        k:[40, 50, 60]
    })
    return Promise.all(handler.calculatorKeys.filter(val=>val!=='VaR').map(key=>{
        return new Promise((resolve, reject)=>{
            handler[key](event, {}, (err, val)=>{
                const parsedVal=JSON.parse(val.body)
                resolve(expect(Array.isArray(parsedVal)).toEqual(true))
            })
        })
    }))
})
it('correctly calls calibrator handler for full model', (done)=>{
    const event=createEvent(calibratorParams)
    handler.fullmodel(event, {}, (err, val)=>{
        console.log(val.body)
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.sigma).toBeDefined()
        expect(parsedVal.C).toBeDefined()
        expect(parsedVal.G).toBeDefined()
        expect(parsedVal.M).toBeDefined()
        expect(parsedVal.Y).toBeDefined()
        expect(parsedVal.speed).toBeDefined()
        expect(parsedVal.adaV).toBeDefined()
        expect(parsedVal.rho).toBeDefined()
        done()
    })
}, 40000) //takes a while, so 20 seconds
it('correctly sends error  for full model', (done)=>{
    const args={
        "numU":8,
        "r":0.003,
        "T":1,
        "S0":178.46,
        "static":{
            "sigma":0.4,
            "v0":0.9,
            "speed":0.5
        },
        "variable":{
            "C":0,
            "G":2.5,
            "M":4,
            "Y":0.4
        },
        "k":[95,100,130,150,160,165,170,175,185,190,195,200,210,240,250],
        "prices":[85,78.7,51.5,35.38,28.3,25.2,22.27,19.45,14.77,12.75,11,9.35,6.9,2.55,1.88]
    }
    const event=createEvent(args)
    handler.fullmodel(event, {}, (err, val)=>{
        //expect(err).toEqual()
        expect(val.body).toEqual("Requires additional keys!  Missing rho")
        done()
    })
}, 40000) //takes a while, so 20 seconds

it('correctly calls VaR', (done)=>{
    const event=createEvent({})
    handler.VaR(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.VaR).toBeDefined()
        expect(parsedVal.ES).toBeDefined()
        done()
    })
})
