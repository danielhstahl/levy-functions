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
        })})
    }))
})
it('correctly calls calibrator handler for full model', (done)=>{
    const event=createEvent(calibratorParams)
    handler.fullmodel(event, {}, (err, val)=>{
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
it('correctly calls calibrator handler for heston model', (done)=>{
    const event=createEvent(calibratorParams)
    handler.hestonmodel(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.sigma).toBeDefined()
        expect(parsedVal.speed).toBeDefined()
        expect(parsedVal.adaV).toBeDefined()
        expect(parsedVal.rho).toBeDefined()
        done()
    })
}, 40000) //takes a while, so 20 seconds
it('correctly calls calibrator handler for bs model', (done)=>{
    const event=createEvent(calibratorParams)
    handler.bsmodel(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.sigma).toBeDefined()
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
