// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 常用类型，函数定义
 * @file: ECDSA.cpp
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-02-02
 */
#include "ECDSA.h"
#include <memory>
#include <array>
#include <cassert>
#include <secp256k1.h>
#include <secp256k1_recovery.h>

namespace dev {

// secp256k1的阶数n
const U256 c_secp256k1n("0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
const U256 c_secp256k1nHalf = c_secp256k1n / 2;

/**
 * 获取secp256k1上下文的常量指针，上下文的创建比较耗时，所以创建为全局变量
 * 使用secp256k1上下文的常量指针是线程安全的
 */
static const secp256k1_context* getSECP256K1Ctx() {
    static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_secp256k1Ctx(
        secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
        &secp256k1_context_destroy
    );
    return s_secp256k1Ctx.get();
}

/**
 * 计算私钥对应的公钥
 * @param sec 私钥（32字节）
 * @return 对应的公钥（64字节）
 * @throw 若私钥非法则抛出BadSecKey异常
 */
PubKey toPubKey(const SecKey& sec) {
    // 获取上下文
    auto secp256k1Ctx = getSECP256K1Ctx();

    // 从私钥计算出原始公钥（64字节）
    secp256k1_pubkey rawPub;
    if (!secp256k1_ec_pubkey_create(secp256k1Ctx, &rawPub, sec.data())) {
        throw BadSecKey();
    }

    // 将原始公钥进行序列化（65字节！），原始公钥不能跨平台，跨版本使用，需要用先序列化
    // 多的一个字节用来标识公钥，0x04作为第一个标识字节，表示未压缩
    std::array<Byte, 65> serializedPub;
    size_t serializedPubSize = serializedPub.size();
    secp256k1_ec_pubkey_serialize(
        secp256k1Ctx,               // 上下文
        serializedPub.data(),    // 序列化公钥的输出
        &serializedPubSize,      // 初始化为输出缓存区的长度，返回实际写入输出缓冲区的长度
        &rawPub,                 // 原始公钥
        SECP256K1_EC_UNCOMPRESSED   // 不压缩占用65字节，压缩占用33字节
    );

    // 确保写入了65个字节
    assert(serializedPub.size() == serializedPubSize);
    // 确保第一个标识字节正确
    assert(0x04 == serializedPub[0]);

    return PubKey(BytesConstRef(&serializedPub[1], serializedPub.size() - 1));
}

/**
 * 用私钥对数字摘要进行签名
 * @param sec 私钥
 * @param digest 数字摘要
 * @return 数字签名
 * @throw 签名错误抛出BadSignature异常
 */
Signature sign(const SecKey& sec, const H256& digest) {
    // 获取上下文
    auto secp256k1Ctx = getSECP256K1Ctx();

    // 对数字摘要进行签名，获取原始签名信息
    secp256k1_ecdsa_recoverable_signature rawSig;
    if (!secp256k1_ecdsa_sign_recoverable(secp256k1Ctx, &rawSig, digest.data(), sec.data(), nullptr, nullptr)) {
        throw BadSignature();
    }

    // 将原始签名信息序列化
    Signature serializedSig;
    int recoveryId = 0;
    secp256k1_ecdsa_recoverable_signature_serialize_compact(
        secp256k1Ctx,                                       // 上下文
        reinterpret_cast<unsigned char*>(&serializedSig),   // 序列化签名输出
        &recoveryId,                                        // 恢复id
        &rawSig                                             // 原始签名信息
    );
    serializedSig.v = recoveryId;

    // 调整签名信息，确保s<=c_secp256k1nHalf
    // 这是因为对于s>c_secp256k1nHalf的值，可以通过调整为s=c_secp256k1n-s，然后recoveryId^=1（顺序变一下），照样是合法的签名
    // 这对于普通的应用场景没问题，但是以太坊中需要用到签名值来计算hash，作为整个交易的hash值，如果有人恶意变换s的值，那么会导致查不出历史交易
    // 所以以太坊规定当s大于c_secp256k1nHalf时，转换为s=c_secp256k1n-s
    // 详见EIP2 https://eips.ethereum.org/EIPS/eip-2
    U256 numericS = serializedSig.s.toArith();
    if (numericS > c_secp256k1nHalf) {
        serializedSig.v ^= 0x01;
        serializedSig.s = H256(c_secp256k1n - numericS);
    }
    assert(serializedSig.s.toArith() <= c_secp256k1nHalf);

    return serializedSig;
}

/**
 * 根据签名信息和被签名数据的数字摘要恢复出签名者公钥
 * @param sig 签名信息
 * @param digest 被签名数据的数字摘要
 * @return 签名者的公钥
 * @throw 恢复错误抛出BadSignature异常
 */
PubKey recover(const Signature& sig, const H256& digest) {
    // 对于secp256k1曲线而言，只有四种recoveryId，分别是0，1，2，3
    // 以太坊中直接忽略了recoveryId等于2和3的情况，因为概率很低，大约为3.73*10^-39
    // 详见fisco-bcos的博客 https://my.oschina.net/fiscobcos/blog/4384028
    // 至于为什么忽略，我也不懂，可能是为了省id数吧，有知道的兄弟补充一下
    if (sig.v > 1) {
        throw BadSignature();
    }

    // 见上面关于s值取值范围的解释
    if (sig.s.toArith() > c_secp256k1nHalf) {
        throw BadSignature();
    }

    // 获取上下文
    auto secp256k1Ctx = getSECP256K1Ctx();

    // 解析出原始签名信息格式
    secp256k1_ecdsa_recoverable_signature rawSig;
    if (!secp256k1_ecdsa_recoverable_signature_parse_compact(secp256k1Ctx, &rawSig, reinterpret_cast<const unsigned char*>(&sig), sig.v)) {
        throw BadSignature();
    }

    // 解析出原始公钥
    secp256k1_pubkey rawPub;
    if (!secp256k1_ecdsa_recover(secp256k1Ctx, &rawPub, &rawSig, digest.data())) {
        throw BadSignature();
    }

    // 将原始公钥进行序列化（65字节！）
    // 多的一个字节用来标识公钥，0x04作为第一个标识字节
    std::array<Byte, 65> serializedPub;
    size_t serializedPubSize = serializedPub.size();
    secp256k1_ec_pubkey_serialize(
        secp256k1Ctx,               // 上下文
        serializedPub.data(),    // 序列化公钥的输出
        &serializedPubSize,      // 初始化为输出缓存区的长度，返回实际写入输出缓冲区的长度
        &rawPub,                 // 原始公钥
        SECP256K1_EC_UNCOMPRESSED   // 不压缩占用65字节，压缩占用33字节
    );

    // 确保写入了65个字节
    assert(serializedPub.size() == serializedPubSize);
    // 确保第一个标识字节正确
    assert(0x04 == serializedPub[0]);

    return PubKey(BytesConstRef(&serializedPub[1], serializedPub.size() - 1));
}

}   // namespace dev