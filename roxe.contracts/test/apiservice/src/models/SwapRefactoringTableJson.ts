// import { isConstructorDeclaration } from "typescript";
import './arr2obj';

export class SwapRefactoringTableJson {


    async refactoringTableDataJson(tablejson: any) {
        // console.log(pooltablejson,oracletablejson);
        // let pooltablejson = JSON.parse(pooltablejsonstr);
        // let oracletablejson = JSON.parse(oracletablejsonstr);

        let pools = await this.refactoringPoolTableJson(tablejson);
        return pools;
        // return JSON.stringify(pools);
    }

    async refactoringPoolTableJson(pooltablejson: any) {
        let pools = pooltablejson.rows[0]["pools"];
        let poolsobj = arrToObjES2019(pools);
        const actionjson = Object.keys().filter((obj: any) => a.indexOf(obj.name) >= 0);


        let allpools: any = {};
        for (let d of pools) {
            allpools[d.pool] = {};
            let basetoken = d.pools._BASE_TOKEN_.symbol.split(",")[1];
            let quotetoken = d.pools._QUOTE_TOKEN_.symbol.split(",")[1];
            for (let f of this.refactoring_fields) {
                allpools[d.pool][f] = d.pools[f];
            }
        }

        return allpools;
    }

    // async refactoringOracleTableJson(oracletablejson: any) {
    //     let oracles = oracletablejson.rows;//[0]["oracles"];
    //     let alloracles: any = {};
    //     for (let oracle of oracles) {
    //         let b = oracle.basetoken.symbol.split(",");
    //         let q = oracle.quotetoken.quantity.split(" ");
    //         alloracles[b[1]] = {};
    //         alloracles[b[1]][q[1]] = q[0];
    //         // Object.assign(alloracles, { [b[1]]: { [q[1]]: q[0] } });
    //     }

    //     return alloracles;
    // }
}

function testRefactoring() {
    const pooltablerows = {
        "rows": [
            {
                "pools": [
                    {
                        "key": "pool",
                        "value": {
                            "mutex": 0,
                            "factory": "eoswapeoswap",
                            "controller": "eoswapeoswap",
                            "publicSwap": 1,
                            "swapFee": 3000,
                            "finalized": 1,
                            "tokens": [
                                "0x04574554480000003015a4b957c33155",
                                "0x04444149000000003015a4b957c33155"
                            ],
                            "records": [
                                {
                                    "key": "0x04444149000000003015a4b957c33155",
                                    "value": {
                                        "bound": 1,
                                        "index": 1,
                                        "denorm": 5000000,
                                        "balance": 2400000,
                                        "exsym": {
                                            "symbol": "4,DAI",
                                            "contract": "eoswapxtoken"
                                        }
                                    }
                                },
                                {
                                    "key": "0x04574554480000003015a4b957c33155",
                                    "value": {
                                        "bound": 1,
                                        "index": 0,
                                        "denorm": 5000000,
                                        "balance": 60000,
                                        "exsym": {
                                            "symbol": "4,WETH",
                                            "contract": "eoswapxtoken"
                                        }
                                    }
                                }
                            ],
                            "totalWeight": 10000000
                        }
                    }
                ]
            }
        ],
        "more": false
    };


    // let pooltablerowsstr = JSON.stringify(pooltablerows);
    // let oracletablerowsstr = JSON.stringify(oracletablerows);
    let tablejson = new SwapRefactoringTableJson().refactoringTableDataJson(pooltablerows, oracletablerows);
    console.log("==tablejson==", tablejson, "=====");

}


// testRefactoring();



