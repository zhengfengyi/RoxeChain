# Roxe Earn

Roxe Earn - Roxe DODO Swap

##代码结构
```
├── README.md
└── earn.contracts
    ├── CMakeLists.txt
    ├── build.sh
    ├── contracts
    │   ├── CMakeLists.txt
    │   └── eosdos
    │       ├── CMakeLists.txt
    │       ├── README.md
    │       ├── batchrename.py
    │       ├── include
    │       │   ├── common
    │       │   │   ├── defines.hpp
    │       │   │   ├── dos.functions.hpp
    │       │   │   ├── dos.types.hpp
    │       │   │   ├── extended_token.hpp
    │       │   │   ├── instance_mgmt.hpp
    │       │   │   ├── murmurhash.hpp
    │       │   │   ├── roxe.tokenize.hpp
    │       │   │   ├── storage_mgmt.hpp
    │       │   │   └── transfer_mgmt.hpp
    │       │   ├── eosdos
    │       │   │   ├── DODOEthProxy.hpp
    │       │   │   ├── DODOZoo.hpp
    │       │   │   ├── dodo.hpp
    │       │   │   ├── helper
    │       │   │   │   ├── MultiSig.hpp
    │       │   │   │   ├── TestERC20.hpp
    │       │   │   │   ├── TestWETH.hpp
    │       │   │   │   ├── UniswapArbitrageur.hpp
    │       │   │   │   └── UniswapV2.hpp
    │       │   │   ├── impl
    │       │   │   │   ├── Admin.hpp
    │       │   │   │   ├── DODOLpToken.hpp
    │       │   │   │   ├── LiquidityProvider.hpp
    │       │   │   │   ├── Pricing.hpp
    │       │   │   │   ├── Settlement.hpp
    │       │   │   │   ├── Storage.hpp
    │       │   │   │   └── Trader.hpp
    │       │   │   ├── intf
    │       │   │   │   ├── IDODO.hpp
    │       │   │   │   ├── IDODOCallee.hpp
    │       │   │   │   ├── IDODOLpToken.hpp
    │       │   │   │   ├── IERC20.hpp
    │       │   │   │   ├── IFactory.hpp
    │       │   │   │   ├── IOracle.hpp
    │       │   │   │   └── IWETH.hpp
    │       │   │   ├── lib
    │       │   │   │   ├── DODOMath.hpp
    │       │   │   │   ├── DecimalMath.hpp
    │       │   │   │   ├── SafeERC20.hpp
    │       │   │   │   ├── SafeMath.hpp
    │       │   │   │   └── Types.hpp
    │       │   │   └── token
    │       │   │       ├── DODOMine.hpp
    │       │   │       ├── DODOMineReader.hpp
    │       │   │       ├── DODORewardVault.hpp
    │       │   │       ├── DODOToken.hpp
    │       │   │       └── LockedTokenVault.hpp
    │       │   └── storage
    │       │       ├── HelperTable.hpp
    │       │       ├── ImplTable.hpp
    │       │       ├── LibTable.hpp
    │       │       ├── TokenTable.hpp
    │       │       └── ZooTable.hpp
    │       └── src
    │           ├── eosdos.cpp
    │           └── extended_token.cpp
    ├── mybuild.sh
    ├── mytest.sh
    └── tests
        ├── CMakeLists.txt
        ├── contracts.hpp.in
        ├── eosdos_tests.cpp
        ├── main.cpp
        └── test_symbol.hpp
```

### 代码结构说明
#### 主要业务逻辑impl
* [eosdos.cpp](earn.contracts/contracts/eosdos/src/eosdos.cpp) 合约外部调用接口Action(roxe::action/eosio::action)
* [dodo.hpp](earn.contracts/contracts/eosdos/include/eosdos/dodo.hpp)交易对初始化Init 
* [LiquidityProvider.hpp](earn.contracts/contracts/eosdos/include/eosdos/impl/LiquidityProvider.hpp) 添加提取流动性功能 存/提取 base/quote代币.
* [Trader.hpp](earn.contracts/contracts/eosdos/include/eosdos/impl/Trader.hpp)  兑换功能如 买卖base代币，卖quote代币。
* [Pricing.hpp](earn.contracts/contracts/eosdos/include/eosdos/impl/Pricing.hpp)  价格功能 根据base兑换量及oracle 价格计算quote兑换量
* [Settlement.hpp](earn.contracts/contracts/eosdos/include/eosdos/impl/Settlement.hpp)  结算功能如 转账base/quote 代币，lp代币，维护资费。
* [Admin.hp](earn.contracts/contracts/eosdos/include/eosdos/impl/Admin.hpp)  管理功能实现 如费率设置，启用/停用流动性，兑换等功能。
* [DODOLpToken.hpp](earn.contracts/contracts/eosdos/include/eosdos/impl/DODOLpToken.hpp) lp代币接口实现
* [Storage.hpp](earn.contracts/contracts/eosdos/include/eosdos/impl/Storage.hpp)  代币lp余额
#### 数学公式lib
* [DODOMath.hpp](earn.contracts/contracts/eosdos/include/eosdos/lib/DODOMath.hpp)  二次方程式计算
* [DecimalMath.hpp](earn.contracts/contracts/eosdos/include/eosdos/lib/DecimalMath.hpp) 安全数学计算
#### 存储结构storage（roxe::table/eosiotable)
* earn.contracts/contracts/eosdos/include/storage
  交易对存储结构表，代币存储结构表

#### 公共模块common
* earn.contracts/contracts/eosdos/include/common
常量定义，代币转账封装，存储封装，roxe代币定义，lp代币实现
### 主要业务流程
#### 添加/提取流动性业务流程
1. 添加流动性Action eosdos.cpp
2. 添加流动性实现逻辑 LiquidityProvider.hpp
3. 获取预期target  pricing.hpp
4. 获取总份额代币lp  settlement.hpp
5. 存奖励/提取费用 
6. 转入/出代币  settlement.hpp
7. 铸币/销币  settlement.hpp
8. 转账  封装实现transfer_mgmt.hpp
9. roxe.tokenize.hpp
10. extended_token.hpp

#### 兑换代币业务流程
1. 兑换代币Action eosdos.cpp
2. 兑换代币实现逻辑 Trader.hpp
3. 获取预期target  pricing.hpp
4. 查询兑换量  pricing.hpp
5. lp/mt费用 
6. 转入/出代币  settlement.hpp
7. 转账lp/mt费用  settlement.hpp
8. 转账  封装实现transfer_mgmt.hpp
9. 转账费 roxe.tokenize.hpp


#### 管理功能业务流程
1. 管理功能Action eosdos.cpp
2. 管理功能实现逻辑 Admin.hpp
3. 存储实现storage_mgmt.hpp


##编译

* 编译服务器 172.16.213.156

* /data/roxe/balanceos/RoxeChain/roxe.contracts

```
./mybuild.sh
or
./build.sh -e /data/roxe/actc/build -c /data/roxe/actc/roxe.cdt/build
```

##部署

```
CLS=/data/roxe/test/cls

$CLS set contract eosdoseosdos /data/roxe/balanceos/RoxeChain/roxe.contracts/build/contracts/eosdos -p eosdoseosdos
 


```

##核心思想
### 核心概念
https://dodoex.github.io/cn/docs/pmmDetails
### PMM 算法的数学原理
https://dodoex.github.io/cn/docs/math
