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
    const kappa=speed
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
        q:5,
        k:[100]
    }
    const event=createEvent(parameters, {
        optionType:'call',
        sensitivity:'price',
        algorithm:'fangoost'
    })
    return handler.calculator(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal[1].value).toBeCloseTo(5.78515545, 3)
        done()
    })
})

it('correctly returns generic price', (done)=>{
    //own MC
    const parameters={
        numU:8,
        r:.03,
        T:1.0,
        S0:50,
        sigma:.2, 
        lambda:.5,
        muJ:-.05,
        sigJ:.1,
        speed:.3,
        v0:.9,
        adaV:.2,
        rho:-.5,
        delta:1,
        q:5,
        k:[50]
    }
    const event=createEvent(parameters, {
        optionType:'call',
        sensitivity:'price',
        algorithm:'fangoost'
    })
    return handler.calculator(event, {}, (err, val)=>{
        const parsedVal=JSON.parse(val.body)
        expect(parsedVal[1].value).toBeCloseTo(5.431071, 3)
        done()
    })
})
/**r=.03
sig=.2
sigL=.1
muL=-.05
rho=-.5
q=5 #size of jump is .2 on average
lambda=.5 #one jumps every two years on average
a=.3
eta=.2
v0=.9
s0=50
k=50
delta=1 */