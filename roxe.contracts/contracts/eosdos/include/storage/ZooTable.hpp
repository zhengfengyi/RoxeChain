
#pragma once
#include <common/defines.hpp>
#include <storage/ImplTable.hpp>
#include <storage/LibTable.hpp>

struct DODOEthProxyStore {
   address         _DODO_ZOO_;
   extended_symbol _WETH_;
   extended_symbol core_symbol;
   ROXELIB_SERIALIZE(DODOEthProxyStore, (_DODO_ZOO_)(_WETH_)(core_symbol))
};

struct QuoteToken2Dodo {
   std::map<namesym, name> q2d;
   ROXELIB_SERIALIZE(QuoteToken2Dodo, (q2d))
};

struct [[roxe::table("dodo"), roxe::contract("eosdos")]] DODOStorage {
   std::map<name, DODOStore> dodos;
   ROXELIB_SERIALIZE(DODOStorage, (dodos))
};

typedef roxe::singleton<"dodo"_n, DODOStorage> DODOStorageSingleton;

struct [[roxe::table, roxe::contract("eosdos")]] dodo_storage {
   name      dodo;
   DODOStore dodos;
   uint64_t  primary_key() const { return dodo.value; }
};

typedef roxe::multi_index<"dodos"_n, dodo_storage> dodo_storage_table;

struct [[roxe::table("proxy"), roxe::contract("eosdos")]] ProxyStorage {
   DODOEthProxyStore    proxy;
   ReentrancyGuardStore guard;

   ROXELIB_SERIALIZE(ProxyStorage, (proxy)(guard))
};

typedef roxe::singleton<"proxy"_n, ProxyStorage> ProxyStorageSingleton;

struct [[roxe::table("zoo"), roxe::contract("eosdos")]] ZooStorage {
   OwnableStore ownable;
   address      _DODO_LOGIC_;
   address      _CLONE_FACTORY_;

   address _DEFAULT_SUPERVISOR_;
   //_DODO_REGISTER_[baseToken][quoteToken] = dodo;
   std::map<namesym, QuoteToken2Dodo> _DODO_REGISTER_;
   std::vector<address>               _DODOs;

   ROXELIB_SERIALIZE(ZooStorage, (ownable)(_DODO_LOGIC_)(_CLONE_FACTORY_)(_DEFAULT_SUPERVISOR_)(_DODO_REGISTER_)(_DODOs))
};

typedef roxe::singleton<"zoo"_n, ZooStorage> ZooStorageSingleton;
