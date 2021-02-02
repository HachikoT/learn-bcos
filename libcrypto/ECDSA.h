// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 常用类型，函数定义
 * @file: Common.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2020-08-31
 */
#pragma once

#include <libdevcore/Address.h>
#include <libdevcore/RLP.h>
#include "keccak.h"
#include "Exceptions.h"

namespace dev {

/**
 * 比特币/以太坊为何选择secp256k1
 * 比特币开发者社区曾讨论过 secp256k1 是否安全。中本聪没有明确解释，只是说道”有根据的推测”。从社区的讨论中，有推测是其它的曲线，
 * 比如secp256r1中的参数是美国国安局精心挑选的，相当于安全性受到权威机构的干涉。总的来说选择secp256k1是安全和性能考量的结果。
 * 以太坊沿用了比特币中的数字签名算法。
 */

/**
 * 长期以来，实现了该算法的第三方库只有openssl，因此btcoin core一直都引用了此库。openssl是一个庞大的开源库，不仅仅实现了椭圆曲线加密算法，
 * 在椭圆曲线加密算法中也不仅仅实现了secp256k1这一种椭圆曲线。不久大家就发现了openssl的一些问题，除了自身结构复杂庞大，文档也不全面，最重要的是，
 * opensssl的算法一致性也有潜在的问题，这会导致区块链发生不可预料的分叉，造成难以估量的损失。所以自2016年2月13日起，
 * 在新发布的bitcoin core 0.12.0版本中，libsecp256k1库代替了openssl ecdsa。 libsecp256k1中只实现了一种椭圆曲线算法，代码简练，
 * 很快大部分社区就接收了这种改变。ibsecp256k1已经成为bitcoin社区事实上的标准，成为开发者唯一能选择的官方库。
 */

// 公钥类型（64字节）
using Public = H512;

// 私钥类型（32字节）
using Secret = H256;

// 数字签名类型（65字节）
// A signature: 65 bytes: r: [0, 32), s: [32, 64), v: 64.
using Signature = H520;

// 数字签名类型（结构体格式）
struct SignatureStruct {
    SignatureStruct() = default;
    SignatureStruct(const H256& r, const H256& s, Byte v) noexcept : r(r), s(s), v(v) {}

    // 检测v,r,s的值是否都处于合法范围（初步检测）
    bool isValid() const noexcept;

    H256 r;         // (r, s)为用私钥对数字摘要签名后的信息
    H256 s;
    Byte v = 0;     // v存储了recovery id，用于从多个可能的结果公钥中直接确定正确的公钥，提升recovery效率
};

/**
 * 计算私钥对应的公钥
 * @param secret 私钥（32字节）
 * @return 对应的公钥（64字节）
 * @throw 若私钥非法则抛出BadPrivateKey异常
 */
Public toPublic(const Secret& secret);

/**
 * 计算公钥对应的账户地址（计算公钥的keccak哈希值，再取哈希值的后160位作为账户地址）
 * @param pub 公钥（64字节）
 * @return 公钥对应的账户地址（20字节）
 */
inline Address toAddress(const Public& pub) noexcept { return right160(keccak(pub)); }

/**
 * 计算合约账户地址
 * @param sender 发起创建合约交易的账户地址
 * @param nonce sender账户中记录的nonce值
 * @return 新创建的合约的账户地址
 */
inline Address toAddress(const Address& sender, const U256& nonce) { return right160(keccak(rlpList(sender, nonce))); }

// 密钥对类型
struct KeyPair {
    // 通过私钥构造密钥对，若私钥非法抛出BadPrivateKey异常
    KeyPair(const Secret& sec) : secret(sec), pub(toPublic(sec)), address(toAddress(pub)) {}

    // 私钥->公钥->账户地址
    Secret secret;
    Public pub;
    Address address;
};

/**
 * 根据签名信息和被签名的数字摘要恢复出签名者公钥
 * @param sig 签名信息
 * @param digest 被签名的数字摘要
 * @return 签名者的公钥
 * @throw 恢复错误抛出BadSignature异常
 */
Public recover(const SignatureStruct& sig, const H256& digest);

/**
 * 用私钥对数字摘要进行签名
 * @param secret 私钥
 * @param digest 数字摘要
 * @return 数字签名
 * @throw 签名错误抛出BadPrivateKey异常
 */
SignatureStruct sign(const Secret& secret, const H256& digest);

/**
 * 验证签名者公钥是否为pub
 * @param pub 指定公钥
 * @param sig 签名信息
 * @param digest 被签名的数字摘要
 * @return 若是返回true，否则返回false
 * @throw 验证错误抛出BadSignature异常
 */
bool verify(const Public& pub, const SignatureStruct& sig, const H256& digest);

}   // namespace dev