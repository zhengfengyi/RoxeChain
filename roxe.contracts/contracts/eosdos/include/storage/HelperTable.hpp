
#pragma once
#include <common/defines.hpp>
#include<storage/LibTable.hpp>
#include <storage/TokenTable.hpp>

struct MigrationsStore {
   address owner;
   uint256 last_completed_migration;
   ROXELIB_SERIALIZE(MigrationsStore, (owner)(last_completed_migration))
};

struct OracleStore {
   OwnableStore ownable;
   address      _OWNER_;
   extended_asset      tokenPrice;
   ROXELIB_SERIALIZE(OracleStore, (ownable)(_OWNER_)(tokenPrice))
};

struct Transaction {
   address destination;
   uint256 value;
   bytes   data;
   bool    executed;
   ROXELIB_SERIALIZE(Transaction, (destination)(value)(data)(executed))
};

struct EmergencyCall {
   bytes32 selector;
   uint256 paramsBytesCount;
   ROXELIB_SERIALIZE(EmergencyCall, (selector)(paramsBytesCount))
};

struct a2b {
   std::map<address, bool> a2c;
   ROXELIB_SERIALIZE(a2b, (a2c))
};

struct MultiSigWalletWithTimelockStore {
   std::map<uint256, Transaction> transactions;
   std::map<uint256, a2b>         confirmations;
   std::map<address, bool>        isOwner;
   std::map<address, address>     unlockTimes;

   std::vector<address> owners;
   uint256              required;
   uint256              transactionCount;

   // Functions bypass the time lock process
   std::vector<EmergencyCall> emergencyCalls;

   bool mutex;

   ROXELIB_SERIALIZE(
       MultiSigWalletWithTimelockStore,
       (transactions)(confirmations)(isOwner)(unlockTimes)(owners)(required)(transactionCount)(emergencyCalls)(mutex))
};

struct IUniswapV2FactoryStore {
   address factory;
   address token0;
   address token1;

   uint112 reserve0;           // uses single storage slot, accessible via getReserves
   uint112 reserve1;           // uses single storage slot, accessible via getReserves
   uint32  blockTimestampLast; // uses single storage slot, accessible via getReserves

   uint256 price0CumulativeLast;
   uint256 price1CumulativeLast;
   uint256 kLast; // reserve0 * reserve1, as of immediately after the most recent liquidity
   uint256 unlocked = 1;

   ROXELIB_SERIALIZE(
       IUniswapV2FactoryStore, (factory)(token0)(token1)(reserve0)(reserve1)(blockTimestampLast)(price0CumulativeLast)(
                                   price1CumulativeLast)(kLast)(unlocked))
};

struct UniswapArbitrageurStore {
   address _UNISWAP_;
   address _DODO_;
   address _BASE_;
   address _QUOTE_;

   bool _REVERSE_; // true if dodo.baseToken=uniswap.token0

   ROXELIB_SERIALIZE(UniswapArbitrageurStore, (_UNISWAP_)(_DODO_)(_BASE_)(_QUOTE_)(_REVERSE_))
};

struct [[roxe::table("helper"), roxe::contract("eosdos")]] HelperStorage {
   MigrationsStore                 mig;
   MultiSigWalletWithTimelockStore msig;
   IUniswapV2FactoryStore          factory;
   UniswapArbitrageurStore         arbit;

   ROXELIB_SERIALIZE(HelperStorage, (mig)(msig)(factory)(arbit))
};

typedef roxe::singleton<"helper"_n, HelperStorage> HelperStorageSingleton;

struct [[roxe::table("oracle"), roxe::contract("eosdos")]] OracleStorage {
   std::map<namesym, OracleStore> oracles;
   ROXELIB_SERIALIZE(OracleStorage, (oracles))
};

typedef roxe::singleton<"oracle"_n, OracleStorage> OracleStorageSingleton;
