// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include <common/BType.hpp>

#include <eoswap/BNum.hpp>

class BMath :  public BNum {
public:
  /**********************************************************************************************
  // calcSpotPrice //
  // sP = spotPrice //
  // bI = tokenBalanceIn                ( bI / wI )         1 //
  // bO = tokenBalanceOut         sP =  -----------  *  ---------- //
  // wI = tokenWeightIn                 ( bO / wO )     ( 1 - sF ) //
  // wO = tokenWeightOut //
  // sF = swapFee //
  **********************************************************************************************/
  uint256 calcSpotPrice(uint256 tokenBalanceIn, uint256 tokenWeightIn,
                     uint256 tokenBalanceOut, uint256 tokenWeightOut, uint256 swapFee) {
    uint256 numer = bdiv(tokenBalanceIn, tokenWeightIn);
    uint256 denom = bdiv(tokenBalanceOut, tokenWeightOut);
    uint256 ratio = bdiv(numer, denom);
    uint256 scale = bdiv(BONE, bsub(BONE, swapFee));
    uint256 spotPrice = bmul(ratio, scale);
    return spotPrice;
  }

  /**********************************************************************************************
  // calcOutGivenIn //
  // aO = tokenAmountOut //
  // bO = tokenBalanceOut //
  // bI = tokenBalanceIn              /      /            bI             \ (wI /
  wO) \      //
  // aI = tokenAmountIn    aO = bO * |  1 - | --------------------------  | ^ |
  //
  // wI = tokenWeightIn               \      \ ( bI + ( aI * ( 1 - sF )) / / //
  // wO = tokenWeightOut //
  // sF = swapFee //
  **********************************************************************************************/
  uint256 calcOutGivenIn(uint256 tokenBalanceIn, uint256 tokenWeightIn,
                      uint256 tokenBalanceOut, uint256 tokenWeightOut,
                      uint256 tokenAmountIn, uint256 swapFee) {
    uint256 weightRatio = bdiv(tokenWeightIn, tokenWeightOut);
    uint256 adjustedIn = bsub(BONE, swapFee);
    adjustedIn = bmul(tokenAmountIn, adjustedIn);
    uint256 y = bdiv(tokenBalanceIn, badd(tokenBalanceIn, adjustedIn));
    uint256 foo = bpow(y, weightRatio);
    uint256 bar = bsub(BONE, foo);
    uint256 tokenAmountOut = bmul(tokenBalanceOut, bar);
    return tokenAmountOut;
  }

  /**********************************************************************************************
  // calcInGivenOut //
  // aI = tokenAmountIn //
  // bO = tokenBalanceOut               /  /     bO      \    (wO / wI)      \
  //
  // bI = tokenBalanceIn          bI * |  | ------------  | ^            - 1  |
  //
  // aO = tokenAmountOut    aI =        \  \ ( bO - aO ) /                   /
  //
  // wI = tokenWeightIn           --------------------------------------------
  //
  // wO = tokenWeightOut                          ( 1 - sF ) //
  // sF = swapFee //
  **********************************************************************************************/
  uint256 calcInGivenOut(uint256 tokenBalanceIn, uint256 tokenWeightIn,
                      uint256 tokenBalanceOut, uint256 tokenWeightOut,
                      uint256 tokenAmountOut, uint256 swapFee) {
    uint256 weightRatio = bdiv(tokenWeightOut, tokenWeightIn);
    uint256 diff = bsub(tokenBalanceOut, tokenAmountOut);
    uint256 y = bdiv(tokenBalanceOut, diff);
    uint256 foo = bpow(y, weightRatio);
    foo = bsub(foo, BONE);
    uint256 tokenAmountIn = bsub(BONE, swapFee);
    tokenAmountIn = bdiv(bmul(tokenBalanceIn, foo), tokenAmountIn);
    return tokenAmountIn;
  }

  /**********************************************************************************************
  // calcPoolOutGivenSingleIn //
  // pAo = poolAmountOut         / \              //
  // tAi = tokenAmountIn        ///      /     //    wI \      \\       \     wI
  \             //
  // wI = tokenWeightIn        //| tAi *| 1 - || 1 - --  | * sF || + tBi \    --
  \            //
  // tW = totalWeight     pAo=||  \      \     \\    tW /      //         | ^ tW
  | * pS - pS //
  // tBi = tokenBalanceIn      \\  ------------------------------------- / / //
  // pS = poolSupply            \\                    tBi               / / //
  // sF = swapFee                \ /              //
  **********************************************************************************************/
  uint256 calcPoolOutGivenSingleIn(uint256 tokenBalanceIn, uint256 tokenWeightIn,
                                uint256 poolSupply, uint256 totalWeight,
                                uint256 tokenAmountIn, uint256 swapFee) {
    // Charge the trading fee for the proportion of tokenAi
    ///  which is implicitly traded to the other pool tokens.
    // That proportion is (1- weightTokenIn)
    // tokenAiAfterFee = tAi * (1 - (1-weightTi) * poolFee);
    uint256 normalizedWeight = bdiv(tokenWeightIn, totalWeight);
    uint256 zaz = bmul(bsub(BONE, normalizedWeight), swapFee);
    uint256 tokenAmountInAfterFee = bmul(tokenAmountIn, bsub(BONE, zaz));

    uint256 newTokenBalanceIn = badd(tokenBalanceIn, tokenAmountInAfterFee);
    uint256 tokenInRatio = bdiv(newTokenBalanceIn, tokenBalanceIn);

    // uint256 newPoolSupply = (ratioTi ^ weightTi) * poolSupply;
    uint256 poolRatio = bpow(tokenInRatio, normalizedWeight);
    uint256 newPoolSupply = bmul(poolRatio, poolSupply);
    uint256 poolAmountOut = bsub(newPoolSupply, poolSupply);
    return poolAmountOut;
  }

  /**********************************************************************************************
  // calcSingleInGivenPoolOut //
  // tAi = tokenAmountIn              //(pS + pAo)\     /    1    \\ //
  // pS = poolSupply                 || ---------  | ^ | --------- || * bI - bI
  //
  // pAo = poolAmountOut              \\    pS    /     \(wI / tW)// //
  // bI = balanceIn          tAi =  --------------------------------------------
  //
  // wI = weightIn                              /      wI  \ //
  // tW = totalWeight                          |  1 - ----  |  * sF //
  // sF = swapFee                               \      tW  / //
  **********************************************************************************************/
  uint256 calcSingleInGivenPoolOut(uint256 tokenBalanceIn, uint256 tokenWeightIn,
                                uint256 poolSupply, uint256 totalWeight,
                                uint256 poolAmountOut, uint256 swapFee) {
    uint256 normalizedWeight = bdiv(tokenWeightIn, totalWeight);
    uint256 newPoolSupply = badd(poolSupply, poolAmountOut);
    uint256 poolRatio = bdiv(newPoolSupply, poolSupply);

    // uint256 newBalTi = poolRatio^(1/weightTi) * balTi;
    uint256 boo = bdiv(BONE, normalizedWeight);
    uint256 tokenInRatio = bpow(poolRatio, boo);
    uint256 newTokenBalanceIn = bmul(tokenInRatio, tokenBalanceIn);
    uint256 tokenAmountInAfterFee = bsub(newTokenBalanceIn, tokenBalanceIn);
    // Do reverse order of fees charged in joinswap_ExternAmountIn, this way
    //     ``` pAo == joinswap_ExternAmountIn(Ti, joinswap_PoolAmountOut(pAo,
    //     Ti)) ```
    // uint256 tAi = tAiAfterFee / (1 - (1-weightTi) * swapFee) ;
    uint256 zar = bmul(bsub(BONE, normalizedWeight), swapFee);
    uint256 tokenAmountIn = bdiv(tokenAmountInAfterFee, bsub(BONE, zar));
    return tokenAmountIn;
  }

  /**********************************************************************************************
  // calcSingleOutGivenPoolIn //
  // tAo = tokenAmountOut            /      / \\   //
  // bO = tokenBalanceOut           /      // pS - (pAi * (1 - eF)) \     /    1
  \      \\  //
  // pAi = poolAmountIn            | bO - || ----------------------- | ^ |
  --------- | * b0 || //
  // ps = poolSupply                \      \\          pS           /     \(wO /
  tW)/      //  //
  // wI = tokenWeightIn      tAo =   \      \ //   //
  // tW = totalWeight                    /     /      wO \       \ //
  // sF = swapFee                    *  | 1 - |  1 - ---- | * sF  | //
  // eF = exitFee                        \     \      tW /       / //
  **********************************************************************************************/
  uint256 calcSingleOutGivenPoolIn(uint256 tokenBalanceOut, uint256 tokenWeightOut,
                                uint256 poolSupply, uint256 totalWeight,
                                uint256 poolAmountIn, uint256 swapFee) {
    uint256 normalizedWeight = bdiv(tokenWeightOut, totalWeight);
    // charge exit fee on the pool token side
    // pAiAfterExitFee = pAi*(1-exitFee)
    uint256 poolAmountInAfterExitFee = bmul(poolAmountIn, bsub(BONE, EXIT_FEE));
    uint256 newPoolSupply = bsub(poolSupply, poolAmountInAfterExitFee);
    uint256 poolRatio = bdiv(newPoolSupply, poolSupply);

    // newBalTo = poolRatio^(1/weightTo) * balTo;
    uint256 tokenOutRatio = bpow(poolRatio, bdiv(BONE, normalizedWeight));
    uint256 newTokenBalanceOut = bmul(tokenOutRatio, tokenBalanceOut);

    uint256 tokenAmountOutBeforeSwapFee =
        bsub(tokenBalanceOut, newTokenBalanceOut);

    // charge swap fee on the output token side
    // uint256 tAo = tAoBeforeSwapFee * (1 - (1-weightTo) * swapFee)
    uint256 zaz = bmul(bsub(BONE, normalizedWeight), swapFee);
    uint256 tokenAmountOut = bmul(tokenAmountOutBeforeSwapFee, bsub(BONE, zaz));
    return tokenAmountOut;
  }

  /**********************************************************************************************
  // calcPoolInGivenSingleOut //
  // pAi = poolAmountIn               // /               tAo             \\ / wO
  \     \   //
  // bO = tokenBalanceOut            // | bO - -------------------------- |\   |
  ---- |     \  //
  // tAo = tokenAmountOut      pS - ||   \     1 - ((1 - (tO / tW)) * sF)/  | ^
  \ tW /  * pS | //
  // ps = poolSupply                 \\ -----------------------------------/ /
  //
  // wO = tokenWeightOut  pAi =       \\               bO                 / / //
  // tW = totalWeight
  -------------------------------------------------------------  //
  // sF = swapFee                                        ( 1 - eF ) //
  // eF = exitFee //
  **********************************************************************************************/
  uint256 calcPoolInGivenSingleOut(uint256 tokenBalanceOut, uint256 tokenWeightOut,
                                uint256 poolSupply, uint256 totalWeight,
                                uint256 tokenAmountOut, uint256 swapFee) {

    // charge swap fee on the output token side
    uint256 normalizedWeight = bdiv(tokenWeightOut, totalWeight);
    // uint256 tAoBeforeSwapFee = tAo / (1 - (1-weightTo) * swapFee) ;
    uint256 zoo = bsub(BONE, normalizedWeight);
    uint256 zar = bmul(zoo, swapFee);
    uint256 tokenAmountOutBeforeSwapFee = bdiv(tokenAmountOut, bsub(BONE, zar));

    uint256 newTokenBalanceOut =
        bsub(tokenBalanceOut, tokenAmountOutBeforeSwapFee);
    uint256 tokenOutRatio = bdiv(newTokenBalanceOut, tokenBalanceOut);

    // uint256 newPoolSupply = (ratioTo ^ weightTo) * poolSupply;
    uint256 poolRatio = bpow(tokenOutRatio, normalizedWeight);
    uint256 newPoolSupply = bmul(poolRatio, poolSupply);
    uint256 poolAmountInAfterExitFee = bsub(poolSupply, newPoolSupply);

    // charge exit fee on the pool token side
    // pAi = pAiAfterExitFee/(1-exitFee)
    uint256 poolAmountIn = bdiv(poolAmountInAfterExitFee, bsub(BONE, EXIT_FEE));
    return poolAmountIn;
  }
};
