#include "roxe.system_tester.hpp"
#include <boost/test/unit_test.hpp>
#include <roxe/chain/abi_serializer.hpp>
#include <roxe/testing/tester.hpp>

#include "Runtime/Runtime.h"

#include <algorithm>
#include <boost/multiprecision/cpp_int.hpp>
#include <fc/variant_object.hpp>
#include <sstream>
using namespace roxe::testing;
using namespace roxe;
using namespace roxe::chain;
using namespace roxe::testing;
using namespace fc;
using namespace std;

using mvo      = fc::mutable_variant_object;
using uint_eth = uint64_t;
using namesym  = roxe::chain::uint128_t;
class findx {
 public:
   findx(const string str) { test = str; }
   bool operator()(const fc::variant& v) {
      if (v["key"].as_string().compare(test) == 0) {
         return true;
      }
      return false;
   }

 private:
   string test;
};

class eoswap_tester : public tester {
 public:
   eoswap_tester() {
      produce_blocks(2);

      create_accounts({N(roxe.token), N(roxe.ram), N(roxe.ramfee), N(roxe.stake), N(roxe.bpay), N(roxe.vpay),
                       N(roxe.saving), N(roxe.names), N(roxe.rex)});

      create_accounts({N(alice), N(bob), N(carol), N(eoswapeoswap), N(pool)});
      create_accounts({N(weth), N(dai), N(mkr), N(xxx), N(eoswapxtoken)});
      produce_blocks(2);
      admin    = N(eoswapeoswap);
      nonadmin = N(alice);
      user1    = N(bob);
      user2    = N(carol);
      set_code(N(eoswapeoswap), contracts::swap_wasm());
      set_abi(N(eoswapeoswap), contracts::swap_abi().data());
      produce_blocks(2);

      set_code(N(roxe.token), contracts::token_wasm());
      set_abi(N(roxe.token), contracts::token_abi().data());

      create_currency(N(roxe.token), config::system_account_name, core_sym::from_string("10000000000.0000"));
      issue(config::system_account_name, core_sym::from_string("1000000000.0000"));
      BOOST_REQUIRE_EQUAL(
          core_sym::from_string("1000000000.0000"), get_balance(config::system_account_name) +
                                                        get_balance(N(roxe.ramfee)) + get_balance(N(roxe.stake)) +
                                                        get_balance(N(roxe.ram)));

      create_currency(
          N(roxe.token), config::system_account_name, roxe::chain::asset::from_string("10000000000.0000 EOS"));

      issue(config::system_account_name, roxe::chain::asset::from_string("1000000000.0000 EOS"));

      set_code(config::system_account_name, contracts::system_wasm());
      set_abi(config::system_account_name, contracts::system_abi().data());
      produce_blocks();
      base_tester::push_action(
          config::system_account_name, N(init), config::system_account_name,
          mutable_variant_object()("version", 0)("core", CORE_SYM_STR));
      produce_blocks();

      create_account_with_resources(N(alice1111111), N(roxe), core_sym::from_string("1.0000"), false);
      create_account_with_resources(N(bob111111111), N(roxe), core_sym::from_string("0.4500"), false);
      create_account_with_resources(N(carol1111111), N(roxe), core_sym::from_string("1.0000"), false);
      create_account_with_resources(N(david1111111), N(roxe), core_sym::from_string("1.0000"), false);

      set_code(N(eoswapxtoken), contracts::token_wasm());
      set_abi(N(eoswapxtoken), contracts::token_abi().data());

      std::vector<string> accounts  = {"alice1111111", "bob111111111", "carol1111111", "david1111111"};
      std::vector<name>   taccounts = {N(alice), N(bob), N(eoswapeoswap)};
      std::vector<string> tokens    = {"WETH", "DAI", "MKR", "XXX"};
      int                 j         = 0;
      std::string         amt       = "100000.0000 ";
      std::string         memo      = "";
      //   for (auto& acc_name : accounts) {
      //      std::string token     = tokens[j++];
      //      std::string amount    = "10000000000.0000 " + token;
      //      std::string tamount   = amt + token;
      //      asset       maxsupply = roxe::chain::asset::from_string(amount.c_str());
      //      name        acc       = name(acc_name.c_str());

      //      create_currency(N(eoswapxtoken), acc, maxsupply);
      //      issuex(N(eoswapxtoken), acc, maxsupply, acc);
      //      produce_blocks(1);
      //      for (auto& to : taccounts) {
      //         transferex(N(eoswapxtoken), acc, to, tamount, acc, memo);
      //      }
      //   }

      is_auth_token = false;
      produce_blocks();

      const auto& accnt = control->db().get<account_object, by_name>(N(eoswapeoswap));
      abi_def     abi;
      BOOST_REQUIRE_EQUAL(abi_serializer::to_abi(accnt.abi, abi), true);
      abi_ser.set_abi(abi, abi_serializer_max_time);
   }

   auto push_permission_update_auth_action(const account_name& signer) {
      auto auth = authority(roxe::testing::base_tester::get_public_key(signer, "active"));
      auth.accounts.push_back(permission_level_weight{{N(eoswapeoswap), config::eosio_code_name}, 1});

      return base_tester::push_action(
          N(roxe), N(updateauth), signer,
          mvo()("account", signer)("permission", "active")("parent", "owner")("auth", auth));
   }

   action_result push_action(const account_name& signer, const action_name& name, const variant_object& data) {
      if (!is_auth_token) {
         is_auth_token = true;
         push_permission_update_auth_action(N(eoswapxtoken));
         push_permission_update_auth_action(nonadmin);
         push_permission_update_auth_action(user1);
         push_permission_update_auth_action(N(alice1111111));
      }

      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = N(eoswapeoswap);
      act.name    = name;
      act.data    = abi_ser.variant_to_binary(action_type_name, data, abi_serializer_max_time);

      return base_tester::push_action(std::move(act), signer.to_uint64_t());
   }

   transaction_trace_ptr create_account_with_resources(
       account_name a, account_name creator, asset ramfunds, bool multisig,
       asset net = core_sym::from_string("10.0000"), asset cpu = core_sym::from_string("10.0000")) {
      signed_transaction trx;
      set_transaction_headers(trx);

      authority owner_auth;
      if (multisig) {
         // multisig between account's owner key and creators active permission
         owner_auth = authority(
             2, {key_weight{get_public_key(a, "owner"), 1}},
             {permission_level_weight{{creator, config::active_name}, 1}});
      } else {
         owner_auth = authority(get_public_key(a, "owner"));
      }

      trx.actions.emplace_back(
          vector<permission_level>{{creator, config::active_name}},
          newaccount{
              .creator = creator, .name = a, .owner = owner_auth, .active = authority(get_public_key(a, "active"))});

      trx.actions.emplace_back(get_action(
          N(roxe), N(buyram), vector<permission_level>{{creator, config::active_name}},
          mvo()("payer", creator)("receiver", a)("quant", ramfunds)));

      trx.actions.emplace_back(get_action(
          N(roxe), N(delegatebw), vector<permission_level>{{creator, config::active_name}},
          mvo()("from", creator)("receiver", a)("stake_net_quantity", net)("stake_cpu_quantity", cpu)("transfer", 0)));

      set_transaction_headers(trx);
      trx.sign(get_private_key(creator, "active"), control->get_chain_id());
      return push_transaction(trx);
   }
   void create_currency(name contract, name manager, asset maxsupply) {
      auto act = mutable_variant_object()("issuer", manager)("maximum_supply", maxsupply);

      base_tester::push_action(contract, N(create), contract, act);
   }
   void issuex(name contract, name to, const asset& amount, name manager = config::system_account_name) {
      base_tester::push_action(
          contract, N(issue), manager, mutable_variant_object()("to", to)("quantity", amount)("memo", ""));
   }

   void issue(name to, const asset& amount, name manager = config::system_account_name) {
      base_tester::push_action(
          N(roxe.token), N(issue), manager, mutable_variant_object()("to", to)("quantity", amount)("memo", ""));
   }
   void transfer(
       name from, name to, const string& amount, name manager = config::system_account_name,
       const std::string& memo = "") {
      base_tester::push_action(
          N(roxe.token), N(transfer), manager,
          mutable_variant_object()("from", from)("to", to)("quantity", core_sym::from_string(amount))("memo", memo));
   }

   void transferex(
       name contract, name from, name to, const string& amount, name manager = config::system_account_name,
       const std::string& memo = "") {
      base_tester::push_action(
          contract, N(transfer), manager,
          mutable_variant_object()("from", from)("to", to)("quantity", roxe::chain::asset::from_string(amount))(
              "memo", memo));
   }

   asset get_balance(const account_name& act) {
      // return get_currency_balance( config::system_account_name,
      // symbol(CORE_SYMBOL), act ); temporary code. current get_currency_balancy
      // uses table name N(accounts) from currency.h generic_currency table name
      // is N(account).
      const auto& db = control->db();
      const auto* tbl =
          db.find<table_id_object, by_code_scope_table>(boost::make_tuple(N(roxe.token), act, N(accounts)));
      share_type result = 0;

      // the balance is implied to be 0 if either the table or row does not exist
      if (tbl) {
         const auto* obj =
             db.find<key_value_object, by_scope_primary>(boost::make_tuple(tbl->id, symbol(CORE_SYM).to_symbol_code()));
         if (obj) {
            // balance is the first field in the serialization
            fc::datastream<const char*> ds(obj->value.data(), obj->value.size());
            fc::raw::unpack(ds, result);
         }
      }
      return asset(result, symbol(CORE_SYM));
   }

   asset get_balancex(const account_name& act, const symbol& sym, name contract = N(eoswapxtoken)) {
      // return get_currency_balance( config::system_account_name,
      // symbol(CORE_SYMBOL), act ); temporary code. current get_currency_balancy
      // uses table name N(accounts) from currency.h generic_currency table name
      // is N(account).
      const auto& db     = control->db();
      const auto* tbl    = db.find<table_id_object, by_code_scope_table>(boost::make_tuple(contract, act, N(accounts)));
      share_type  result = 0;

      // the balance is implied to be 0 if either the table or row does not exist
      if (tbl) {
         const auto* obj =
             db.find<key_value_object, by_scope_primary>(boost::make_tuple(tbl->id, sym.to_symbol_code()));
         if (obj) {
            // balance is the first field in the serialization
            fc::datastream<const char*> ds(obj->value.data(), obj->value.size());
            fc::raw::unpack(ds, result);
         }
      }
      return asset(result, sym);
   }

   /////////////factory///////////
   action_result setblabs(account_name msg_sender, account_name blabs) {
      return push_action(msg_sender, N(setblabs), mvo()("msg_sender", msg_sender)("blabs", blabs));
   }

   action_result collect(account_name msg_sender, account_name pool_name) {
      return push_action(msg_sender, N(collect), mvo()("msg_sender", msg_sender)("pool_name", pool_name));
   }

   action_result newpool(account_name msg_sender, account_name pool_name) {
      return push_action(msg_sender, N(newpool), mvo()("msg_sender", msg_sender)("pool_name", pool_name));
   }

   ///////////////pool ////////////////////
   // msg_sender, name dst, uint_eth amt
   action_result setswapfee(account_name msg_sender, account_name pool_name, uint_eth swapFee) {
      return push_action(
          msg_sender, N(setswapfee), mvo()("msg_sender", msg_sender)("pool_name", pool_name)("swapFee", swapFee));
   }

   action_result setcontroler(account_name msg_sender, account_name pool_name, name manager) {
      return push_action(
          msg_sender, N(setcontroler), mvo()("msg_sender", msg_sender)("pool_name", pool_name)("manager", manager));
   }

   action_result setpubswap(account_name msg_sender, account_name pool_name, bool public_) {
      return push_action(
          msg_sender, N(setpubswap), mvo()("msg_sender", msg_sender)("pool_name", pool_name)("public_", public_));
   }

   action_result finalize(account_name msg_sender, account_name pool_name) {
      return push_action(msg_sender, N(finalize), mvo()("msg_sender", msg_sender)("pool_name", pool_name));
   }

   action_result bind(account_name msg_sender, account_name pool_name, const extended_asset& balance, uint_eth denorm) {
      return push_action(
          msg_sender, N(bind),
          mvo()("msg_sender", msg_sender)("pool_name", pool_name)("balance", balance)("denorm", denorm));
   }

   action_result
   rebind(account_name msg_sender, account_name pool_name, const extended_asset& balance, uint_eth denorm) {
      return push_action(
          msg_sender, N(rebind),
          mvo()("msg_sender", msg_sender)("pool_name", pool_name)("balance", balance)("denorm", denorm));
   }

   action_result unbind(account_name msg_sender, account_name pool_name, const extended_symbol& token) {
      return push_action(
          msg_sender, N(unbind), mvo()("msg_sender", msg_sender)("pool_name", pool_name)("token", token));
   }

   action_result gulp(account_name msg_sender, account_name pool_name, const extended_symbol& token) {
      return push_action(msg_sender, N(gulp), mvo()("msg_sender", msg_sender)("pool_name", pool_name)("token", token));
   }

   action_result joinpool(
       account_name msg_sender, account_name pool_name, uint_eth poolAmountOut, std::vector<uint_eth> maxAmountsIn) {
      return push_action(
          msg_sender, N(joinpool),
          mvo()("msg_sender", msg_sender)("pool_name", pool_name)("poolAmountOut", poolAmountOut)(
              "maxAmountsIn", maxAmountsIn));
   }

   action_result exitpool(
       account_name msg_sender, account_name pool_name, uint_eth poolAmountIn, std::vector<uint_eth> minAmountsOut) {
      return push_action(
          msg_sender, N(exitpool),
          mvo()("msg_sender", msg_sender)("pool_name", pool_name)("poolAmountIn", poolAmountIn)(
              "minAmountsOut", minAmountsOut));
   }

   action_result swapamtin(
       name msg_sender, name pool_name, const extended_asset& tokenAmountIn, const extended_asset& minAmountOut,
       uint maxPrice) {
      return push_action(
          msg_sender, N(swapamtin),
          mvo()("msg_sender", msg_sender)("pool_name", pool_name)("tokenAmountIn", tokenAmountIn)(
              "minAmountOut", minAmountOut)("maxPrice", maxPrice));
   }

   action_result swapamtout(
       name msg_sender, name pool_name, const extended_asset& maxAmountIn, const extended_asset& tokenAmountOut,
       uint maxPrice) {
      return push_action(
          msg_sender, N(swapamtout),
          mvo()("msg_sender", msg_sender)("pool_name", pool_name)("maxAmountIn", maxAmountIn)(
              "tokenAmountOut", tokenAmountOut)("maxPrice", maxPrice));
   }

   ////////////////TOKEN//////////////
   action_result extransfer(name from, name to, const extended_asset& quantity, const std::string& memo = "") {
      return push_action(from, N(extransfer), mvo()("from", from)("to", to)("quantity", quantity)("memo", memo));
   }

   action_result newtoken(account_name msg_sender, const extended_asset& token) {
      return push_action(msg_sender, N(newtoken), mvo()("msg_sender", msg_sender)("token", token));
   }

   action_result approve(account_name msg_sender, account_name dst, const extended_asset& amt) {
      return push_action(msg_sender, N(approve), mvo()("msg_sender", msg_sender)("dst", dst)("amt", amt));
   }

   action_result transfer(account_name msg_sender, account_name dst, const extended_asset& amt) {
      return push_action(msg_sender, N(transfer), mvo()("msg_sender", msg_sender)("dst", dst)("amt", amt));
   }

   action_result transferfrom(account_name msg_sender, account_name src, account_name dst, const extended_asset& amt) {
      return push_action(
          msg_sender, N(transferfrom), mvo()("msg_sender", msg_sender)("src", src)("dst", dst)("amt", amt));
   }

   action_result incapproval(account_name msg_sender, account_name dst, const extended_asset& amt) {
      return push_action(msg_sender, N(incapproval), mvo()("msg_sender", msg_sender)("dst", dst)("amt", amt));
   }

   action_result decapproval(account_name msg_sender, account_name dst, const extended_asset& amt) {
      return push_action(msg_sender, N(decapproval), mvo()("msg_sender", msg_sender)("dst", dst)("amt", amt));
   }

   action_result mint(account_name msg_sender, const extended_asset& amt) {
      return push_action(msg_sender, N(mint), mvo()("msg_sender", msg_sender)("amt", amt));
   }

   action_result burn(account_name msg_sender, const extended_asset& amt) {
      return push_action(msg_sender, N(burn), mvo()("msg_sender", msg_sender)("amt", amt));
   }

   action_result move(account_name msg_sender, name dst, const extended_asset& amt) {
      return push_action(msg_sender, N(move), mvo()("msg_sender", msg_sender)("dst", dst)("amt", amt));
   }

   ////////////////get table//////////////

   fc::variant get_factory_store() {
      vector<char> data = get_row_by_account(N(eoswapeoswap), N(eoswapeoswap), N(factorystore), N(factorystore));
      if (data.empty())
         std::cout << "\nData is empty\n" << std::endl;
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant("BFactoryStorage", data, abi_serializer_max_time);
   }

   fc::variant get_pool_store() {
      vector<char> data = get_row_by_account(N(eoswapeoswap), N(eoswapeoswap), N(poolstore), N(poolstore));
      if (data.empty())
         std::cout << "\nData is empty\n" << std::endl;
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant("BPoolStorage", data, abi_serializer_max_time);
   }

   fc::variant get_pool_table(name pool_name) {
      name         table_name = N(pools);
      vector<char> data       = get_row_by_account(N(eoswapeoswap), N(eoswapeoswap), table_name, pool_name);
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant("pool_storage", data, abi_serializer_max_time);
   }

   fc::variant get_token_store() {
      vector<char> data = get_row_by_account(N(eoswapeoswap), N(eoswapeoswap), N(tokenstore), N(tokenstore));
      if (data.empty())
         std::cout << "\nData is empty\n" << std::endl;
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant("BTokenStorage", data, abi_serializer_max_time);
   }
   ///////////////utils///////////////////////////
   void to_kv_helper(const fc::variant& v, std::function<void(const std::string&, const std::string&)>&& append) {
      if (v.is_object()) {
         const auto&              obj = v.get_object();
         static const std::string sep = ".";

         for (const auto& entry : obj) {
            to_kv_helper(entry.value(), [&append, &entry](const std::string& path, const std::string& value) {
               append(sep + entry.key() + path, value);
            });
         }
      } else if (v.is_array()) {
         const auto& arr = v.get_array();
         for (size_t idx = 0; idx < arr.size(); idx++) {
            const auto& entry = arr.at(idx);
            to_kv_helper(entry, [&append, idx](const std::string& path, const std::string& value) {
               append(std::string("[") + std::to_string(idx) + std::string("]") + path, value);
            });
         }
      } else if (!v.is_null()) {
         append("", v.as_string());
      }
   }

   auto to_kv(const fc::variant& v) {
      std::map<std::string, std::string> result;
      to_kv_helper(v, [&result](const std::string& k, const std::string& v) { result.emplace(k, v); });
      return result;
   }

   std::string find_value(const fc::variant& v, const std::string& key) {
      auto a = v.get_array();
      auto i = std::find_if(a.begin(), a.end(), findx(key));
      if (i != a.end()) {
         fc::variant vv = *i;
         return vv["value"].as_string();
      }
      return std::string("");
   }

   fc::variant find_variant(const fc::variant& v, const std::string& key) {
      auto a = v.get_array();
      auto i = std::find_if(a.begin(), a.end(), findx(key));
      if (i != a.end()) {
         fc::variant vv = *i;
         return vv["value"];
      }
      return fc::variant();
   }

   bool isBPool(name pool_name) {
      const auto f = get_factory_store();
      const auto p = find_variant(f["isBPool"], pool_name.to_string());
      return p.as<bool>();
   }

   std::string balanceOf(const extended_symbol& token, name account) {
      const auto s = get_token_store();
      const auto t = find_variant(s["tokens"], ns_to_string(to_namesym(token)));
      const auto b = find_value(t["balance"], account.to_string());
      return b;
   }

   std::string allowance(const extended_symbol& token, name src, name dst) {
      const auto s   = get_token_store();
      const auto t   = find_variant(s["tokens"], ns_to_string(to_namesym(token)));
      const auto a   = find_variant(t["allowance"], src.to_string());
      const auto amt = find_value(a["dst2amt"], dst.to_string());
      return amt;
   }

   fc::variant records(name pool_name, const extended_symbol& token) {
      const auto ps = get_pool_store();
      const auto p  = find_variant(ps["pools"], pool_name.to_string());
      const auto r  = find_value(p["records"], ns_to_string(to_namesym(token)));
      return r;
   }

   fc::variant pools(name pool_name) {
      const auto ps = get_pool_store();
      const auto p  = find_variant(ps["pools"], pool_name.to_string());
      return p;
   }

   uint_eth to_wei(uint_eth value) { return value * pow(10, 6); }

   extended_asset to_pool_asset(name pool_name, int64_t value) {
      return extended_asset{asset{value, symbol{4, "BPT"}}, pool_name};
   }

   extended_symbol to_pool_sym(name pool_name) { return extended_symbol{symbol{4, "BPT"}, pool_name}; }

   extended_asset to_ext_asset(int64_t value, const std::string& sym) {
      return extended_asset{asset{value, symbol{4, sym.c_str()}}, name{"eoswapxtoken"}};
   }

   extended_asset to_asset(int64_t value, const std::string& sym) {
      return extended_asset{asset{value, symbol{4, sym.c_str()}}, name{"eoswapxtoken"}};
   }
   extended_asset to_wei_asset(uint_eth value, const std::string& sym) {
      return to_asset(static_cast<int64_t>(to_wei(value)), sym);
   }

   extended_symbol to_sym(const std::string& sym) {
      return extended_symbol{symbol{4, sym.c_str()}, name{"eoswapxtoken"}};
   }

   extended_asset to_maximum_supply(const std::string& sym) {
      return extended_asset{asset{1000000000000000, symbol{4, sym.c_str()}}, name{"eoswapxtoken"}};
   }

   namesym to_namesym(const extended_symbol& exsym) {
      namesym ns = exsym.contract.to_uint64_t();
      return ns << 64 | exsym.sym.value();
   }

   std::string boost_to_string(boost::multiprecision::uint128_t t) {
      using namespace boost::multiprecision;
      std::stringstream ss;
      ss << t;
      return ss.str();
   }

   std::string ns_to_string(namesym ns) {
      std::string s = boost_to_string(ns);
      return s;
   }

   void newpoolBefore() { newpool(admin, N(pool)); }

   void setswapfeeBefore() {
      // await pool.setSwapFee(toWei('0.003'));
      setswapfee(admin, N(pool), 3000);
   }

   void mintBefore() {
      newtoken(admin, to_maximum_supply("WETH"));
      newtoken(admin, to_maximum_supply("MKR"));
      newtoken(admin, to_maximum_supply("DAI"));
      newtoken(admin, to_maximum_supply("XXX"));

      mint(admin, to_wei_asset(50, "WETH"));
      mint(admin, to_wei_asset(20, "MKR"));
      mint(admin, to_wei_asset(10000, "DAI"));
      mint(admin, to_wei_asset(10, "XXX"));

      mint(user1, to_wei_asset(25, "WETH"));
      mint(user1, to_wei_asset(4, "MKR"));
      mint(user1, to_wei_asset(40000, "DAI"));
      mint(user1, to_wei_asset(10, "XXX"));

      mint(user2, to_asset(12222200, "WETH"));
      mint(user2, to_asset(1015333, "MKR"));
      mint(user2, to_asset(1, "DAI"));
      mint(user2, to_wei_asset(51, "XXX"));

      mint(nonadmin, to_wei_asset(1, "WETH"));
      mint(nonadmin, to_wei_asset(200, "DAI"));
   }

   void bindBefore() {
      bind(admin, N(pool), to_wei_asset(50, "WETH"), to_wei(5));
      bind(admin, N(pool), to_wei_asset(20, "MKR"), to_wei(5));
      bind(admin, N(pool), to_wei_asset(10000, "DAI"), to_wei(5));
   }

   void finalizeBefore() { finalize(admin, N(pool)); }

   void joinpoolBefore() {
      std::vector<uint_eth> v{uint_eth(-1), uint_eth(-1), uint_eth(-1)};
      joinpool(user1, N(pool), to_wei(5), v);
   }

   void before() {
      newpoolBefore();
      setswapfeeBefore();
      mintBefore();
      bindBefore();
      finalizeBefore();
      joinpoolBefore();
   }

   void mintBefore1() {

      newtoken(admin, to_maximum_supply("WETH"));
      newtoken(admin, to_maximum_supply("DAI"));

      mint(admin, to_wei_asset(5, "WETH"));
      mint(admin, to_wei_asset(200, "DAI"));

      mint(nonadmin, to_wei_asset(1, "WETH"));
      mint(nonadmin, to_wei_asset(200, "DAI"));
   }

   void bindBefore1() {
      bind(admin, N(pool), to_wei_asset(5, "WETH"), to_wei(5));
      bind(admin, N(pool), to_wei_asset(200, "DAI"), to_wei(5));
   }

   void joinpoolBefore1() {
      std::vector<uint_eth> v{uint_eth(-1), uint_eth(-1)};
      joinpool(nonadmin, N(pool), to_wei(10), v);
   }

   void exitpoolBefore1() { exitpool(nonadmin, N(pool), to_wei(10), std::vector<uint_eth>{0, 0}); }
   void before1() {
      newpoolBefore();
      mintBefore1();
      bindBefore1();
      finalizeBefore();
      joinpoolBefore1();
      exitpoolBefore1();
   }

   /////////////extended token///////////////////////
   /////////////extended token////////////////////////
   ////////////extended token///////////////////////

   std::vector<std::string> parse_string(const std::string& source, const std::string& delimiter = " ") {
      std::vector<std::string> results;
      // const std::string delimiter = ",";
      size_t prev = 0;
      size_t next = 0;

      while ((next = source.find_first_of(delimiter.c_str(), prev)) != std::string::npos) {
         if (next - prev != 0) {
            results.push_back(source.substr(prev, next - prev));
         }
         prev = next + 1;
      }

      if (prev < source.size()) {
         results.push_back(source.substr(prev));
      }

      return results;
   }

   uint64_t to_int(const std::string& str) {
      bool        isOK   = false;
      const char* nptr   = str.c_str();
      char*       endptr = NULL;
      errno              = 0;
      uint64_t val       = std::strtoull(nptr, &endptr, 10);
      // error ocur
      if ((errno == ERANGE && (val == ULLONG_MAX)) || (errno != 0 && val == 0)) {

      }
      // no digit find
      else if (endptr == nptr) {

      } else if (*endptr != '\0') {
         // printf("Further characters after number: %s\n", endptr);
      } else {
         isOK = true;
      }

      return val;
   }

   extended_asset ext_asset_from_string(const std::string& tokenstr) {
      std::vector<std::string> strvec = parse_string(tokenstr);
      const int                len    = strvec.size();
      BOOST_REQUIRE_EQUAL(len, 2);
      std::string str = strvec[0];
      std::string sym = strvec[1];
      str.erase(std::remove(str.begin(), str.end(), '.'), str.end());
      int64_t value = static_cast<int64_t>(to_int(str));
      return extended_asset{asset{value, symbol{4, sym.c_str()}}, name{"eoswapeoswap"}};
   }

   action_result push_extoken_action(const account_name& signer, const action_name& name, const variant_object& data) {
      string action_type_name = abi_ser.get_action_type(name);

      action act;
      act.account = N(extendxtoken);
      act.name    = name;
      act.data    = abi_ser.variant_to_binary(action_type_name, data, abi_serializer_max_time);

      return base_tester::push_action(std::move(act), signer.to_uint64_t());
   }

   fc::variant get_stats(const string& symbolname) {
      auto         symb        = roxe::chain::symbol::from_string(symbolname);
      auto         symbol_code = symb.to_symbol_code().value;
      vector<char> data = get_row_by_account(N(extendxtoken), name(symbol_code), N(stat), account_name(symbol_code));
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant("currency_stats", data, abi_serializer_max_time);
   }

   fc::variant get_account(account_name acc, const string& symbolname) {
      auto         symb        = roxe::chain::symbol::from_string(symbolname);
      auto         symbol_code = symb.to_symbol_code().value;
      vector<char> data        = get_row_by_account(N(extendxtoken), acc, N(accounts), account_name(symbol_code));
      return data.empty() ? fc::variant() : abi_ser.binary_to_variant("account", data, abi_serializer_max_time);
   }

   action_result create(account_name issuer, extended_asset maximum_supply) {

      return push_extoken_action(N(extendxtoken), N(create), mvo()("issuer", issuer)("maximum_supply", maximum_supply));
   }

   action_result issue(account_name issuer, extended_asset quantity, string memo) {
      return push_extoken_action(issuer, N(issue), mvo()("to", issuer)("quantity", quantity)("memo", memo));
   }

   action_result retire(account_name issuer, extended_asset quantity, string memo) {
      return push_extoken_action(issuer, N(retire), mvo()("quantity", quantity)("memo", memo));
   }

   action_result transfer(account_name from, account_name to, extended_asset quantity, string memo) {
      return push_extoken_action(from, N(transfer), mvo()("from", from)("to", to)("quantity", quantity)("memo", memo));
   }

   action_result open(account_name owner, const string& symbolname, account_name ram_payer) {
      return push_extoken_action(
          ram_payer, N(open), mvo()("owner", owner)("symbol", symbolname)("ram_payer", ram_payer));
   }

   action_result close(account_name owner, const string& symbolname) {
      return push_extoken_action(owner, N(close), mvo()("owner", owner)("symbol", "0,CERO"));
   }
   /////////////extended token////////////////////
   bool is_auth_token;

   name           admin;
   name           nonadmin;
   name           user1;
   name           user2;
   abi_serializer abi_ser;
};

BOOST_AUTO_TEST_SUITE(eoswap_tests)
////////////////factory////////////////////
BOOST_FIXTURE_TEST_CASE(newpool_tests, eoswap_tester) try { newpool(admin, N(pool)); }
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(setblabs_tests, eoswap_tester) try {
   setblabs(N(eoswapeoswap), N(alice));
   std::string b = get_factory_store()["blabs"].as_string();
   BOOST_REQUIRE_EQUAL("alice", b);
}
FC_LOG_AND_RETHROW()

////////////////pool////////////////////

BOOST_FIXTURE_TEST_CASE(bind_tests, eoswap_tester) try {

   newpoolBefore();
   mintBefore1();
   bind(admin, N(pool), to_wei_asset(5, "WETH"), to_wei(5));
   bind(admin, N(pool), to_wei_asset(200, "DAI"), to_wei(5));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(finalize_tests, eoswap_tester) try {

   newpoolBefore();
   mintBefore1();
   bindBefore1();

   finalize(admin, N(pool));
   bool flag = pools(N(pool))["finalized"].as<bool>();
   BOOST_REQUIRE_EQUAL(true, flag);
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(joinpool_tests, eoswap_tester) try {

   newpoolBefore();
   mintBefore1();
   bindBefore1();
   finalizeBefore();

   std::vector<uint_eth> v{uint_eth(-1), uint_eth(-1)};
   joinpool(nonadmin, N(pool), to_wei(10), v);

   auto pool = get_pool_store();
   BOOST_TEST_CHECK(nullptr == pool);
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(exitpool_tests, eoswap_tester) try {
   newpoolBefore();
   mintBefore1();
   bindBefore1();
   finalizeBefore();
   joinpoolBefore1();
   exitpool(nonadmin, N(pool), to_wei(10), std::vector<uint_eth>{0, 0});
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(collect_tests, eoswap_tester) try {

   before1();
   collect(admin, N(pool));
   const auto ab = balanceOf(to_pool_sym(N(pool)), admin);
   BOOST_REQUIRE_EQUAL(std::to_string(to_wei(100)), ab);
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(swapExactAmountIn_tests, eoswap_tester) try {
   before();
   auto pool = get_pool_table(N(pool));
   BOOST_TEST_CHECK(nullptr == pool);
   swapamtin(user1, N(pool), to_asset(2500000, "WETH"), to_wei_asset(475, "DAI"), to_wei(200));
   pool = get_pool_table(N(pool));
   BOOST_TEST_CHECK(nullptr == pool);
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(swapExactAmountOut_tests, eoswap_tester) try {
   before();
   auto pool = get_pool_store();
   BOOST_TEST_CHECK(nullptr == pool);
   swapamtout(user1, N(pool), to_wei_asset(3, "WETH"), to_wei_asset(1, "MKR"), to_wei(500));
   pool = get_pool_store();
   BOOST_TEST_CHECK(nullptr == pool);
}
FC_LOG_AND_RETHROW()

////////////////token////////////////////
BOOST_FIXTURE_TEST_CASE(mint_tests, eoswap_tester) try {
   newpool(admin, N(pool));
   newtoken(admin, to_maximum_supply("WETH"));
   newtoken(admin, to_maximum_supply("DAI"));
   mint(N(alice), to_wei_asset(5, "WETH"));
   mint(N(alice), to_wei_asset(200, "DAI"));

   const auto ab = balanceOf(to_sym("WETH"), N(alice));
   BOOST_REQUIRE_EQUAL(std::to_string(to_wei(5)), ab);
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(burn_tests, eoswap_tester) try {
   newtoken(admin, to_maximum_supply("POOL"));
   mint(admin, to_asset(300, "POOL"));
   burn(admin, to_asset(300, "POOL"));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(approve_tests, eoswap_tester) try {
   newpool(admin, N(pool));
   approve(N(alice), N(bob), to_pool_asset(N(pool), 300));
   const auto b = allowance(to_pool_sym(N(pool)), N(alice), N(bob));
   BOOST_REQUIRE_EQUAL("300", b);
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(swap_transfer_tests, eoswap_tester) try {
   newtoken(admin, to_maximum_supply("POOL"));
   mint(N(alice), to_asset(300, "POOL"));
   transfer(N(alice), N(bob), to_asset(300, "POOL"));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(extransfer_tests, eoswap_tester) try {
   const std::string     token_name = "POOL";
   const extended_asset& max_supply = to_asset(1, token_name);
   newtoken(admin, to_maximum_supply(token_name));
   mint(N(alice1111111), max_supply);
   const symbol& sym = max_supply.quantity.get_symbol();
   extransfer(N(alice1111111), user2, max_supply);

   BOOST_REQUIRE_EQUAL(roxe::chain::asset::from_string("0.0001 " + token_name), get_balancex(user2, sym));
}
FC_LOG_AND_RETHROW()

//////////////extened token///////////////////////////////
//////////////extened token///////////////////////////////
//////////////extened token///////////////////////////////
//////////////extened token///////////////////////////////
//////////////extened token///////////////////////////////

BOOST_FIXTURE_TEST_CASE(create_tests, eoswap_tester) try {

   auto token = create(N(alice), ext_asset_from_string("1000.000 TKN"));
   auto stats = get_stats("3,TKN");
   REQUIRE_MATCHING_OBJECT(stats, mvo()("supply", "0.000 TKN")("max_supply", "1000.000 TKN")("issuer", "alice"));
   produce_blocks(1);
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(create_negative_max_supply, eoswap_tester) try {

   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("max-supply must be positive"), create(N(alice), ext_asset_from_string("-1000.000 TKN")));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(symbol_already_exists, eoswap_tester) try {

   auto token = create(N(alice), ext_asset_from_string("100 TKN"));
   auto stats = get_stats("0,TKN");
   REQUIRE_MATCHING_OBJECT(stats, mvo()("supply", "0 TKN")("max_supply", "100 TKN")("issuer", "alice"));
   produce_blocks(1);

   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("token with symbol already exists"), create(N(alice), ext_asset_from_string("100 TKN")));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(create_max_supply, eoswap_tester) try {

   auto token = create(N(alice), ext_asset_from_string("4611686018427387903 TKN"));
   auto stats = get_stats("0,TKN");
   REQUIRE_MATCHING_OBJECT(stats, mvo()("supply", "0 TKN")("max_supply", "4611686018427387903 TKN")("issuer", "alice"));
   produce_blocks(1);

   extended_asset max =
       extended_asset{asset{10, symbol(SY(0, NKT))}, name{"eoswapeoswap"}}; // max(10, symbol(SY(0, NKT)));
   share_type amount = 4611686018427387904;
   static_assert(sizeof(share_type) <= sizeof(extended_asset), "extended_asset changed so test is no longer valid");
   static_assert(std::is_trivially_copyable<extended_asset>::value, "extended_asset is not trivially copyable");
   memcpy(&max, &amount, sizeof(share_type)); // hack in an invalid amount

   BOOST_CHECK_EXCEPTION(create(N(alice), max), asset_type_exception, [](const asset_type_exception& e) {
      return expect_assert_message(e, "magnitude of extended_asset amount must be less than 2^62");
   });
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(create_max_decimals, eoswap_tester) try {

   auto token = create(N(alice), ext_asset_from_string("1.000000000000000000 TKN"));
   auto stats = get_stats("18,TKN");
   REQUIRE_MATCHING_OBJECT(
       stats, mvo()("supply", "0.000000000000000000 TKN")("max_supply", "1.000000000000000000 TKN")("issuer", "alice"));
   produce_blocks(1);

   // extended_asset{asset{value, symbol{4, sym.c_str()}}, name{"eoswapeoswap"}}
   extended_asset max = extended_asset{asset{10, symbol(SY(0, NKT))}, name{"eoswapeoswap"}};
   // 1.0000000000000000000 => 0x8ac7230489e80000L
   share_type amount = 0x8ac7230489e80000L;
   static_assert(sizeof(share_type) <= sizeof(extended_asset), "extended_asset changed so test is no longer valid");
   static_assert(std::is_trivially_copyable<extended_asset>::value, "extended_asset is not trivially copyable");
   memcpy(&max, &amount, sizeof(share_type)); // hack in an invalid amount

   BOOST_CHECK_EXCEPTION(create(N(alice), max), asset_type_exception, [](const asset_type_exception& e) {
      return expect_assert_message(e, "magnitude of extended_asset amount must be less than 2^62");
   });
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(issue_tests, eoswap_tester) try {

   auto token = create(N(alice), ext_asset_from_string("1000.000 TKN"));
   produce_blocks(1);

   issue(N(alice), ext_asset_from_string("500.000 TKN"), "hola");

   auto stats = get_stats("3,TKN");
   REQUIRE_MATCHING_OBJECT(stats, mvo()("supply", "500.000 TKN")("max_supply", "1000.000 TKN")("issuer", "alice"));

   auto alice_balance = get_account(N(alice), "3,TKN");
   REQUIRE_MATCHING_OBJECT(alice_balance, mvo()("balance", "500.000 TKN"));

   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("quantity exceeds available supply"),
       issue(N(alice), ext_asset_from_string("500.001 TKN"), "hola"));

   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("must issue positive quantity"), issue(N(alice), ext_asset_from_string("-1.000 TKN"), "hola"));

   BOOST_REQUIRE_EQUAL(success(), issue(N(alice), ext_asset_from_string("1.000 TKN"), "hola"));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(retire_tests, eoswap_tester) try {

   auto token = create(N(alice), ext_asset_from_string("1000.000 TKN"));
   produce_blocks(1);

   BOOST_REQUIRE_EQUAL(success(), issue(N(alice), ext_asset_from_string("500.000 TKN"), "hola"));

   auto stats = get_stats("3,TKN");
   REQUIRE_MATCHING_OBJECT(stats, mvo()("supply", "500.000 TKN")("max_supply", "1000.000 TKN")("issuer", "alice"));

   auto alice_balance = get_account(N(alice), "3,TKN");
   REQUIRE_MATCHING_OBJECT(alice_balance, mvo()("balance", "500.000 TKN"));

   BOOST_REQUIRE_EQUAL(success(), retire(N(alice), ext_asset_from_string("200.000 TKN"), "hola"));
   stats = get_stats("3,TKN");
   REQUIRE_MATCHING_OBJECT(stats, mvo()("supply", "300.000 TKN")("max_supply", "1000.000 TKN")("issuer", "alice"));
   alice_balance = get_account(N(alice), "3,TKN");
   REQUIRE_MATCHING_OBJECT(alice_balance, mvo()("balance", "300.000 TKN"));

   // should fail to retire more than current supply
   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("overdrawn balance"), retire(N(alice), ext_asset_from_string("500.000 TKN"), "hola"));

   BOOST_REQUIRE_EQUAL(success(), transfer(N(alice), N(bob), ext_asset_from_string("200.000 TKN"), "hola"));
   // should fail to retire since tokens are not on the issuer's balance
   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("overdrawn balance"), retire(N(alice), ext_asset_from_string("300.000 TKN"), "hola"));
   // transfer tokens back
   BOOST_REQUIRE_EQUAL(success(), transfer(N(bob), N(alice), ext_asset_from_string("200.000 TKN"), "hola"));

   BOOST_REQUIRE_EQUAL(success(), retire(N(alice), ext_asset_from_string("300.000 TKN"), "hola"));
   stats = get_stats("3,TKN");
   REQUIRE_MATCHING_OBJECT(stats, mvo()("supply", "0.000 TKN")("max_supply", "1000.000 TKN")("issuer", "alice"));
   alice_balance = get_account(N(alice), "3,TKN");
   REQUIRE_MATCHING_OBJECT(alice_balance, mvo()("balance", "0.000 TKN"));

   // trying to retire tokens with zero supply
   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("overdrawn balance"), retire(N(alice), ext_asset_from_string("1.000 TKN"), "hola"));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(transfer_tests, eoswap_tester) try {

   auto token = create(N(alice), ext_asset_from_string("1000 CERO"));
   produce_blocks(1);

   issue(N(alice), ext_asset_from_string("1000 CERO"), "hola");

   auto stats = get_stats("0,CERO");
   REQUIRE_MATCHING_OBJECT(stats, mvo()("supply", "1000 CERO")("max_supply", "1000 CERO")("issuer", "alice"));

   auto alice_balance = get_account(N(alice), "0,CERO");
   REQUIRE_MATCHING_OBJECT(alice_balance, mvo()("balance", "1000 CERO"));

   transfer(N(alice), N(bob), ext_asset_from_string("300 CERO"), "hola");

   alice_balance = get_account(N(alice), "0,CERO");
   REQUIRE_MATCHING_OBJECT(alice_balance, mvo()("balance", "700 CERO")("frozen", 0)("whitelist", 1));

   auto bob_balance = get_account(N(bob), "0,CERO");
   REQUIRE_MATCHING_OBJECT(bob_balance, mvo()("balance", "300 CERO")("frozen", 0)("whitelist", 1));

   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("overdrawn balance"), transfer(N(alice), N(bob), ext_asset_from_string("701 CERO"), "hola"));

   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("must transfer positive quantity"),
       transfer(N(alice), N(bob), ext_asset_from_string("-1000 CERO"), "hola"));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(open_tests, eoswap_tester) try {

   auto token = create(N(alice), ext_asset_from_string("1000 CERO"));

   auto alice_balance = get_account(N(alice), "0,CERO");
   BOOST_REQUIRE_EQUAL(true, alice_balance.is_null());
   BOOST_REQUIRE_EQUAL(
       wasm_assert_msg("tokens can only be issued to issuer account"),
       push_extoken_action(
           N(alice), N(issue), mvo()("to", "bob")("quantity", ext_asset_from_string("1000 CERO"))("memo", "")));
   BOOST_REQUIRE_EQUAL(success(), issue(N(alice), ext_asset_from_string("1000 CERO"), "issue"));

   alice_balance = get_account(N(alice), "0,CERO");
   REQUIRE_MATCHING_OBJECT(alice_balance, mvo()("balance", "1000 CERO"));

   auto bob_balance = get_account(N(bob), "0,CERO");
   BOOST_REQUIRE_EQUAL(true, bob_balance.is_null());

   BOOST_REQUIRE_EQUAL(wasm_assert_msg("owner account does not exist"), open(N(nonexistent), "0,CERO", N(alice)));
   BOOST_REQUIRE_EQUAL(success(), open(N(bob), "0,CERO", N(alice)));

   bob_balance = get_account(N(bob), "0,CERO");
   REQUIRE_MATCHING_OBJECT(bob_balance, mvo()("balance", "0 CERO"));

   BOOST_REQUIRE_EQUAL(success(), transfer(N(alice), N(bob), ext_asset_from_string("200 CERO"), "hola"));

   bob_balance = get_account(N(bob), "0,CERO");
   REQUIRE_MATCHING_OBJECT(bob_balance, mvo()("balance", "200 CERO"));

   BOOST_REQUIRE_EQUAL(wasm_assert_msg("symbol does not exist"), open(N(carol), "0,INVALID", N(alice)));

   BOOST_REQUIRE_EQUAL(wasm_assert_msg("symbol precision mismatch"), open(N(carol), "1,CERO", N(alice)));
}
FC_LOG_AND_RETHROW()

BOOST_FIXTURE_TEST_CASE(close_tests, eoswap_tester) try {

   auto token = create(N(alice), ext_asset_from_string("1000 CERO"));

   auto alice_balance = get_account(N(alice), "0,CERO");
   BOOST_REQUIRE_EQUAL(true, alice_balance.is_null());

   BOOST_REQUIRE_EQUAL(success(), issue(N(alice), ext_asset_from_string("1000 CERO"), "hola"));

   alice_balance = get_account(N(alice), "0,CERO");
   REQUIRE_MATCHING_OBJECT(alice_balance, mvo()("balance", "1000 CERO"));

   BOOST_REQUIRE_EQUAL(success(), transfer(N(alice), N(bob), ext_asset_from_string("1000 CERO"), "hola"));

   alice_balance = get_account(N(alice), "0,CERO");
   REQUIRE_MATCHING_OBJECT(alice_balance, mvo()("balance", "0 CERO"));

   BOOST_REQUIRE_EQUAL(success(), close(N(alice), "0,CERO"));
   alice_balance = get_account(N(alice), "0,CERO");
   BOOST_REQUIRE_EQUAL(true, alice_balance.is_null());
}
FC_LOG_AND_RETHROW()

BOOST_AUTO_TEST_SUITE_END()
