const handler=require('../../lambda/calculatorLambda')

const createEvent=(data, parameters, queryStringParameters)=>({
    body:JSON.stringify(data),
    pathParameters:parameters,
    queryStringParameters
})
it('correctly calls calculator handlers', (done)=>{
    const event=createEvent({
        k:[40, 50, 60]
    }, {
        optionType:'call',
        sensitivity:'price',
        algorithm:'fangoost'
    })
    return handler.calculator(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(Array.isArray(parsedVal)).toEqual(true)
        done()
    })
})
it('correctly calls defaultParameters', (done)=>{
    handler.defaultParameters({}, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.sigma).toBeDefined()
        expect(parsedVal.speed).toBeDefined()
        expect(parsedVal.adaV).toBeDefined()
        expect(parsedVal.rho).toBeDefined()
        done()
    })
})

it('correctly calls VaR', (done)=>{
    const event=createEvent({}, {
        densityType:'var'
    })
    handler.density(event, {}, (err, val)=>{
        console.log(val.body)
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal.VaR).toBeDefined()
        expect(parsedVal.ES).toBeDefined()
        done()
    })
})
