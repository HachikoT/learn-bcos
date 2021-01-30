// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2013-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * RLP序列化/反序列化
 * @file: RLP.cpp
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-30
 */
#include "RLP.h"
#include <cstring>
#include <cassert>

namespace dev {

/**
 * 将字节数组看作一个RLP数据项
 * @param bs 字节数组
 * @param failIfTooBig 当输入长度大于RLP数据项本身的长度时，是否抛出异常
 * @throw 若当前RLP数据项不合法抛出BadRLP异常
 * @throw failIfTooBig为true，输入长度大于RLP数据项本身的长度时，抛出BadRLP异常
 */
RLP::RLP(BytesConstRef bs, bool failIfTooBig) : m_data(bs) {
    size_t s = prefixSize() + payloadSize();
    if (failIfTooBig && m_data.size() > s) {
        throw BadRLP();
    }
    m_data = m_data.cropped(0, s);
}

/**
 * 分解列表
 * @return 列表包含的所有RLP数据项
 * @throw 若当前RLP不是列表则抛出RLPBadCast异常
 * @throw 若列表中包含的任何RLP数据项不合法抛出BadRLP异常
 */
std::vector<RLP> RLP::splitList() const {
    if (!isList()) {
        throw RLPBadCast();
    }

    std::vector<RLP> ret;
    BytesConstRef leftItems = payload();
    while (leftItems.size() != 0) {
        RLP item = RLP(leftItems, false);
        leftItems = leftItems.cropped(item.actualSize());
        ret.push_back(item);
    }

    return ret;
}

// 获取当前RLP数据项前缀长度（前缀+长度编码所占字节数）
unsigned RLP::prefixSize() const noexcept {
    // 当前RLP数据项为空或单字节RLP编码
    if (isNull() || m_data[0] < c_rlpDataImmLenStart) {
        return 0;
    }

    // 当前RLP数据项是字符串且长度单独编码
    if (isData() && m_data[0] > c_rlpDataIndLenZero) {
        return 1 + (m_data[0] - c_rlpDataIndLenZero);
    }

    // 当前RLP数据项是列表且长度单独编码
    if (isList() && m_data[0] > c_rlpListIndLenZero) {
        return 1 + (m_data[0] - c_rlpListIndLenZero);
    }

    // 长度编码到前缀
    return 1;
}

/**
 * 获取当前RLP数据项数据载荷长度
 * @throw 若当前RLP数据项不合法抛出BadRLP异常
 */
size_t RLP::payloadSize() const {
    if (isNull()) {                                     // 没有数据
        return 0;
    } else if (m_data[0] < c_rlpDataImmLenStart) {      // 单字节字符串
        return 1;
    } else if (m_data[0] <= c_rlpDataIndLenZero) {      // 长度编码到前缀的字符串
        size_t len = m_data[0] - c_rlpDataImmLenStart;
        if (
            len >= m_data.size() ||
            (c_rlpDataImmLenStart + 1 == m_data[0] && m_data[1] < c_rlpDataImmLenStart)
        ) {
            // RLP数据项不完整
            // 长度为1的字符串，但其实应该编码为单字节RLP编码
            throw BadRLP();
        }
        return len;
    } else if (m_data[0] < c_rlpListStart) {            // 长度单独编码的字符串
        size_t lenSize = m_data[0] - c_rlpDataIndLenZero;
        if (lenSize >= m_data.size() || 0 == m_data[1]) {
            // RLP数据项不完整
            // RLP长度编码不能有前导0
            throw BadRLP();
        }
        // 反序列化出长度编码
        auto dataSize = fromBigEndian<uint64_t>(m_data.cropped(1, lenSize));
        if (dataSize < c_rlpDataImmLenCount || dataSize >= m_data.size() - lenSize) {
            // 单独编码的长度必须大于55
            // RLP数据项不完整
            throw BadRLP();
        }
        return dataSize;
    } else if (m_data[0] < c_rlpListIndLenZero) {       // 长度编码到前缀的列表
        // 长度为[0, 55]的列表，长度编码到前缀中
        size_t len = m_data[0] - c_rlpListStart;
        if (len >= m_data.size()) {
            // RLP数据项不完整
            throw BadRLP();
        }
        return len;
    } else {                                            // 长度单独编码的列表
        // 长度大于55的列表，长度单独编码
        size_t lenSize = m_data[0] - c_rlpListIndLenZero;
        if (lenSize >= m_data.size() || 0 == m_data[1]) {
            // RLP数据项不完整
            // RLP长度编码不能有前导0
            throw BadRLP();
        }
        // 反序列化出长度编码
        auto dataSize = fromBigEndian<uint64_t>(m_data.cropped(1, lenSize));
        if (dataSize < c_rlpListImmLenCount || dataSize >= m_data.size() - lenSize) {
            // 单独编码的长度必须大于55
            // RLP数据项不完整
            throw BadRLP();
        }
        return dataSize;
    }
}

/**
 * 开启新的列表追加流程
 * @param itemCount 新列表包含的数据项数目（若为0，直接完成空列表编码）
 * @return RLP编码流对象的引用
 */
RLPStream& RLPStream::appendList(size_t itemCount) {
    if (0 != itemCount) {
        // 将新列表包含的RLP数据项数目，和当前RLP编码流长度压入栈
        m_listStack.push_back(std::make_pair(itemCount, m_out.size()));
    } else {
        // 直接完成空列表的追加
        m_out.push_back(c_rlpListStart);
    }

    return *this;
}

/**
 * 追加字节数组到RLP编码
 * @param bs 字节数组
 * @param ignoreLeading0s 是否忽略前导零
 * @return RLP编码流对象的引用
 * @throw 若长度编码大于8抛出RLPItemTooLarge异常
 */
RLPStream& RLPStream::append(BytesConstRef bs, bool ignoreLeading0s) {
    auto size = bs.size();
    auto data = bs.data();
    if (ignoreLeading0s) {
        while (0 != size && 0 == *data) {
            // 忽略前导0
            --size;
            ++data;
        }
    }

    if (1 == size && data[0] < c_rlpDataImmLenStart) {
        // 字符串长度是1，并且值属于[0x00, 0x7f]，那么其RLP编码就是其本身
        m_out.push_back(data[0]);
        noteAppended();
    } else {
        if (size < c_rlpDataImmLenCount) {
            // 字符串长度是[0, 55]，那么长度编码到前缀，即[0x80, 0xb7]，后面接字符串本身
            m_out.push_back(size + c_rlpDataImmLenStart);
        } else {
            // 字符串长度大于55，那么RLP编码为前缀0xb7+长度编码所占字节数（最大8字节），即[0xb8, 0xbf]，后接长度编码+字符串本身
            pushCount(size, c_rlpDataIndLenZero);
        }
        // 写入字符串本身
        appendRaw(BytesConstRef(data, size));
    }

    return *this;
}

/**
 * 查看当前编码结果
 * @return 当前编码结果的常量引用
 * @throw 若当前还有列表未完成编码抛出RLPIncompleteList异常
 */
const Bytes& RLPStream::peek() {
    if (!m_listStack.empty()) {
        throw RLPIncompleteList();
    }

    // 获取当前编码结果
    return m_out;
}

/**
 * 取走当前编码结果
 * @return 当前编码结果
 * @throw 若当前还有列表未完成编码抛出RLPIncompleteList异常
 */
Bytes RLPStream::take() {
    if (!m_listStack.empty()) {
        throw RLPIncompleteList();
    }

    // 取走编码结果
    return std::move(m_out);
}

/**
 * 直接追加原始数据项
 * @param rawItem 原始数据项
 * @return RLP编码流对象的引用
 * @throw 若造成当前列表长度编码大于8字节抛出RLPItemTooLarge异常
 */
RLPStream& RLPStream::appendRaw(BytesConstRef rawItem) {
    // 直接追加数据
    m_out.insert(m_out.end(), rawItem.begin(), rawItem.end());

    // 通知追加了新的数据项
    noteAppended();

    return *this;
}

/**
 * 通知追加了新的数据项（以便列表出栈）
 * @throw 若造成当前列表长度编码大于8字节抛出RLPItemTooLarge异常
 */
void RLPStream::noteAppended() {
    // 进行列表出栈操作
    while (!m_listStack.empty()) {
        // 当前列表
        auto& top = m_listStack.back();

        top.first--;
        if (0 != top.first) {
            // 当前列表还没编码完，不做任何事
            return;
        } else {
            // 当前列表已经编码完，执行出栈操作
            // 当前列表编码的起始位置
            auto start = top.second;

            // 出栈当前列表
            m_listStack.pop_back();

            // 当前列表总的长度
            size_t count = m_out.size() - start;

            // 判断长度是否单独编码
            if (count < c_rlpListImmLenCount) {
                // 腾出空间放前缀和长度编码（这里用memmove是因为源和目的重叠了）
                m_out.resize(m_out.size() + 1);
                memmove(m_out.data() + start + 1, m_out.data() + start, count);
                // 长度在0-55，直接将长度加到前缀就行
                m_out[start] = c_rlpListStart + count;
            } else {
                // 判断长度编码是否大于8
                auto br = bytesRequired(count);
                if (sizeof(size_t) > c_rlpMaxLengthBytes && br > c_rlpMaxLengthBytes) {
                    throw RLPItemTooLarge();
                }
                // 腾出空间放前缀和长度编码
                m_out.resize(m_out.size() + 1 + br);
                memmove(m_out.data() + start + 1 + br, m_out.data() + start, count);
                // 长度大于55，需要将长度单独编码
                m_out[start] = c_rlpListIndLenZero + br;
                toBigEndian(count, BytesRef(&m_out[start + 1], br));
            }
        }
        // 当前列表完成编码之后，当前列表也被看做新添加的一个数据项，再次尝试列表出栈操作
    }
}

/**
 * 追加字符串/列表的前缀+长度单独编码
 * @param count 字符串或列表的长度（大于55）
 * @param offset 前缀起始偏移值
 * @throw 若长度编码大于8抛出RLPItemTooLarge异常
 */
void RLPStream::pushCount(size_t count, unsigned offset) {
    // count和offset必须满足编码规范
    assert(
        (c_rlpDataIndLenZero == offset && count >= c_rlpDataImmLenCount) ||
        (c_rlpListIndLenZero == offset && count >= c_rlpListImmLenCount)
    );

    // 判断长度编码是否大于8
    auto br = bytesRequired(count);
    if (sizeof(size_t) > c_rlpMaxLengthBytes && br > c_rlpMaxLengthBytes) {
        throw RLPItemTooLarge();
    }

    // 写入前缀
    m_out.push_back(offset + br);

    // 写入长度编码
    size_t oldSize = m_out.size();
    m_out.resize(oldSize + br);
    toBigEndian(count, BytesRef(&m_out[oldSize], br));
}

// 空字符串rlp编码
const Bytes c_rlpEmptyData = rlpData("");;

// 空列表rlp编码
const Bytes c_rlpEmptyList = rlpList();

}   // namespace dev