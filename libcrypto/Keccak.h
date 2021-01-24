/* ethash: C/C++ implementation of Ethash, the Ethereum Proof of Work algorithm.
 * Copyright 2018-2019 Pawel Bylica.
 * Licensed under the Apache License, Version 2.0.
 */

/**
 * keccak哈希函数
 * @file: keccak.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-24
 */
#pragma once

#include <libdevcore/Common.h>
#include <libdevcore/FixedBytes.h>

namespace dev {

/**
 * SHA-3由keccak标准化而来，在很多场合下keccak和SHA-3是同义词，但在2015年8月SHA-3最终完成标准化时，NIST调整了填充算法：
 * SHA3-256(M) = KECCAK [512] (M || 01, 256)
 * 以太坊在开发的时候SHA-3还在标准化中，所以采用了keccak。
 */

/**
 * 推出SHA-3不是因为SHA-2出现了漏洞，只是当时学术界对于SHA-1被成功碰撞的担忧，但目前基于NIST的建议
 * SHA-2和SHA-3都是属于可以安全商用的哈希算法，SHA-3相当于多了一种安全选择
 * 比特币选用的就是SHA-2（SHA256）
 */

// 计算字节数组的keccak256哈希值
H256 keccak256(BytesConstRef src) noexcept;

}   // namespace dev