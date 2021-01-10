// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 常用类型，函数定义
 * @file: Common.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-03
 */
#pragma once

#include <vector>
#include <string>
#include <limits>
#include <chrono>
#include <cstdint>
#include <boost/multiprecision/cpp_int.hpp>
#include "vector_ref.h"
#include "Exceptions.h"

namespace dev {

//------------------------------------类型定义------------------------------------//
// 字节
using Byte = uint8_t;

// 字节数组
using Bytes = std::vector<Byte>;

// 字节数组引用
using BytesRef = vector_ref<Byte>;

// 字节数组常量引用
using BytesConstRef = vector_ref<const Byte>;

/**
 * cpp_int_backend<MinBits=0, MaxBits=0, SignType=signed, Checked=unchecked, Allocator=std::allocator<limb_type>>
 * @param MinBits 底层整型占用的最少位宽
 * @param MaxBits 底层整型占用的最大位宽
 * @param SignType 有符号整型还是无符号整型（任意精度的整型只能为signed！）
 * @param Checked 当发生数值溢出，从字符串转换失败，对负数进行位运算是否抛出异常
 * @param Allocator 内存分配器，当MinBits==MaxBits时，定义为void，表示不需要动态内存分配
 */

/**
 * cpp_int中无符号型用二进制补码表示，有符号型用原码表示，并且额外用一位来表示符号
 * 不允许任意精度的无符号整型也是因为二进制补码表示需要基于定长的精度
 */

// 任意精度有符号整型
using BigInt = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<>>;

// unsigned int160
using U160 = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<160, 160,
    boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;
// signed int160
using S160 = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<160, 160,
    boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void>>;
// unsigned int256
using U256 = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<256, 256,
    boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;
// signed int256
using S256 = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<256, 256,
    boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void>>;
// unsigned int512
using U512 = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<512, 512,
    boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;
// signed int512
using S512 = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<512, 512,
    boost::multiprecision::signed_magnitude, boost::multiprecision::unchecked, void>>;

//------------------------------------编解码------------------------------------//
/**
 * 忽略前导0，计算无符号整型占用多少字节内存
 * @param u 无符号整型
 * @return 占用多少字节内存（可以为0）
 */
template<typename T>
unsigned bytesRequired(T u) noexcept {
    static_assert(
        std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed,
        "only unsigned types supported"
    );

    unsigned br = 0;
    while (u) {
        ++br;
        u >>= 8;
    }
    return br;
}

/**
 * 将无符号整型序列化为字节数组（大端序）
 * @param u 无符号整型
 * @param bs 用于保存结果的字节数组引用
 */
template<typename T>
void toBigEndian(T u, BytesRef bs) noexcept {
    static_assert(
        std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed,
        "only unsigned types supported"
    );

    for (size_t i = bs.size(); i > 0; --i) {
        bs[i - 1] = static_cast<Byte>(u);
        u >>= 8;
    }
}

/**
 * 将字节数组反序列化为无符号整型（大端序）
 * @param bs 大端序字节数组常量引用
 * @return 主机序的整型
 */
template<typename T>
T fromBigEndian(BytesConstRef bs) {
    static_assert(
        std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed,
        "only unsigned types supported"
    );

    T ret = 0;
    for (auto b : bs) {
        ret <<= 8;
        ret |= b;
    }
    return ret;
}

//------------------------------------数值计算------------------------------------//
/**
 * 将补码表示的256位无符号数转换为原码表示的256位有符号数
 * @param u 补码表示的256位无符号数
 * @return 对应的原码表示的256位有符号数
 */
inline S256 u2s(U256 u) {
    static const BigInt c_end = BigInt(1) << 256;
    // get the +/- symbols
    return boost::multiprecision::bit_test(u, 255) ? S256(-(c_end - u)) : S256(u);
}

/**
 * 将原码表示的256位有符号数转换为补码表示的256位无符号数
 * @param s 原码表示的256位有符号数
 * @return 对应的补码表示的256位无符号数
 */
inline U256 s2u(S256 s) {
    return static_cast<U256>(s);
}

//------------------------------------日期&时间------------------------------------//
// 简易计时器
class Timer {
public:
    // 开始计时
    Timer() noexcept { restart(); }

    // 流逝的时间（ms）
    uint64_t elapsed() const noexcept {
        auto duration = std::chrono::steady_clock::now() - m_start;
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }

    // 重新开始计时
    void restart() noexcept { m_start = std::chrono::steady_clock::now(); }

private:
    std::chrono::steady_clock::time_point m_start;
};

}   // namespace dev