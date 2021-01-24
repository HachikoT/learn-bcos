// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * POD类型数组引用
 * @file: vector_ref.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-03
 */
#pragma once

#include <algorithm>
#include <string>
#include <type_traits>
#include <cstddef>
#include <cstring>
#include "Exceptions.h"

namespace dev {

template<typename T>
class vector_ref {
public:
    // 只支持POD类型
    static_assert(
        std::is_pod<T>::value,
        "vector_ref can only be used with PODs due to its low-level treatment of data."
    );

    using value_type = T;               // 值类型
    using pointer = value_type*;        // 指针类型
    using reference = value_type&;      // 引用类型
    using iterator = value_type*;       // 迭代器类型

    // 默认构造函数
    vector_ref() = default;

    // 通过指针构造
    vector_ref(pointer data, size_t size) noexcept : m_data(data), m_size(size) {}

    // 通过字符串构造
    vector_ref(std::string& str) noexcept
    : vector_ref(reinterpret_cast<pointer>(&str[0]), str.size() / sizeof(T)) {}
    vector_ref(char* cstr) noexcept
    : vector_ref(reinterpret_cast<pointer>(cstr), strlen(cstr) / sizeof(T)) {}
    vector_ref(const char* cstr) noexcept
    : vector_ref(reinterpret_cast<pointer>(cstr), strlen(cstr) / sizeof(T)) {}

    // 通过POD容器构造
    template<typename Container, typename std::enable_if<std::is_pod<typename Container::value_type>::value, int>::type = 0>
    vector_ref(Container& c) noexcept
    : vector_ref(reinterpret_cast<pointer>(c.data()), c.size() * sizeof(typename Container::value_type) / sizeof(T)) {}
    template<typename Container, typename std::enable_if<std::is_pod<typename Container::value_type>::value, int>::type = 0>
    vector_ref(const Container& c) noexcept
    : vector_ref(reinterpret_cast<pointer>(c.data()), c.size() * sizeof(typename Container::value_type) / sizeof(T)) {}

    // 通过POD数组构造
    template<typename Elem, size_t N, typename std::enable_if<std::is_pod<Elem>::value, int>::type = 0>
    vector_ref(Elem (&arr)[N]) noexcept
    : vector_ref(reinterpret_cast<pointer>(arr), N * sizeof(Elem) / sizeof(T)) {}
    template<typename Elem, size_t N, typename std::enable_if<std::is_pod<Elem>::value, int>::type = 0>
    vector_ref(const Elem (&arr)[N]) noexcept
    : vector_ref(reinterpret_cast<pointer>(arr), N * sizeof(Elem) / sizeof(T)) {}

    // 是否为空
    bool empty() const noexcept { return 0 == m_size; }

    // 获取长度
    size_t size() const noexcept { return m_size; }

    // 获取指针
    pointer data() const noexcept { return m_data; }

    // 获取引用
    reference operator[](size_t idx) const noexcept { return m_data[idx]; }

    // 获取迭代器
    iterator begin() const noexcept { return m_data; }
    iterator end() const noexcept { return m_data + m_size; }

    // 截取[beg, min(beg + cnt, size))范围的子数组引用，若beg大于size抛出OutOfRange异常
    vector_ref cropped(size_t beg, size_t cnt) const {
        if (beg <= m_size) {
            return vector_ref(m_data + beg, std::min(m_size - beg, cnt));
        } else {
            throw OutOfRange();
        }
    }

    // 截取[beg, size)范围的子数组引用，若beg大于size抛出OutOfRange异常
    vector_ref cropped(size_t beg) const {
        if (beg <= m_size) {
            return vector_ref(m_data + beg, m_size - beg);
        } else {
            throw OutOfRange();
        }
    }

    // 转换为字符串
    std::string toString() const {
        return std::string(reinterpret_cast<const char*>(m_data), sizeof(T) * m_size);
    }

private:
    // 保存原始指针和长度信息（需保证引用对象的生命周期）
    pointer m_data = nullptr;
    size_t m_size = 0;
};

}   // namespace dev