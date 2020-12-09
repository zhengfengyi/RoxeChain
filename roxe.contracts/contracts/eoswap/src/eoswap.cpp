#include <roxe/asset.hpp>
#include <roxe/roxe.hpp>
#include <roxe/name.hpp>
#include <roxe/symbol.hpp>
#include <roxe/system.hpp>
#include <roxe/transaction.hpp>
#include <string>
#include <vector>

#include "extended_token.cpp"
#include <cmath>
#include <common/BType.hpp>
#include <common/extended_token.hpp>
#include <eoswap/BFactory.hpp>
#include <eoswap/BPool.hpp>
#include <storage/BFactoryTable.hpp>
#include <storage/BPoolTable.hpp>
#include <storage/BTokenTable.hpp>

using roxe::action;
using roxe::asset;
using roxe::name;
using roxe::permission_level;
using std::string;

class [[roxe::contract("eoswap")]] eoswap : public roxe::contract {
 private:
   instance_mgmt  _instance_mgmt;
   BFactory       factory;
   extended_token extoken;

 public:
//    static constexpr roxe::name admin_account{"eoswapeoswap"_n};
//    static constexpr roxe::name controller_account{"poolmanagers"_n};
//    static constexpr roxe::name tokenissuer_account{"tokenissuers"_n};
//    static constexpr roxe::name swaptoken_account{"eoswapxtoken"_n};
   //    static constexpr extended_symbol weth_symbol = {symbol(symbol_code("WETH"), 4), "eosdosxtoken"_n};

   eoswap(name s, name code, roxe::datastream<const char*> ds)
       : contract(s, code, ds)
       , _instance_mgmt(s)
       , factory(s, _instance_mgmt)
       , extoken(s) {}

   //////////////////factory////////////////////////
   [[roxe::action]] void setblabs(name msg_sender, name blabs) {
    
      factory.setMsgSender(msg_sender);
      factory.setBLabs(blabs);
   }

   [[roxe::action]] void collect(name msg_sender, name pool_name) {
    
      factory.setMsgSender(msg_sender);
      factory.collect(pool_name);
   }

   [[roxe::action]] void newpool(name msg_sender, name pool_name) {
    
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.newBPool(pool_name);
      factory.setMsgSender(msg_sender);
      factory.enableBPool(pool_name);
   }

   //////////////////POOL////////////////////////
   [[roxe::action]] void setswapfee(name msg_sender, name pool_name, uint64_t swapFee) {
    
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.setSwapFee(swapFee); });
   }

   [[roxe::action]] void setcontroler(name msg_sender, name pool_name, name manager) {
    
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.setController(manager); });
   }

   [[roxe::action]] void setpubswap(name msg_sender, name pool_name, bool public_) {
    
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.setPublicSwap(public_); });
   }

   [[roxe::action]] void finalize(name msg_sender, name pool_name) {
    
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.finalize(); });
   }
   // _lock_  Bind does not lock because it jumps to `rebind`, which does

   [[roxe::action]] void bind(name msg_sender, name pool_name, const extended_asset& balance, uint64_t denorm) {
     extended_asset balances = convert_one_decimals(balance);
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.bind(balances, denorm); });
   }

   [[roxe::action]] void rebind(name msg_sender, name pool_name, const extended_asset& balance, uint64_t denorm) {
      extended_asset balances = convert_one_decimals(balance);
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.rebind(balances, denorm); });
   }

   [[roxe::action]] void unbind(name msg_sender, name pool_name, const extended_symbol& token) {
    
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.unbind(token); });
   }

   // Absorb any _token_ that have been sent to this contract into the pool

   [[roxe::action]] void gulp(name msg_sender, name pool_name, const extended_symbol& token) {
    
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.gulp(token); });
   }

   [[roxe::action]] void joinpool(
       name msg_sender, name pool_name, uint64_t poolAmountOut, std::vector<uint64_t> maxAmountsIn) {
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.joinPool(poolAmountOut, maxAmountsIn); });
   }

   [[roxe::action]] void exitpool(
       name msg_sender, name pool_name, uint64_t poolAmountIn, std::vector<uint64_t> minAmountsOut) {
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.exitPool(poolAmountIn, minAmountsOut); });
   }

   [[roxe::action]] void swapamtin(
       name msg_sender, name pool_name, const extended_asset& tokenAmountIn, const extended_asset& minAmountOut,
       uint64_t maxPrice) {
     extended_asset tokenAmountIns = convert_one_decimals(tokenAmountIn);
     extended_asset minAmountOuts = convert_one_decimals(minAmountOut);

      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(
          pool_name, [&](auto& pool) { pool.swapExactAmountIn(tokenAmountIns, minAmountOuts, maxPrice); });
   }

   [[roxe::action]] void swapamtout(
       name msg_sender, name pool_name, const extended_asset& maxAmountIn, const extended_asset& tokenAmountOut,
       uint64_t maxPrice) {
 extended_asset maxAmountIns = convert_one_decimals(maxAmountIn);
     extended_asset tokenAmountOuts = convert_one_decimals(tokenAmountOut);
      _instance_mgmt.setMsgSender(msg_sender);
      _instance_mgmt.pool(
          pool_name, [&](auto& pool) { pool.swapExactAmountOut(maxAmountIns, tokenAmountOuts, maxPrice); });
   }

   ////////////////// TEST pool storage///////////////////////
   [[roxe::action]] void cppool2table(name msg_sender, name pool_name) {
    //   check(admin_account == msg_sender, "no admin");
      _instance_mgmt.get_storage_mgmt().copyPoolStore2Table(msg_sender, pool_name);
   }

   ////////////////// TEST pool TOKEN////////////////////////
   [[roxe::action]] void extransfer(name from, name to, extended_asset quantity, std::string memo) {
      _instance_mgmt.get_transfer_mgmt().transfer(from, to, quantity, memo);
   }

   [[roxe::action]] void newtoken(name msg_sender, const extended_asset& token) {
    //   check(tokenissuer_account == msg_sender, "no token issuer");
      _instance_mgmt.get_transfer_mgmt().create(msg_sender, token);
   }

   [[roxe::action]] void newtokenex(name msg_sender, const extended_asset& token) {
    //   check(tokenissuer_account == msg_sender, "no token issuer");
         BToken       otoken(_self,token.get_extended_symbol());
        otoken.setMsgSender(msg_sender);
        otoken.create(msg_sender,token);
    }

   [[roxe::action]] void transferex(name msg_sender, name dst, const extended_asset& amt) {
      BToken token(_self, amt.get_extended_symbol());
      token.setMsgSender(msg_sender);
      token.transfer(dst, amt.quantity.amount);
   }

   /////test interface /////
   [[roxe::action]] void mint(name msg_sender, const extended_asset& amt) {
      _instance_mgmt.get_transfer_mgmt().issue(msg_sender, amt, "");
   }

   [[roxe::action]] void mintex(name msg_sender, const extended_asset& amt) {
      BToken token(_self, amt.get_extended_symbol());
      token.setMsgSender(msg_sender);
      token._mint(amt.quantity.amount);
   }

   [[roxe::action]] void burn(name msg_sender, const extended_asset& amt) {
    //   check(tokenissuer_account == msg_sender, "no token issuer");
      _instance_mgmt.get_transfer_mgmt().burn(msg_sender, amt, "");
   }

   [[roxe::action]] void burnex(name msg_sender, const extended_asset& amt) {
    //   check(tokenissuer_account == msg_sender, "no token issuer");
      BToken token(_self, amt.get_extended_symbol());
      token.setMsgSender(msg_sender);
      token._burn(amt.quantity.amount);
   }

//    ////////////////////extended_token////////////////////

//    /**
//     * Allows `issuer` account to create a token in supply of `maximum_supply`. If validation is successful a new entry
//     * in statstable for token symbol scope gets created.
//     *
//     * @param issuer - the account that creates the token,
//     * @param maximum_supply - the maximum supply set for the token created.
//     *
//     * @pre Token symbol has to be valid,
//     * @pre Token symbol must not be already created,
//     * @pre maximum_supply has to be smaller than the maximum supply allowed by the system: 1^62 - 1.
//     * @pre Maximum supply must be positive;
//     */
//    [[roxe::action]] void create(const name& issuer, const extended_asset& maximum_supply) {
//       extoken.create(issuer, maximum_supply);
//    }
//    /**
//     *  This action issues to `to` account a `quantity` of tokens.
//     *
//     * @param to - the account to issue tokens to, it must be the same as the issuer,
//     * @param quntity - the amount of tokens to be issued,
//     * @memo - the memo string that accompanies the token issue transaction.
//     */
//    [[roxe::action]] void issue(const name& to, const extended_asset& quantity, const string& memo) {
//       extoken.issue(to, quantity, memo);
//    }

//    /**
//     * The opposite for create action, if all validations succeed,
//     * it debits the statstable.supply amount.
//     *
//     * @param quantity - the quantity of tokens to retire,
//     * @param memo - the memo string to accompany the transaction.
//     */
//    [[roxe::action]] void retire(const extended_asset& quantity, const string& memo) { extoken.retire(quantity, memo); }

//    /**
//     * Allows `from` account to transfer to `to` account the `quantity` tokens.
//     * One account is debited and the other is credited with quantity tokens.
//     *
//     * @param from - the account to transfer from,
//     * @param to - the account to be transferred to,
//     * @param quantity - the quantity of tokens to be transferred,
//     * @param memo - the memo string to accompany the transaction.
//     */
//    [[roxe::action]] void transfer(
//        const name& from, const name& to, const extended_asset& quantity, const string& memo) {
//       extoken.transfer(from, to, quantity, memo);
//    }
//    /**
//     * Allows `ram_payer` to create an account `owner` with zero balance for
//     * token `symbol` at the expense of `ram_payer`.
//     *
//     * @param owner - the account to be created,
//     * @param symbol - the token to be payed with by `ram_payer`,
//     * @param ram_payer - the account that supports the cost of this action.
//     *
//     * More information can be read [here](https://github.com/EOSIO/roxe.contracts/issues/62)
//     * and [here](https://github.com/EOSIO/roxe.contracts/issues/61).
//     */
//    [[roxe::action]] void open(const name& owner, const extended_symbol& symbol, const name& ram_payer) {
//       extoken.open(owner, symbol, ram_payer);
//    }

//    /**
//     * This action is the opposite for open, it closes the account `owner`
//     * for token `symbol`.
//     *
//     * @param owner - the owner account to execute the close action for,
//     * @param symbol - the symbol of the token to execute the close action for.
//     *
//     * @pre The pair of owner plus symbol has to exist otherwise no action is executed,
//     * @pre If the pair of owner plus symbol exists, the balance has to be zero.
//     */
//    [[roxe::action]] void close(const name& owner, const extended_symbol& symbol) { extoken.close(owner, symbol); }

   ////////////////////on_notify////////////////////
   [[roxe::on_notify("roxe.token::transfer")]] void on_transfer(
       name from, name to, asset quantity, std::string memo) {
      check(get_first_receiver() == "roxe.token"_n, "should be roxe.token");
      //   print_f("On notify : % % % %", from, to, quantity, memo);
      _instance_mgmt.get_transfer_mgmt().eosiotoken_transfer(from, to, quantity, memo, [&](const auto& action_event) {
         if (action_event.action.empty()) {
            return;
         }

         if (action_event.action.compare(bind_action_string) == 0) {
            auto           paras = transfer_mgmt::parse_string(action_event.param);
            name           pool_name;
            extended_asset balance;
            uint64_t       denorm;
            _instance_mgmt.setMsgSender(action_event.msg_sender);
            _instance_mgmt.pool(pool_name, [&](auto& pool) { pool.bind(balance, denorm); });
         }
      });
   }

   [[roxe::on_notify("*::transfer")]] void on_transfer_by_non(name from, name to, asset quantity, std::string memo) {
      check(get_first_receiver() != "roxe.token"_n, "should not be roxe.token");
      //   print_f("On notify 2 : % % % %", from, to, quantity, memo);
      _instance_mgmt.get_transfer_mgmt().non_eosiotoken_transfer(
          from, to, quantity, memo, [&](const auto& action_event) {

          });
   }
};
