const { chain, wallet } = require('../index');
const w = wallet();
const c = chain();

const { PRIVATE_KEY_1 } = require('../config');

describe('X-Action - #create() ', () => {
    it('should create a transaction', () => {

        const expiration = new Date(new Date().getTime() + (1 * 60000)).toISOString().split('.')[0]; //"2018-01-09T10:28:49"
        const actions = [{
            account: 'eosio.token',
            name: 'transfer',
            authorization: [{ actor: 'useraaaaaaab', permission: 'active' }],
            data: '',

        }];
        let signatures = [];
        const scope = ["useraaaaaaaa", "useraaaaaaab"];

        // get last block num
        c.get_info()
            .then(info => {
                const ref_block_num = info.last_irreversible_block_num

                // get info on that block.
                c.get_block(ref_block_num)
                    .then(b => {
                        const ref_block_prefix = b.ref_block_prefix;

                        // get abi_json_to_bin
                        c.abi_json_to_bin("eosio.token", "transfer", { from: "useraaaaaaab", to: "useraaaaaaaa", quantity: '0.0001 SYS', memo: '' })
                            .then(bin => {
                                // set data in message
                                actions[0].data = bin.binargs;

                                // get signature
                                w.wallet_sign_trx([{
                                    ref_block_num,
                                    ref_block_prefix,
                                    expiration,
                                    actions,
                                    signatures,
                                },
                                [PRIVATE_KEY_1],
                                    ''
                                ])
                                    .then(sig => {
                                        const compression = "none";
                                        const transaction_extensions = [];
                                        const context_free_actions = [];
                                        // push the transaction
                                        c.push_transaction(compression, { expiration, ref_block_num, ref_block_prefix, context_free_actions, actions, transaction_extensions }, sig.signatures)
                                            .then(xAction => {
                                                console.log(xAction);
                                                expect(xAction).toBeDefined()
                                                expect(xAction.transaction_id).toBeDefined()
                                                expect(xAction.processed).toBeDefined()
                                                expect(xAction.processed.ref_block_num).toBeDefined()
                                                expect(xAction.processed.ref_block_prefix).toBeDefined()
                                            })
                                            .catch(e => {
                                                console.log('eee ', e);
                                            }) // push xaction
                                    }) // get sig
                                    .catch(e => console.error('get sig err ', e.response.text))
                            }) // abi
                    }) // get block
            }) // get info
    }) // it
}) // define
