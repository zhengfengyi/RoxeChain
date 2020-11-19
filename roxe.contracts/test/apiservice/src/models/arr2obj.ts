const refactoring_fields: any[] = [
    "swapFee",
    "totalWeight",
    "records"
];

const refactoring_records_fields: any[] = [
    "denorm",
    "balance",
    "exsym"
];


// If your target environment supports ES2019, you could use Object.fromEntries(), like this:

function arrToObjES2019(arr: any[]) {
    return Object.fromEntries(arr.map(({ key, value }) => [key, 
Object.keys(value).filter((v: any) => refactoring_fields.indexOf(v) >= 0).reduce((obj,p) => {
obj[p]=value[p];
Object.assign(obj,arrRecordsToObjES2019(obj.records));
return obj;
}
)
]));
}

function arrRecordsToObjES2019(arr: any[]) {
    return Object.fromEntries(arr.map(({ key, value }) => [key, 
Object.keys(value).filter((v: any) => refactoring_records_fields.indexOf(v) >= 0).reduce((obj,p) => {
obj[p]=value[p];
let sym = obj.exsym.symbol.split(",")[1];
Object.assign(obj,{sym:{p:value[p]}};
return obj;
},{})
]));
}


// Or, if not, you can make your own polyfill-like version of Object.fromEntries() using array reduce() on an empty object, like this:

function fromEntries<V>(iterable: Iterable<[string, V]>) {
    return [...iterable].reduce((obj, [key, val]) => {
        obj[key] = val
        return obj
    }, {} as { [k: string]: V })
}
// and then use it:

function arrToObj(arr: any[]) {
    return fromEntries(arr.map(({ key, value }) => [key, value]));
}
// Either way should let you do what you want:

// const arr: MyObject[] = [
//   { id: "id1", position: 1 },
//   { id: "id2", position: 2 }
// ];

// console.log(JSON.stringify(arrToObj(arr))); // {"id1":1,"id2":2}
// Okay, hope that helps. Good luck!


