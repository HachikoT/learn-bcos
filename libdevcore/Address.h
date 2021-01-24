// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 账户地址类型
 * @file: Address.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-24
 */
#pragma once

#include "FixedBytes.h"

namespace dev {

/**
 * 以太坊的账户共分成两类
 * 外部账户（Externally Owned Account, EOA）：链上用户的账户，由用户的私钥控制
 * 合约账户（Contract Account, CA）：部署智能合约产生的账户，由合约自身的代码控制
 * 账户地址计算规则：
 * 外部账户：EOA=right160(keccak256(toPublic(secp256k1_privateKey)))
 * 合约账户：CA=right160(keccak256(rlp(msg.sender, msg.sender.nonce)))
 */

// 账户地址类型（20字节）
using Address = H160;
using Addresses = H160s;

}   // namespace dev