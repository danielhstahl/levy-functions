const csvFilePath='./apple_call_jan_19_2018.csv'
const csv=require('csvtojson')
const fs=require('fs')
const strikes=[]
const prices=[]
const records=[]
const restOfJson={
    numU:8,
    r:.003,
    T:1.0,
    S0:178.46
}
const objArrToArr=key=>objArr=>objArr.map(val=>val[key])
csv({checkType:true, header:true})
.fromFile(csvFilePath)
.on('json',({Strike, Price, Volume})=>{
    records.push({Strike, Price, Volume})
    //strikes.push(Strike)
    //prices.push(Price)
})
.on('done',(error)=>{
    const onlyLiquid=records.filter(({Volume})=>Volume>100)
    fs.writeFile('exampleCalibrator.json', JSON.stringify({...restOfJson, k:objArrToArr('Strike')(onlyLiquid), prices:objArrToArr('Price')(onlyLiquid)}), 'utf8', (err, res)=>{
        console.log(err);
        console.log("done")
    });
})