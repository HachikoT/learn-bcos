// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 定长字节数组类型
 * @file: FixedBytes.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-24
 */
#pragma once

#include <array>
#include <vector>
#include <string>
#include <random>
#include <cstring>
#include <boost/functional/hash.hpp>
#include "Common.h"
#include "Hex.h"

namespace dev {

// 真随机数生成引擎，linux上通过读取/dev/urandom设备实现
extern thread_local std::random_device t_randomEngine;

// 对齐选项
struct Align {
    static constexpr unsigned c_right = 0;
    static constexpr unsigned c_left = 1;
    static constexpr unsigned c_failIfTooSmall = 2;
    static constexpr unsigned c_failIfTooBig = 4;
};

template<size_t N>
class FixedBytes {
public:
    // 值类型
    using value_type = typename std::array<Byte, N>::value_type;

    // 指针类型
    using pointer = typename std::array<Byte, N>::pointer;
    using const_pointer = typename std::array<Byte, N>::const_pointer;

    // 引用类型
    using reference = typename std::array<Byte, N>::reference;
    using const_reference = typename std::array<Byte, N>::const_reference;

    // 迭代器类型
    using iterator = typename std::array<Byte, N>::iterator;
    using const_iterator = typename std::array<Byte, N>::const_iterator;

    // 对应的无符号整型（U8*N）
    using Arith = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<N * 8, N * 8,
        boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;

    // 默认构造0值定长字节数组
    FixedBytes() noexcept { m_data.fill(0); }

    /**
     * 通过字节数组构造
     * @param bs 字节数组
     * @param align 对齐选项
     * @throw 若设置了c_alignFailIfTooSmall且bs长度小于N抛出Unaligned异常
     * @throw 若设置了c_alignFailIfTooBig且bs长度大于N抛出Unaligned异常
     */
    FixedBytes(BytesConstRef bs, unsigned align = Align::c_failIfTooSmall | Align::c_failIfTooBig) {
        if (N == bs.size()) {
            memcpy(m_data.data(), bs.data(), N);
        } else if (N > bs.size()) {
            // 输入比N小
            if (align & Align::c_failIfTooSmall) {
                throw Unaligned();
            }
            m_data.fill(0);
            memcpy(align & Align::c_left ? m_data.data() : m_data.data() + N - bs.size(), bs.data(), bs.size());
        } else {
            // 输入比N大
            if (align & Align::c_failIfTooBig) {
                throw Unaligned();
            }
            memcpy(m_data.data(), align & Align::c_left ? bs.data() : bs.data() + bs.size() - N, N);
        }
    }

    // 通过16进制字符串构造
    FixedBytes(const std::string& hexStr, unsigned align = Align::c_failIfTooSmall | Align::c_failIfTooBig)
    : FixedBytes(fromHex(hexStr), align) {}
    FixedBytes(const char* hexStr, unsigned align = Align::c_failIfTooSmall | Align::c_failIfTooBig)
    : FixedBytes(fromHex(hexStr), align) {}

    // 通过算术类型构造
    explicit FixedBytes(const Arith& arith) noexcept { toBigEndian(arith, m_data); }

    // 转换为算术类型
    Arith toArith() const { return fromBigEndian<Arith>(m_data); }

    // 判断是否为非0值定长字节数组
    explicit operator bool() const noexcept { return *this != FixedBytes(); }

    // 获取长度（单位：字节）
    constexpr size_t size() const noexcept { return N; }

    // 布尔运算
    bool operator==(const FixedBytes& rhs) const noexcept { return m_data == rhs.m_data; }
    bool operator!=(const FixedBytes& rhs) const noexcept { return !(*this == rhs); }
    bool operator<(const FixedBytes& rhs) const noexcept { return m_data < rhs.m_data; }
    bool operator<=(const FixedBytes& rhs) const noexcept { return m_data <= rhs.m_data; }
    bool operator>(const FixedBytes& rhs) const noexcept { return m_data > rhs.m_data; }
    bool operator>=(const FixedBytes& rhs) const noexcept { return m_data >= rhs.m_data; }

    // 位运算
    FixedBytes operator~() const noexcept { FixedBytes ret; for (size_t i = 0; i < N; ++i) ret[i] = ~m_data[i]; return ret; }
    FixedBytes& operator&=(const FixedBytes& c) noexcept { for (size_t i = 0; i < N; ++i) m_data[i] &= c.m_data[i]; return *this; }
    FixedBytes operator&(const FixedBytes& c) const noexcept { return FixedBytes(*this) &= c; }
    FixedBytes& operator|=(const FixedBytes& c) noexcept { for (size_t i = 0; i < N; ++i) m_data[i] |= c.m_data[i]; return *this; }
    FixedBytes operator|(const FixedBytes& c) const noexcept { return FixedBytes(*this) |= c; }
    FixedBytes& operator^=(const FixedBytes& c) noexcept { for (size_t i = 0; i < N; ++i) m_data[i] ^= c.m_data[i]; return *this; }
    FixedBytes operator^(const FixedBytes& c) const noexcept { return FixedBytes(*this) ^= c; }

    // 求定长的哈希值，用于存放到基于hash table的标准容器中（std::unordered_xxx）
    // 标准库并没有为自定义类型提供哈希函数，这里引用boost中的哈希函数，散列性比较好
    struct hash {
        size_t operator()(const FixedBytes& value) const {
            return boost::hash_range(value.begin(), value.end());
        }
    };

    // 获取指针
    pointer data() noexcept { return m_data.data(); }
    const_pointer data() const noexcept { return m_data.data(); }

    // 获取引用
    reference operator[](size_t idx) noexcept { return m_data[idx]; }
    const_reference operator[](size_t idx) const noexcept { return m_data[idx]; }

    // 获取迭代器
    iterator begin() noexcept { return m_data.begin(); }
    iterator end() noexcept { return m_data.end(); }
    const_iterator begin() const noexcept { return m_data.begin(); }
    const_iterator end() const noexcept { return m_data.end(); }

    // 转换为字节数组引用
    BytesRef ref() noexcept { return m_data; }
    BytesConstRef ref() const noexcept { return m_data; }

    // 将该定长字节数组转换为16进制字符串
    std::string hex() const { return toHex(ref()); }
    std::string hex0x() const { return toHex0x(ref()); }

    // 将该定长字节数组转换为缩减版的16进制字符串（小写，不带前缀0x，最多只显示前8个字符）
    std::string abridged() const { return toHex(ref().cropped(0, 4)) + "..."; }

    // 用真随机数填充每一个字节
    FixedBytes& randomize() {
        // 0-255均匀分布
        std::uniform_int_distribution<int> dis(0, 255);
        for (auto& b : m_data) {
            b = dis(t_randomEngine);
        }
        return *this;
    }

    // 生成随机定长字节数组对象
    static FixedBytes random() { return FixedBytes().randomize(); }

private:
    // 定长字节数组
    std::array<Byte, N> m_data;
};

// 常用的定长哈希值类型
using H160 = FixedBytes<20>;
using H256 = FixedBytes<32>;
using H512 = FixedBytes<64>;
using H2048 = FixedBytes<256>;
using H160s = std::vector<H160>;
using H256s = std::vector<H256>;
using H512s = std::vector<H512>;
using H2048s = std::vector<H2048>;

// 用H256中后160位用来填充H160
inline H160 right160(const H256& h256) noexcept {
    H160 ret;
    memcpy(ret.data(), h256.data() + 12, 20);
    return ret;
}

// 优化布尔运算性能
template <>
inline bool H256::operator==(const H256& rhs) const noexcept {
    auto hash1 = reinterpret_cast<const uint64_t*>(data());
    auto hash2 = reinterpret_cast<const uint64_t*>(rhs.data());
    return (hash1[0] == hash2[0]) && (hash1[1] == hash2[1]) &&
           (hash1[2] == hash2[2]) && (hash1[3] == hash2[3]);
}

// 优化hash运算性能
template <>
inline size_t H160::hash::operator()(const H160& value) const {
    auto data = reinterpret_cast<const uint32_t*>(value.data());
    return boost::hash_range(data, data + 5);
}
template <>
inline size_t H256::hash::operator()(const H256& value) const {
    auto data = reinterpret_cast<const uint64_t*>(value.data());
    return boost::hash_range(data, data + 4);
}
template <>
inline size_t H512::hash::operator()(const H512& value) const {
    auto data = reinterpret_cast<const uint64_t*>(value.data());
    return boost::hash_range(data, data + 8);
}
template <>
inline size_t H2048::hash::operator()(const H2048& value) const {
    auto data = reinterpret_cast<const uint64_t*>(value.data());
    return boost::hash_range(data, data + 32);
}

// Stream I/O for the FixedBytes<N> class.
// 主要便于日志可以直接输出FixedBytes<N>对象
template <size_t N>
inline std::ostream& operator<<(std::ostream& out, const FixedBytes<N>& h) {
    out << h.hex();
    return out;
}

}   // namespace dev

namespace std {

// 在std空间实例化std::hash<HXXX>类型，这样使用std::unordered_xxx时不用手动指定哈希函数
template <>
struct hash<dev::H160> : dev::H160::hash {};
template <>
struct hash<dev::H256> : dev::H256::hash {};
template <>
struct hash<dev::H512> : dev::H512::hash {};
template <>
struct hash<dev::H2048> : dev::H2048::hash {};

}   // namespace std