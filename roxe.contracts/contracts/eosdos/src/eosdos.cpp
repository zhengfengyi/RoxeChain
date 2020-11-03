#include <roxe/asset.hpp>
#include <roxe/roxe.hpp>
#include <roxe/name.hpp>
#include <roxe/symbol.hpp>
#include <roxe/system.hpp>
#include <roxe/transaction.hpp>
#include <string>
#include <vector>

#include <cmath>
#include <common/defines.hpp>
#include <eosdos/DODOEthProxy.hpp>
using roxe::action;
using roxe::asset;
using roxe::name;
using roxe::permission_level;
enum ACTION_STEP { STEP_ONE = 1, STEP_TWO, STEP_THREE };

class [[roxe::contract("eosdos")]] eosdos : public roxe::contract {
 private:
   instance_mgmt _instance_mgmt;
   DODOZoo       zoo;
   DODOEthProxy  proxy;

 public:
   static constexpr roxe::name     admin_account{"eosdoseosdos"_n};
   static constexpr roxe::name     doowner_account{"dodoowner111"_n};
   static constexpr roxe::name     tokenissuer_account{"tokenissuer1"_n};
   static constexpr roxe::name     maintainer_account{"maintainer11"_n};
   static constexpr roxe::name     oracle_account{"eosdosoracle"_n};
   static constexpr extended_symbol weth_symbol = {symbol(symbol_code("WETH"), 4), "eosdosxtoken"_n};

   eosdos(name s, name code, roxe::datastream<const char*> ds)
       : contract(s, code, ds)
       , _instance_mgmt(s)
       , zoo(s, _instance_mgmt)
       , proxy(s, _instance_mgmt, zoo) {
      zoo.init(_self, _self, _self);
   }

   //////////////////zoo////////////////////////
   [[roxe::action]] void newdodo(
       name msg_sender, name dodo_name, address owner, address supervisor, address maintainer,
       const extended_symbol& baseToken, const extended_symbol& quoteToken, const extended_symbol& oracle,
       uint256 lpFeeRate, uint256 mtFeeRate, uint256 k, uint256 gasPriceLimit) {
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.newDODO(
          msg_sender, dodo_name, owner, supervisor, maintainer, baseToken, quoteToken, oracle, lpFeeRate, mtFeeRate, k,
          gasPriceLimit);
   }

   [[roxe::action]] void adddodo(name msg_sender, address _dodo) {
      proxy.setMsgSender(msg_sender);
      zoo.addDODO(_dodo);
   }

   [[roxe::action]] void removedodo(name msg_sender, address _dodo) {
      proxy.setMsgSender(msg_sender);
      zoo.removeDODO(_dodo);
   }

   [[roxe::action]] void breeddodo(
       name msg_sender, name dodo_name, address maintainer, const extended_symbol& baseToken,
       const extended_symbol& quoteToken, const extended_symbol& oracle, uint256 lpFeeRate, uint256 mtFeeRate,
       uint256 k, uint256 gasPriceLimit) {
      proxy.setMsgSender(msg_sender);
      zoo.breedDODO(dodo_name, maintainer, baseToken, quoteToken, oracle, lpFeeRate, mtFeeRate, k, gasPriceLimit);
   }

   //////////////////proxy////////////////////////

   [[roxe::action]] void init(
       name msg_sender, address dodoZoo, const extended_symbol& weth, const extended_symbol& core_symbol) {
      check(admin_account == msg_sender, "no  admin");
      proxy.setMsgSender(msg_sender);
      proxy.init(dodoZoo, weth, core_symbol);
   }

   [[roxe::action]] void sellethtoken(
       name msg_sender, const extended_asset& ethToken, const extended_asset& minReceiveToken) {
      proxy.setMsgSender(msg_sender);
      proxy.sellEthToToken(ethToken, minReceiveToken);
   }
   /////////////////////////////////////////
   [[roxe::action]] void buyethtoken(
       name msg_sender, const extended_asset& ethToken, const extended_asset& maxPayTokenAmount) {
      proxy.setMsgSender(msg_sender);
      //   proxy.buyEthWithToken(ethToken, maxPayTokenAmount);
      buyethtokenx_action buyethtokenx_act{admin_account, {{_self, "active"_n}}};
      buyethtokenx_act.send(msg_sender, ethToken, maxPayTokenAmount, true);
      buyethtokenx_act.send(msg_sender, ethToken, maxPayTokenAmount, false);
   }

   [[roxe::action]] void buyethtokenx(
       name msg_sender, const extended_asset& ethToken, const extended_asset& maxPayTokenAmount, bool pretransfer) {
      require_auth(_self);
      proxy.setMsgSender(msg_sender, false);
      proxy.buyEthWithToken(ethToken, maxPayTokenAmount, pretransfer);
   }

   using buyethtokenx_action = roxe::action_wrapper<"buyethtokenx"_n, &eosdos::buyethtokenx>;

   /////////////////////buy&sell token ////////////////////////
   [[roxe::action]] void selltokeneth(
       name msg_sender, const extended_asset& baseToken, const extended_asset& minReceiveEth) {
      proxy.setMsgSender(msg_sender);
      //   proxy.sellTokenToEth(baseToken, minReceiveEth);
      selltokenetha_action selltokenetha_act{admin_account, {{_self, "active"_n}}};
      selltokenetha_act.send(msg_sender, baseToken, minReceiveEth, 0, ACTION_STEP::STEP_ONE);
      selltokenetha_act.send(msg_sender, baseToken, minReceiveEth, 0, ACTION_STEP::STEP_TWO);
   }

   [[roxe::action]] void selltokenetha(
       name msg_sender, const extended_asset& baseToken, const extended_asset& minReceiveEth, uint256 receiveEthAmount,
       uint8_t state) {
      require_auth(_self);
      proxy.setMsgSender(msg_sender, false);
      uint256 receiveEthAmounts = proxy.sellTokenToEth(baseToken, minReceiveEth, receiveEthAmount, state);
      if (ACTION_STEP::STEP_TWO == state) {
         selltokenetha_action selltokenetha_act{admin_account, {{_self, "active"_n}}};
         selltokenetha_act.send(msg_sender, baseToken, minReceiveEth, receiveEthAmounts, ACTION_STEP::STEP_THREE);
      }
   }

   using selltokenetha_action = roxe::action_wrapper<"selltokenetha"_n, &eosdos::selltokenetha>;

   [[roxe::action]] void buytokeneth(
       name msg_sender, const extended_asset& baseToken, const extended_asset& maxPayEthAmount) {
      proxy.setMsgSender(msg_sender);
      //   proxy.buyTokenWithEth(baseToken, maxPayEthAmount);
      buytokenethx_action buytokenethx_act{admin_account, {{_self, "active"_n}}};
      buytokenethx_act.send(msg_sender, baseToken, maxPayEthAmount, ACTION_STEP::STEP_ONE);
      buytokenethx_act.send(msg_sender, baseToken, maxPayEthAmount, ACTION_STEP::STEP_TWO);
      buytokenethx_act.send(msg_sender, baseToken, maxPayEthAmount, ACTION_STEP::STEP_THREE);
   }

   [[roxe::action]] void buytokenethx(
       name msg_sender, const extended_asset& baseToken, const extended_asset& maxPayEthAmount, uint8_t state) {
      require_auth(_self);
      proxy.setMsgSender(msg_sender, false);
      proxy.buyTokenWithEth(baseToken, maxPayEthAmount, state);
   }

   using buytokenethx_action = roxe::action_wrapper<"buytokenethx"_n, &eosdos::buytokenethx>;

   /////////////////////////////////////////////////////////////
   // _lock_  Bind does not lock because it jumps to `rebind`, which does

   [[roxe::action]] void depositethab(
       name msg_sender, const extended_asset& ethtokenamount, const extended_symbol& quoteToken) {
      proxy.setMsgSender(msg_sender);
      proxy.depositEthAsBase(ethtokenamount, quoteToken);
   }

   [[roxe::action]] void withdraweab(
       name msg_sender, const extended_asset& ethtokenamount, const extended_symbol& quoteToken) {
      proxy.setMsgSender(msg_sender);
      //   proxy.withdrawEthAsBase(ethtokenamount, quoteToken);
      withdraweabx_action withdraweabx_act{admin_account, {{_self, "active"_n}}};
      withdraweabx_act.send(msg_sender, ethtokenamount, quoteToken, ACTION_STEP::STEP_ONE);
      withdraweabx_act.send(msg_sender, ethtokenamount, quoteToken, ACTION_STEP::STEP_TWO);
      withdraweabx_act.send(msg_sender, ethtokenamount, quoteToken, ACTION_STEP::STEP_THREE);
   }

   [[roxe::action]] void withdraweabx(
       name msg_sender, const extended_asset& ethtokenamount, const extended_symbol& quoteToken, uint8_t state) {
      require_auth(_self);
      proxy.setMsgSender(msg_sender, false);
      proxy.withdrawEthAsBase(ethtokenamount, quoteToken, state);
   }

   using withdraweabx_action = roxe::action_wrapper<"withdraweabx"_n, &eosdos::withdraweabx>;

   ////////////////////////////////////////////////////
   [[roxe::action]] void withdrawaeab(name msg_sender, const extended_symbol& quoteToken) {
      proxy.setMsgSender(msg_sender);
      //   proxy.withdrawAllEthAsBase(quoteToken);
      withdrawaebx_action withdrawaebx_act{admin_account, {{_self, "active"_n}}};
      withdrawaebx_act.send(msg_sender, quoteToken, ACTION_STEP::STEP_ONE);
      withdrawaebx_act.send(msg_sender, quoteToken, ACTION_STEP::STEP_TWO);
      withdrawaebx_act.send(msg_sender, quoteToken, ACTION_STEP::STEP_THREE);
   }

   [[roxe::action]] void withdrawaebx(name msg_sender, const extended_symbol& quoteToken, uint8_t state) {
      require_auth(_self);
      proxy.setMsgSender(msg_sender, false);
      proxy.withdrawAllEthAsBase(quoteToken, state);
   }

   using withdrawaebx_action = roxe::action_wrapper<"withdrawaebx"_n, &eosdos::withdrawaebx>;

   ////////////////////////////////////////////////////
   // Absorb any _token_ that have been sent to this contract into the pool
   [[roxe::action]] void depositethaq(
       name msg_sender, const extended_asset& ethtokenamount, const extended_symbol& baseToken) {
      proxy.setMsgSender(msg_sender);
      proxy.depositEthAsQuote(ethtokenamount, baseToken);
   }
   ////////////////////////////////////////////////////
   [[roxe::action]] void withdraweaq(
       name msg_sender, const extended_asset& ethtokenamount, const extended_symbol& baseToken) {
      proxy.setMsgSender(msg_sender);
      //   proxy.withdrawEthAsQuote(ethtokenamount, baseToken);
      withdraweaqx_action withdraweaqx_act{admin_account, {{_self, "active"_n}}};
      withdraweaqx_act.send(msg_sender, ethtokenamount, baseToken, ACTION_STEP::STEP_ONE);
      withdraweaqx_act.send(msg_sender, ethtokenamount, baseToken, ACTION_STEP::STEP_TWO);
      withdraweaqx_act.send(msg_sender, ethtokenamount, baseToken, ACTION_STEP::STEP_THREE);
   }

   [[roxe::action]] void withdraweaqx(
       name msg_sender, const extended_asset& ethtokenamount, const extended_symbol& baseToken, uint8_t state) {
      require_auth(_self);
      proxy.setMsgSender(msg_sender, false);
      proxy.withdrawEthAsQuote(ethtokenamount, baseToken, state);
   }

   using withdraweaqx_action = roxe::action_wrapper<"withdraweaqx"_n, &eosdos::withdraweaqx>;

   ////////////////////////////////////////////////////
   [[roxe::action]] void withdrawaeaq(name msg_sender, const extended_symbol& baseToken) {
      proxy.setMsgSender(msg_sender);
      //   proxy.withdrawAllEthAsQuote(baseToken);
      withdrawaeqx_action withdrawaeqx_act{admin_account, {{_self, "active"_n}}};
      withdrawaeqx_act.send(msg_sender, baseToken, ACTION_STEP::STEP_ONE);
      withdrawaeqx_act.send(msg_sender, baseToken, ACTION_STEP::STEP_TWO);
      withdrawaeqx_act.send(msg_sender, baseToken, ACTION_STEP::STEP_THREE);
   }

   [[roxe::action]] void withdrawaeqx(name msg_sender, const extended_symbol& baseToken, uint8_t state) {
      require_auth(_self);
      proxy.setMsgSender(msg_sender, false);
      proxy.withdrawAllEthAsQuote(baseToken, state);
   }

   using withdrawaeqx_action = roxe::action_wrapper<"withdrawaeqx"_n, &eosdos::withdrawaeqx>;

   ////////////////////  admin dodo////////////////////////
   [[roxe::action]] void enabletradin(name msg_sender, name dodo_name) {
      check(admin_account == msg_sender, "no  admin");
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.get_dodo(msg_sender, dodo_name, [&](auto& dodo) { dodo.enableTrading(); });
   }

   [[roxe::action]] void enablequodep(name msg_sender, name dodo_name) {
      check(admin_account == msg_sender, "no  admin");
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.get_dodo(msg_sender, dodo_name, [&](auto& dodo) { dodo.enableQuoteDeposit(); });
   }

   [[roxe::action]] void enablebasdep(name msg_sender, name dodo_name) {
      check(admin_account == msg_sender, "no  admin");
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.get_dodo(msg_sender, dodo_name, [&](auto& dodo) { dodo.enableBaseDeposit(); });
   }

   ////////////////////  LiquidityProvider dodo////////////////////////
   [[roxe::action]] void depositquote(name msg_sender, name dodo_name, const extended_asset& amt) {
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.get_dodo(msg_sender, dodo_name, [&](auto& dodo) { (void)dodo.depositQuote(amt.quantity.amount); });
   }
   [[roxe::action]] void depositbase(name msg_sender, name dodo_name, const extended_asset& amt) {
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.get_dodo(msg_sender, dodo_name, [&](auto& dodo) { (void)dodo.depositBase(amt.quantity.amount); });
   }
   ////////////////////   Oracle////////////////////////
   [[roxe::action]] void neworacle(name msg_sender, const extended_symbol& token) {
      check(oracle_account == msg_sender, "no oracle admin");
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.newOracle(msg_sender, token);
   }

   [[roxe::action]] void setprice(name msg_sender, const extended_asset& amt) {
      check(oracle_account == msg_sender, "no oracle admin");
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.get_oracle(msg_sender, amt.get_extended_symbol(), [&](auto& oracle) { oracle.setPrice(amt); });
   }

   ////////////////////  TOKEN////////////////////////
   [[roxe::action]] void extransfer(name from, name to, extended_asset quantity, std::string memo) {
      transfer_mgmt::static_transfer(from, to, quantity, memo);
   }

   [[roxe::action]] void newtoken(name msg_sender, const extended_asset& token) {
      check(tokenissuer_account == msg_sender, "no  token issuer");
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.newToken<TestERC20>(msg_sender, token);
   }

   [[roxe::action]] void newethtoken(name msg_sender, const extended_asset& token) {
      check(tokenissuer_account == msg_sender, "no  token issuer");
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.newToken<WETH9>(msg_sender, token);
   }

   //    /////test interface /////
   [[roxe::action]] void mint(name msg_sender, const extended_asset& amt) {
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.get_token<TestERC20>(
          msg_sender, amt.get_extended_symbol(), [&](auto& _token_) { _token_.mint(msg_sender, amt.quantity.amount); });
   }

   [[roxe::action]] void mintweth(name msg_sender, const extended_asset& amt) {
      proxy.setMsgSender(msg_sender);
      _instance_mgmt.get_token<WETH9>(
          msg_sender, amt.get_extended_symbol(), [&](auto& _token_) { _token_.mint(msg_sender, amt.quantity.amount); });
   }

   ////////////////////on_notify////////////////////
   [[roxe::on_notify("roxe.token::transfer")]] void on_transfer(
       name from, name to, asset quantity, std::string memo) {
      check(get_first_receiver() == "roxe.token"_n, "should be roxe.token");
      my_print_f("On notify : % % % %", from, to, quantity, memo);
      _instance_mgmt.get_transfer_mgmt().eosiotoken_transfer(from, to, quantity, memo, [&](const auto& action_event) {
         if (action_event.action.empty()) {
            return;
         }

         if (action_event.action.compare(bind_action_string) == 0) {
            auto           paras = transfer_mgmt::parse_string(action_event.param);
            name           pool_name;
            extended_asset balance;
            uint256        denorm;
            proxy.setMsgSender(action_event.msg_sender);
            // proxy.pool(pool_name, [&](auto& pool) { pool.bind(balance, denorm); });
         }
      });
   }

   [[roxe::on_notify("*::transfer")]] void on_transfer_by_non(name from, name to, asset quantity, std::string memo) {
      check(get_first_receiver() != "roxe.token"_n, "should not be roxe.token");
      my_print_f("On notify ACTION_STEP::STEP_TWO :% % % % %", get_first_receiver(), from, to, quantity, memo);
      _instance_mgmt.get_transfer_mgmt().non_eosiotoken_transfer(
          from, to, quantity, memo, [&](const auto& action_event) {

          });
   }
};
