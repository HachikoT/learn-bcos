// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 常用类型，函数定义
 * @file: Common.cpp
 * @author: rancheng <rc4work@163.com>
 * @date: 2020-08-31
 */
#include "Common.h"
#include <memory>
#include <cassert>
#include <secp256k1.h>
#include <secp256k1_recovery.h>

namespace dev {

/**
 * 获取secp256k1上下文的常量指针，上下文的创建比较耗时，所以创建为全局变量
 * secp256k1上下文的常量指针保证线程安全
 */
static const secp256k1_context* getCtx() {
    static std::unique_ptr<secp256k1_context, decltype(&secp256k1_context_destroy)> s_ctx(
        secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY),
        &secp256k1_context_destroy
    );
    return s_ctx.get();
}

// 检测v,r,s的值是否都处于合法范围（初步检测）
bool SignatureStruct::isValid() const noexcept {
    static const H256 s_max{"0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141"};
    static const H256 s_zero;

    // 这里忽略v可以为2，3这两种极小概率的可能
    return (v <= 1 && r > s_zero && s > s_zero && r < s_max && s < s_max);
}

/**
 * 计算私钥对应的公钥
 * @param secret 私钥（32字节）
 * @return 对应的公钥（64字节）
 * @throw 若私钥非法则抛出BadPrivateKey异常
 */
Public toPublic(const Secret& secret) {
    // 获取上下文
    auto ctx = getCtx();

    // 从私钥计算出原始公钥（64字节）
    // 原始公钥不能跨平台，跨版本使用，需要用secp256k1_ec_pubkey_serialize进行序列化
    secp256k1_pubkey rawPubkey;
    if (!secp256k1_ec_pubkey_create(ctx, &rawPubkey, secret.data())) {
        throw BadPrivateKey();
    }

    // 将原始公钥进行序列化（65字节！）
    // 多的一个字节用来标识公钥，0x04作为第一个标识字节
    std::array<Byte, 65> serializedPubkey;
    size_t serializedPubkeySize = serializedPubkey.size();
    secp256k1_ec_pubkey_serialize(
        ctx,                        // 上下文
        serializedPubkey.data(),    // 序列化公钥的输出
        &serializedPubkeySize,      // 初始化为输出缓存区的长度，返回实际写入输出缓冲区的长度
        &rawPubkey,                 // 原始公钥
        SECP256K1_EC_UNCOMPRESSED   // 不压缩占用65字节，压缩占用33字节
    );

    // 确保写入了65个字节
    assert(serializedPubkey.size() == serializedPubkeySize);
    // 确保第一个标识字节正确
    assert(0x04 == serializedPubkey[0]);

    return Public(BytesConstRef(&serializedPubkey[1], serializedPubkey.size() - 1));
}

/**
 * 根据签名信息和被签名的数字摘要恢复出签名者公钥
 * @param sig 签名信息
 * @param digest 被签名的数字摘要
 * @return 签名者的公钥
 * @throw 恢复错误抛出BadSignature异常
 */
Public recover(const SignatureStruct& sig, const H256& digest) {
    // 对于secp256k1曲线而言，只有四种recovery id，分别是0，1，2，3
    if (sig.v > 3) {
        throw BadSignature();
    }

    // 获取上下文
    auto ctx = getCtx();

    // 解析出原始签名信息格式
    secp256k1_ecdsa_recoverable_signature rawSig;
    if (!secp256k1_ecdsa_recoverable_signature_parse_compact(ctx, &rawSig, reinterpret_cast<const Byte*>(&sig), sig.v)) {
        throw BadSignature();
    }

    // 解析出原始公钥
    secp256k1_pubkey rawPubkey;
    if (!secp256k1_ecdsa_recover(ctx, &rawPubkey, &rawSig, digest.data())) {
        throw BadSignature();
    }

    // 将原始公钥进行序列化（65字节！）
    // 多的一个字节用来标识公钥，0x04作为第一个标识字节
    std::array<Byte, 65> serializedPubkey;
    size_t serializedPubkeySize = serializedPubkey.size();
    secp256k1_ec_pubkey_serialize(
        ctx,                        // 上下文
        serializedPubkey.data(),    // 序列化公钥的输出
        &serializedPubkeySize,      // 初始化为输出缓存区的长度，返回实际写入输出缓冲区的长度
        &rawPubkey,                 // 原始公钥
        SECP256K1_EC_UNCOMPRESSED   // 不压缩占用65字节，压缩占用33字节
    );

    // 确保写入了65个字节
    assert(serializedPubkey.size() == serializedPubkeySize);
    // 确保第一个标识字节正确
    assert(0x04 == serializedPubkey[0]);

    return Public(BytesConstRef(&serializedPubkey[1], serializedPubkey.size() - 1));
}

static const U256 c_secp256k1n("115792089237316195423570985008687907852837564279074904382605163141518161494337");

/**
 * 用私钥对数字摘要进行签名
 * @param secret 私钥
 * @param digest 数字摘要
 * @return 数字签名
 * @throw 签名错误抛出BadPrivateKey异常
 */
SignatureStruct sign(const Secret& secret, const H256& digest) {
    // 获取上下文
    auto ctx = getCtx();

    // 对数字摘要进行签名，获取原始签名信息
    secp256k1_ecdsa_recoverable_signature rawSig;
    if (!secp256k1_ecdsa_sign_recoverable(ctx, &rawSig, digest.data(), secret.data(), nullptr, nullptr)) {
        throw BadPrivateKey();
    }

    // 将原始签名信息序列化
    SignatureStruct ret;
    int v = 0;
    secp256k1_ecdsa_recoverable_signature_serialize_compact(ctx, reinterpret_cast<Byte*>(&ret), &v, &rawSig);
    ret.v = v;

    // 调整签名信息
    U256 numericS = static_cast<U256>(ret.s);
    if (numericS > c_secp256k1n / 2) {
        ret.v ^= 1;
        ret.s = H256(c_secp256k1n - numericS);
    }
    assert(static_cast<U256>(ret.s) <= c_secp256k1n / 2);

    return ret;
}

/**
 * 验证签名者公钥是否为pub
 * @param pub 指定公钥
 * @param sig 签名信息
 * @param digest 被签名的数字摘要
 * @return 若是返回true，否则返回false
 * @throw 验证错误抛出BadSignature异常
 */
bool verify(const Public& pub, const SignatureStruct& sig, const H256& digest) {
    return pub == recover(sig, digest);
}

}   // namespace dev