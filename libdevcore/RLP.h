// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2013-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * RLP序列化/反序列化
 * @file: RLP.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-30
 */
#pragma once

#include <vector>
#include <string>
#include <utility>
#include <cstddef>
#include "Common.h"
#include "FixedBytes.h"
#include "Exceptions.h"

namespace dev {

/**
 * RLP（Recursive Length Prefix）递归长度前缀编码，是由以太坊提出的序列化/反序列化标准
 * 相比json格式体积更小，直接用二进制编码
 * 相比protobuf又简单很多，编码规则几句话就能说清楚
 */

/**
 * RLP将数据分为两类：
 * 1. 字符串
 * 2. 列表（可以包含字符串和列表）
 * 编码规则：
 * 1. 字符串长度是1，并且值属于[0x00, 0x7f]，那么其RLP编码就是字符串本身
 * 2. 字符串长度是0-55，那么RLP编码为前缀0x80+长度，即[0x80, 0xb7]，后接字符串本身
 * 3. 字符串长度大于55，那么RLP编码为前缀0xb7+长度编码所占字节数（最大8字节），即[0xb8, 0xbf]，后接长度编码+字符串本身
 * 4. 列表长度是0-55，那么RLP编码为前缀0xc0+长度，即[0xc0, 0xf7]，后接列表中各数据项的rlp编码
 * 5. 列表长度大于55，那么RLP编码为前缀0xf7+长度编码所占字节数（最大8字节），即[0xf8, 0xff]，后接长度编码+列表中各数据项rlp编码
 */

/**
 * RLP数值常量
 * IMM表示immediate，即长度编码到前缀中
 * IND表示independent，即长度单独编码
 */
constexpr unsigned c_rlpMaxLengthBytes = 8;         // rlp支持的最大长度编码为8字节（2^64）
constexpr unsigned c_rlpDataImmLenStart = 0x80;     // 字符串将长度编码到前缀中的起始前缀
constexpr unsigned c_rlpDataIndLenZero = 0xb7;      // 字符串将长度单独编码的起始前缀
constexpr unsigned c_rlpListStart = 0xc0;           // 列表的起始前缀
constexpr unsigned c_rlpListIndLenZero = 0xf7;      // 列表将长度单独编码的起始前缀
constexpr unsigned c_rlpDataImmLenCount = 56;       // 字符串可以将长度编码到前缀的范围[0, 55]
constexpr unsigned c_rlpListImmLenCount = 56;       // 列表可以将长度编码到前缀的范围[0, 55]

// 整型类型的长度
template <typename T>
struct intTraits {
    static constexpr unsigned c_maxSize = sizeof(T);
};
template <>
struct intTraits<U160> {
    static constexpr unsigned c_maxSize = 20;
};
template <>
struct intTraits<U256> {
    static constexpr unsigned c_maxSize = 32;
};
template <>
struct intTraits<U512> {
    static constexpr unsigned c_maxSize = 64;
};

// RLP负责反序列化，一个RLP对象表示一个RLP数据项
class RLP {
public:
    /**
     * 将字节数组看作一个RLP数据项
     * @param bs 字节数组
     * @param failIfTooBig 当输入长度大于RLP数据项本身的长度时，是否抛出异常
     * @throw 若当前RLP数据项不合法抛出BadRLP异常
     * @throw failIfTooBig为true，输入长度大于RLP数据项本身的长度时，抛出BadRLP异常
     */
    explicit RLP(BytesConstRef bs, bool failIfTooBig = true);

    // 判断当前RLP数据项是否为空（不存在任何数据）
    bool isNull() const noexcept { return 0 == m_data.size(); }

    // 判断当前RLP数据项是否是空字符串或空列表
    bool isEmpty() const noexcept { return !isNull() && (c_rlpDataImmLenStart == m_data[0] || c_rlpListStart == m_data[0]); }
    bool isEmptyData() const noexcept { return !isNull() && c_rlpDataImmLenStart == m_data[0]; }
    bool isEmptyList() const noexcept { return !isNull() && c_rlpListStart == m_data[0]; }

    // 判断当前RLP数据项是字符串还是列表
    bool isData() const noexcept { return !isNull() && m_data[0] < c_rlpListStart; }
    bool isList() const noexcept { return !isNull() && m_data[0] >= c_rlpListStart; }

    /**
     * 将当前RLP数据项转换为无符号整型
     * @return 对应的无符号整型
     * @throw 若当前RLP数据项不是字符串抛出RLPBadCast异常
     * @throw 若包含前导0或数值溢出抛出RLPBadCast异常
     */
    template<typename T>
    T toInt() const {
        static_assert(
            std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed,
            "only unsigned types supported"
        );

        if (!isData()) {
            throw RLPBadCast();
        }

        auto p = payload();
        if (p.size() > 0 && 0 == p[0]) {
            // 包含前导0
            throw RLPBadCast();
        }
        if (p.size() > intTraits<T>::c_maxSize) {
            // 数值溢出
            throw RLPBadCast();
        }
        return fromBigEndian<T>(p);
    }

    /**
     * 将当前RLP数据项转换为字节数组
     * @return 对应的字节数组
     * @throw 若当前RLP数据项不是字符串抛出RLPBadCast异常
     */
    Bytes toBytes() const {
        if (!isData()) {
            throw RLPBadCast();
        }

        // 获取字符串本身
        auto p = payload();
        return Bytes(p.data(), p.data() + p.size());
    }

    /**
     * 将当前RLP数据项转换为定长字节数组
     * @param align 对齐方式
     * @return 对应的定长字节数组
     * @throw 若当前RLP数据项不是字符串抛出RLPBadCast异常
     */
    template<typename T>
    T toFixedBytes(unsigned align = Align::c_failIfTooSmall | Align::c_failIfTooBig) const {
        if (!isData()) {
            throw RLPBadCast();
        }

        return T(payload(), align);
    }

    /**
     * 将当前RLP数据项转换为字符串
     * @return 对应的字符串
     * @throw 若当前RLP数据项不是字符串抛出RLPBadCast异常
     */
    std::string toString() const {
        if (!isData()) {
            throw RLPBadCast();
        }

        // 获取字符串本身
        return payload().toString();
    }

    // 转换为对应类型（对不支持的类型直接抛出RLPUnsupportedCast异常）
    template <typename T>
    T convert() const { throw RLPUnsupportedCast(); }

    // 获取当前RLP数据项的实际数据与长度（前缀+长度编码+载荷）
    BytesConstRef actualData() const noexcept { return m_data; }
    size_t actualSize() const noexcept { return m_data.size(); }

    // 获取当前RLP数据项的数据载荷
    BytesConstRef payload() const { return m_data.cropped(prefixSize(), payloadSize()); }

    /**
     * 分解列表
     * @return 列表包含的所有RLP数据项
     * @throw 若当前RLP不是列表则抛出RLPBadCast异常
     * @throw 若列表中包含的任何RLP数据项不合法抛出BadRLP异常
     */
    std::vector<RLP> splitList() const;

private:
    // 获取当前RLP数据项前缀长度（前缀+长度编码所占字节数）
    unsigned prefixSize() const noexcept;

    /**
     * 获取当前RLP数据项数据载荷长度
     * @throw 若当前RLP数据项不合法抛出BadRLP异常
     */
    size_t payloadSize() const;

    // 表示当前RLP数据项（需注意引用数据的生命周期）
    BytesConstRef m_data;
};

template <>
inline uint32_t RLP::convert() const { return toInt<uint32_t>(); }  // 转换为uint32_t类型
template <>
inline uint64_t RLP::convert() const { return toInt<uint64_t>(); }  // 转换为uint64_t类型
template <>
inline U160 RLP::convert() const { return toInt<U160>(); }          // 转换为U160类型
template <>
inline U256 RLP::convert() const { return toInt<U256>(); }          // 转换为U256类型
template <>
inline U512 RLP::convert() const { return toInt<U512>(); }          // 转换为U512类型
template <>
inline H160 RLP::convert() const { return toFixedBytes<H160>(); }   // 转换为H160类型
template <>
inline H256 RLP::convert() const { return toFixedBytes<H256>(); }   // 转换为H256类型
template <>
inline H512 RLP::convert() const { return toFixedBytes<H512>(); }   // 转换为H512类型
template <>
inline H2048 RLP::convert() const { return toFixedBytes<H2048>(); } // 转换为H2048类型
template <>
inline Bytes RLP::convert() const { return toBytes(); }             // 转换为Bytes类型
template <>
inline std::string RLP::convert() const { return toString(); }      // 转换为std::string类型

// RLPStream类负责序列化
class RLPStream {
public:
    // 默认构造空的RLP编码流
    RLPStream() = default;

    /**
     * 开启新的列表追加流程
     * @param itemCount 新列表包含的数据项数目（若为0，直接完成空列表编码）
     */
    RLPStream(size_t itemCount) { appendList(itemCount); }

    /**
     * 开启新的列表追加流程
     * @param itemCount 新列表包含的数据项数目（若为0，直接完成空列表编码）
     * @return RLP编码流对象的引用
     */
    RLPStream& appendList(size_t itemCount);

    /**
     * 追加无符号整型到RLP编码，忽略前导零
     * @param u 无符号整型
     * @return RLP编码流对象的引用
     * @throw 若长度编码大于8抛出RLPItemTooLarge异常
     */
    template<typename T>
    RLPStream& appendInt(const T& u) {
        static_assert(
            std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed,
            "only unsigned types supported"
        );

        if (!u) {
            // 字符串的长度为0，那么直接写入长度为0的编码
            m_out.push_back(c_rlpDataImmLenStart);
        } else if (u < c_rlpDataImmLenStart) {
            // 字符串长度是1，并且值属于[0x00, 0x7f]，那么其RLP编码就是其本身
            m_out.push_back(static_cast<Byte>(u));
        } else {
            // 计算整型数值的长度
            auto br = bytesRequired(u);

            if (br < c_rlpDataImmLenCount) {
                // 字符串长度是0-55，那么长度编码到前缀，即[0x80, 0xb7]，后面接字符串本身
                m_out.push_back(br + c_rlpDataImmLenStart);
            } else {
                // 字符串长度大于55，那么RLP编码为前缀0xb7+长度编码所占字节数（最大8字节），即[0xb8, 0xbf]，后接长度编码+字符串本身
                pushCount(br, c_rlpDataIndLenZero);
            }
            // 写入字符串本身
            size_t oldSize = m_out.size();
            m_out.resize(oldSize + br);
            toBigEndian(u, BytesRef(&m_out[oldSize], br));
        }
        noteAppended();
        return *this;
    }
    RLPStream& append(const U512& n) { return appendInt(n); }
    RLPStream& append(const U256& n) { return appendInt(n); }
    RLPStream& append(const U160& n) { return appendInt(n); }
    RLPStream& append(uint64_t n) { return appendInt(n); }
    RLPStream& append(uint32_t n) { return appendInt(n); }

    /**
     * 追加字节数组到RLP编码
     * @param bs 字节数组
     * @param ignoreLeading0s 是否忽略前导零
     * @return RLP编码流对象的引用
     * @throw 若长度编码大于8抛出RLPItemTooLarge异常
     */
    RLPStream& append(BytesConstRef bs, bool ignoreLeading0s = false);

    // 追加rlp数据项
    RLPStream& append(RLP item) { return appendRaw(item.actualData()); }

    // 重载流操作符
    template <typename T>
    RLPStream& operator<<(T&& data) { return append(std::forward<T>(data)); }

    /**
     * 查看当前编码结果
     * @return 当前编码结果的常量引用
     * @throw 若当前还有列表未完成编码抛出RLPIncompleteList异常
     */
    const Bytes& peek();

    /**
     * 取走当前编码结果
     * @return 当前编码结果
     * @throw 若当前还有列表未完成编码抛出RLPIncompleteList异常
     */
    Bytes take();

private:
    /**
     * 直接追加原始数据项
     * @param rawItem 原始数据项
     * @return RLP编码流对象的引用
     * @throw 若造成当前列表长度编码大于8字节抛出RLPItemTooLarge异常
     */
    RLPStream& appendRaw(BytesConstRef rawItem);

    /**
     * 通知追加了新的数据项（以便列表出栈）
     * @throw 若造成当前列表长度编码大于8字节抛出RLPItemTooLarge异常
     */
    void noteAppended();

    /**
     * 追加字符串/列表的前缀+长度单独编码
     * @param count 字符串或列表的长度（大于55）
     * @param offset 前缀起始偏移值
     * @throw 若长度编码大于8抛出RLPItemTooLarge异常
     */
    void pushCount(size_t count, unsigned offset);

    // 编码结果
    Bytes m_out;

    // 嵌套的列表需要递归，这里用手动压栈的方式实现
    // top.first记录当前列表还剩多少个数据项待添加
    // top.second记录当前列表的存储起始位置
    std::vector<std::pair<size_t, size_t>> m_listStack;
};

// 计算单个字符串的RLP编码
template <typename T>
Bytes rlpData(T&& data) { return (RLPStream() << std::forward<T>(data)).take(); }

// 计算列表的RLP编码（空列表）
inline Bytes rlpList() { return RLPStream(0).take(); }

template <typename T>
void rlpListAux(RLPStream& out, T&& data) { out << std::forward<T>(data); }
template <typename T, typename... Ts>
void rlpListAux(RLPStream& out, T&& data, Ts&&... list) { rlpListAux(out << std::forward<T>(data), std::forward<Ts>(list)...); }

// 计算列表的RLP编码
template <typename... Ts>
Bytes rlpList(Ts&&... list) {
    RLPStream ret(sizeof...(Ts));
    rlpListAux(ret, std::forward<Ts>(list)...);
    return ret.take();
}

// 空字符串rlp编码
extern const Bytes c_rlpEmptyData;

// 空列表rlp编码
extern const Bytes c_rlpEmptyList;

}   // namespace dev