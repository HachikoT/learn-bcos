// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 账户地址类型
 * @file: Address.cpp
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-24
 */
#include "Address.h"
#include <libcrypto/Keccak.h>
#include <libdevcore/RLP.h>

namespace dev { namespace eth {

// 计算外部账户地址
Address toAddress(const PubKey& pub) noexcept {
    return right160(keccak256(pub));
}

// 计算合约账户地址
Address toAddress(const Address& sender, const U256& nonce) {
    return right160(keccak256(rlpList(sender, nonce)));
}

}}   // namespace dev::eth