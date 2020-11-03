
#pragma once
#include <common/defines.hpp>

// Info of each user.
struct UserInfo {
   uint256 amount;     // How many LP tokens the user has provided.
   uint256 rewardDebt; // Reward debt. See explanation below.
                       //
                       // We do some fancy math here. Basically, any point in time, the amount of DODOs
                       // entitled to a user but is pending to be distributed is:
                       //
                       //   pending reward = (user.amount * pool.accDODOPerShare) - user.rewardDebt
                       //
                       // Whenever a user deposits or withdraws LP tokens to a pool. Here's what happens:
                       //   1. The pool's `accDODOPerShare` (and `lastRewardBlock`) gets updated.
                       //   2. User receives the pending reward sent to his/her address.
                       //   3. User's `amount` gets updated.
                       //   4. User's `rewardDebt` gets updated.
   ROXELIB_SERIALIZE(UserInfo, (amount)(rewardDebt))
};

// Info of each pool.
struct PoolInfo {
   address lpToken;         // Address of LP token contract.
   uint256 allocPoint;      // How many allocation points assigned to this pool. DODOs to distribute per block.
   uint256 lastRewardBlock; // Last block number that DODOs distribution occurs.
   uint256 accDODOPerShare; // Accumulated DODOs per share, times 1e12. See below.
   ROXELIB_SERIALIZE(PoolInfo, (lpToken)(allocPoint)(lastRewardBlock)(accDODOPerShare))
};

struct user2info {
   std::map<address, UserInfo> a2info; // is token bound to pool

   ROXELIB_SERIALIZE(user2info, (a2info))
};

struct DODOMineStore {
   address dodoRewardVault;
   uint256 dodoPerBlock;

   // Info of each pool.
   std::vector<PoolInfo>      poolInfos;
   std::map<address, uint256> lpTokenRegistry;

   // Info of each user that stakes LP tokens.
   std::map<uint256, user2info> userInfo;
   std::map<address, uint256>   realizedReward;

   // Total allocation points. Must be the sum of all allocation points in all pools.
   uint256 totalAllocPoint;
   // The block number when DODO mining starts.
   uint256 startBlock;
   ROXELIB_SERIALIZE(
       DODOMineStore, (dodoRewardVault)(dodoPerBlock)(poolInfos)(lpTokenRegistry)(userInfo)(realizedReward)(
                          totalAllocPoint)(startBlock))
};

struct DODORewardVaultStore {
   address dodoToken;
   ROXELIB_SERIALIZE(DODORewardVaultStore, (dodoToken))
};

struct Account2Amt {
   std::map<name, uint256> dst2amt; // is token bound to pool

   ROXELIB_SERIALIZE(Account2Amt, (dst2amt))
};

   //    string symbol = "DODO";
   //    string name   = "DODO bird";

   //    uint256 decimals    = 18;
   //    uint256 totalSupply = 1000000000 * 10 * *18; // 1 Billion

   //    mapping(address = > uint256) balances;
   //    mapping(address = > mapping(address = > uint256)) allowed;
   //    string  symbol = "DLP";

struct TokenStore {
   extended_symbol originToken;
   extended_symbol esymbol;
   OwnableStore    ownable;
   std::string                 names;
//    name            contract_self;
//    std::string                 symbols;
//    uint256                     decimals;
//    std::map<name, uint256>     balances;
//    std::map<name, uint256>     balanceOf;
//    std::map<name, Account2Amt> allowed;
//    std::map<name, Account2Amt> allowance;
//    uint256                     totalSupply;
// (contract_self)(symbols)(decimals)(balances)(allowed)(
//                        balanceOf)(allowance)(totalSupply)
   ROXELIB_SERIALIZE(
       TokenStore, (esymbol)(originToken)(ownable)(names))
};

struct LockedTokenVaultStore {

   address _TOKEN_;

   std::map<name, uint256> originBalances;
   std::map<name, uint256> claimedBalances;

   uint256 _UNDISTRIBUTED_AMOUNT_;
   uint256 _START_RELEASE_TIME_;
   uint256 _RELEASE_DURATION_;
   uint256 _CLIFF_RATE_;

   bool _DISTRIBUTE_FINISHED_;

   ROXELIB_SERIALIZE(
       LockedTokenVaultStore, (_TOKEN_)(originBalances)(claimedBalances)(_UNDISTRIBUTED_AMOUNT_)(_START_RELEASE_TIME_)(
                                  _RELEASE_DURATION_)(_CLIFF_RATE_)(_DISTRIBUTE_FINISHED_))
};

struct [[roxe::table("token"), roxe::contract("eosdos")]] TokenStorage {
   std::map<namesym, TokenStore> tokens;
   ROXELIB_SERIALIZE(TokenStorage, (tokens))
};

typedef roxe::singleton<"token"_n, TokenStorage> TokenStorageSingleton;

struct [[roxe::table("mining"), roxe::contract("eosdos")]] MiningStorage {
   DODOMineStore         mine;
   DODORewardVaultStore  rewardvault;
   LockedTokenVaultStore lockedtoken;
   ROXELIB_SERIALIZE(MiningStorage, (mine)(rewardvault)(lockedtoken))
};

typedef roxe::singleton<"mining"_n, MiningStorage> MiningStorageSingleton;