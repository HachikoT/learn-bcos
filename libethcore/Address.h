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

#include <libdevcore/FixedBytes.h>

namespace dev { namespace eth {

/**
 * 以太坊的账户共分成两类，fisco-bcos继承了以太坊的账户模型
 * 外部账户（Externally Owned Account, EOA）：链上用户的账户，由用户的私钥控制
 * 合约账户（Contract Account, CA）：部署智能合约产生的账户，由合约自身的代码控制
 * 账户地址计算规则：
 * 外部账户：EOA=right160(keccak256(toPublic(secp256k1_privateKey)))
 * 合约账户：CA=right160(keccak256(rlp(msg.sender, msg.sender.nonce)))
 */

// 账户地址类型（20字节）
using Address = H160;
using Addresses = H160s;

// /**
//  * 计算公钥对应的账户地址（计算公钥的keccak哈希值，再取哈希值的后160位作为账户地址）
//  * @param pub 公钥（64字节）
//  * @return 公钥对应的账户地址（20字节）
//  */
// inline Address toAddress(const Public& pub) noexcept { return right160(keccak(pub)); }

// /**
//  * 计算合约账户地址
//  * @param sender 发起创建合约交易的账户地址
//  * @param nonce sender账户中记录的nonce值
//  * @return 新创建的合约的账户地址
//  */
// inline Address toAddress(const Address& sender, const U256& nonce) { return right160(keccak(rlpList(sender, nonce))); }

}}   // namespace dev::eth