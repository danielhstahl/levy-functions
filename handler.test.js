const handler=require('./handler')
const createEvent=data=>({
    body:JSON.stringify(data)
})
it('correctly calls all handlers', ()=>{
    const event=createEvent({
        k:[40, 50, 60]
    })
    return Promise.all(handler.keys.filter(val=>val!=='VaR').map(key=>{
        return new Promise((resolve, reject)=>{
            handler[key](event, {}, (err, val)=>{
                const parsedVal=JSON.parse(val.body)
                resolve(expect(Array.isArray(parsedVal)).toEqual(true))
        })})
    }))
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
