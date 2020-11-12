import { RefactoringTableJsonMin } from "./RefactoringTableJsonMin"

const jq = require('node-jq');
const { chain } = require('../../../eos-rpc');
// import { chain } from '../../../eos-rpc';
const c = chain();
const prettyJson = async (log: any) => {
    let jsonstr = await jq.run('.', JSON.stringify(log), { input: 'string', output: 'pretty' });
    console.log(jsonstr);
};

export class PricingApi {

    async getDodos() {
        const res = await c.get_table_rows('eosdoseosdos', 'eosdoseosdos', 'dodo', true);
        // console.log(JSON.stringify(res));
        // await prettyJson(res);
        return res;
    }
    async getOraclePrices() {
        const res = await c.get_table_rows('eosdoseosdos', 'eosdoseosdos', 'oracle', true);
        // console.log(JSON.stringify(res));
        // await prettyJson(res);

        return res;
    }

    async getDodo() {
        let dodo = await this.getDodos();
        let oracle = await this.getOraclePrices();
        let dodojsonstr = new RefactoringTableJsonMin().refactoringTableDataJson(dodo, oracle);
        // prettyJson(dodojsonstr);
        return dodojsonstr;
    }
}

(async function () {
    // const api = new PricingApi();
    // let b: any = await api.getDodo();
    // console.log("==b==", b, "=====");
    // let s: any = await api.querySellToken(10000, "DAI", "MKR");
    // console.log("=s==", s, "===");
})();





