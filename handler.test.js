const handler=require('./handler')
const createEvent=data=>({
    body:JSON.stringify(data)
})

it('correctly calls fangoostcall', (done)=>{
    const event=createEvent({
        k:[40, 50, 60]
    })
    handler.fangoostcall(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls fangoostcalldelta', (done)=>{
    const event=createEvent({
        k:[40, 50, 60]
    })
    handler.fangoostcalldelta(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls fangoostput', (done)=>{
    const event=createEvent({
        k:[40, 50, 60]
    })
    handler.fangoostput(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls fangoostputdelta', (done)=>{
    const event=createEvent({
        k:[40, 50, 60]
    })
    handler.fangoostputdelta(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls carrmadanput', (done)=>{
    const event=createEvent({})
    handler.carrmadanput(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls carrmadancall', (done)=>{
    const event=createEvent({})
    handler.carrmadancall(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls fstsput', (done)=>{
    const event=createEvent({})
    handler.fstsput(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls fstsputdelta', (done)=>{
    const event=createEvent({})
    handler.fstsputdelta(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls fstscall', (done)=>{
    const event=createEvent({})
    handler.fstscall(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls fstscalldelta', (done)=>{
    const event=createEvent({})
    handler.fstscalldelta(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls VaR', (done)=>{
    const event=createEvent({})
    handler.VaR(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.VaR).toBeDefined()
        expect(parsedVal.ES).toBeDefined()
        done()
    })
})
it('correctly calls density', (done)=>{
    const event=createEvent({})
    handler.density(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
