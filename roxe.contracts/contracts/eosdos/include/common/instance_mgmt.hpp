// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the dodoied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
#pragma once

#include <common/storage_mgmt.hpp>
#include <common/transfer_mgmt.hpp>
#include <eosdos/dodo.hpp>
#include <eosdos/helper/TestERC20.hpp>
#include <eosdos/helper/TestWETH.hpp>
#include <eosdos/impl/DODOLpToken.hpp>
class instance_mgmt : public IFactory {
 private:
   name          self;
   storage_mgmt  _storage_mgmt;
   transfer_mgmt _transfer_mgmt;

 public:
   instance_mgmt(name _self)
       : self(_self)
       , _storage_mgmt(_self)
       , _transfer_mgmt(_self) {}
   ~instance_mgmt() {}
   name           get_self() override { return self; }
   storage_mgmt&  get_storage_mgmt() override { return _storage_mgmt; }
   transfer_mgmt& get_transfer_mgmt() override { return _transfer_mgmt; }
   instance_mgmt& get_instance_mgmt() override { return *this; }

   template <typename T>
   void get_dodo(name _msg_sender, name dodo_name, T func) {
      const DODOStore& dodoStore = _storage_mgmt.get_dodo(dodo_name);
      DODO             dodo(dodoStore, *this);
      dodo.setMsgSender(_msg_sender,false);
      func(dodo);
   }

   template <typename T>
   void get_lptoken(name _msg_sender, const extended_symbol& lptoken, T func) {
    //   TokenStore& lptokenStore  = _storage_mgmt.get_token_store(lptoken);
    //   TokenStore& olptokenStore = _storage_mgmt.get_token_store(lptokenStore.originToken);
      DODOLpToken token(lptoken, *this);
      token.setMsgSender(_msg_sender,false);
      func(token);
   }

   template <typename T, typename F>
   void get_token(name _msg_sender, const extended_symbol& _token, F func) {
      //   TokenStore& tokenStore = _storage_mgmt.get_token_store(_token);
      T token(_token);
      token.setMsgSender(_msg_sender,false);
      func(token);
   }

   void newDODO(
       name _msg_sender, name dodo_name, address owner, address supervisor, address maintainer,
       const extended_symbol& baseToken, const extended_symbol& quoteToken, const extended_symbol& oracle,
       uint64_t lpFeeRate, uint64_t mtFeeRate, uint64_t k, uint64_t gasPriceLimit) {
      const DODOStore& dodoStore = _storage_mgmt.newDodo(_msg_sender, dodo_name);
      DODO             dodo(dodoStore, *this);
      dodo.setMsgSender(_msg_sender,false);
      dodo.init(
          dodo_name, owner, supervisor, maintainer, baseToken, quoteToken, oracle, lpFeeRate, mtFeeRate, k,
          gasPriceLimit);
   }

   template <typename T>
   void newToken(name _msg_sender, const extended_asset& tokenx) {
      const extended_symbol& exsym = tokenx.get_extended_symbol();
      //   TokenStore&            tokenStore = _storage_mgmt.newTokenStore(exsym);
      T otoken(exsym);
      otoken.setMsgSender(_msg_sender,false);
      otoken.init(tokenx);
   }

   static const uint64_t MAX_TOTAL_SUPPLY = my_pow(10,15);

   extended_symbol       newLpToken(name _msg_sender, name dodo_name, const extended_symbol& tokenx) override {
      const symbol&   sym   = tokenx.get_symbol();
      extended_symbol exsym = extended_symbol(sym, dodo_name);

    //   TokenStore& tokenStore    = _storage_mgmt.newTokenStore(exsym);
    //   TokenStore& olptokenStore = _storage_mgmt.get_token_store(tokenx);
      DODOLpToken token(exsym, *this);
      token.setMsgSender(_msg_sender,false);
      token.init(extended_asset{MAX_TOTAL_SUPPLY, exsym}, tokenx);

      return exsym;
   }
};

template <typename T>
void IFactory::get_lptoken(name _msg_sender, const extended_symbol& lptoken, T func) {
   get_instance_mgmt().get_lptoken(_msg_sender, lptoken, func);
}

// template <typename T>
// void IFactory::get_oracle(name _msg_sender, const extended_symbol& oracle, T func) {
//    get_instance_mgmt().get_oracle(_msg_sender, oracle, func);
// }
