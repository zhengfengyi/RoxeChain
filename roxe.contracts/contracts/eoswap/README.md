# Roxe Swap

Roxe Balancer Swap

##代码结构
```

├── README.md
└── swap.contracts
    ├── CMakeLists.txt
    ├── build.sh
    ├── contracts
    │   ├── CMakeLists.txt
    │   └── eoswap
    │       ├── CMakeLists.txt
    │       ├── include
    │       │   ├── common
    │       │   │   ├── BType.hpp
    │       │   │   ├── IFactory.hpp
    │       │   │   ├── extended_token.hpp
    │       │   │   ├── instance_mgmt.hpp
    │       │   │   ├── roxe.tokenize.hpp
    │       │   │   ├── storage_mgmt.hpp
    │       │   │   └── transfer_mgmt.hpp
    │       │   ├── eoswap
    │       │   │   ├── BColor.hpp
    │       │   │   ├── BConst.hpp
    │       │   │   ├── BFactory.hpp
    │       │   │   ├── BMath.hpp
    │       │   │   ├── BNum.hpp
    │       │   │   ├── BPool.hpp
    │       │   │   └── BToken.hpp
    │       │   └── storage
    │       │       ├── BFactoryTable.hpp
    │       │       ├── BPoolTable.hpp
    │       │       └── BTokenTable.hpp
    │       ├── ricardian
    │       └── src
    │           ├── eoswap.cpp
    │           └── extended_token.cpp
    ├── mybuild.sh
    ├── mytest.sh
    └── tests
        ├── CMakeLists.txt
        ├── contracts.hpp.in
        ├── eoswap_tests.cpp
        ├── main.cpp
        └── test_symbol.hpp
```
### 代码结构说明
#### 主要业务逻辑impl
* [eoswap.cpp](swap.contracts/contracts/eoswap/src/eoswap.cpp) 合约外部调用接口Action(roxe::action/eosio::action)
* [BFactory.hpp](swap.contracts/contracts/eoswap/include/eoswap/BFactory.hpp)交易池初始化创建，提取兑换费用。
* [BPool.hpp](swap.contracts/contracts/eoswap/include/eoswap/BPool.hppp) 
    1. 添加提取流动性功能bind，rebind,unbind,joinPool,exitPool。
    2. 兑换功能swapExactAmountIn,swapExactAmountOut。
    3. 管理功能setSwapFee，setController，setPublicSwap，finalize,gulp。

* [BToken.hpp](swap.contracts/contracts/eoswap/include/eoswap/BToken.hpp) lp代币接口实现

#### 数学公式
* [BMath.hpp](swap.contracts/contracts/eoswap/include/eoswap/BMath.hpp)  二次方程式计算
* [BNum.hpp](swap.contracts/contracts/eoswap/include/eoswap/BNum.hpp) 安全数学计算
#### 存储结构storage（roxe::table/eosiotable)
* swap.contracts/contracts/eoswap/include/storage
  交易池存储结构表，代币存储结构表

#### 公共模块common
* swap.contracts/contracts/eoswap/include/common
常量定义，代币转账封装，存储封装，roxe代币定义，lp代币实现


### 主要业务流程
#### 创建交易池
1. 创建交易池Action newpool  eoswap.cpp
2. 设置兑换资费 setswapfee 
3. 设置交易池控制账户 setcontroler  
4. 设置是否允许公开兑换setpubswap  


#### 添加/提取初始流动性业务流程
1. 添加/提取初始流动性Action  bind/rebind/unbind eoswap.cpp
2. 添加流动性实现逻辑 BPool.hpp
3. 转账代币，分配lptoken
4. finalize  

#### 添加/提取流动性业务流程
1. 添加/提取流动性Action  joinpool/joinpool eoswap.cpp
2. 添加流动性实现逻辑 BPool.hpp
3. 转账代币，分配lptoken

#### 兑换代币业务流程
1. 兑换代币Action swapamtin/swapamtout eoswap.cpp
2. 兑换代币实现逻辑 swapExactAmountIn/swapExactAmountOut
3. 计算兑换出量
4. 验证现价 
5. 转入/出代币 
6. 转账  封装实现transfer_mgmt.hpp
7. 转账费 roxe.tokenize.hpp


   

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

$CLS set contract roxeswap1213 /data/roxe/balanceos/RoxeChain/roxe.contracts/build/contracts/eoswap -p roxeswap1213

```

## 核心思想
* https://docs.balancer.finance/
  