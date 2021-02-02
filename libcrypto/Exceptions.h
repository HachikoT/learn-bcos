// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 异常类型定义
 * @file: Exceptions.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-02-02
 */
#pragma once

#include <stdexcept>
#include <string>

namespace dev {

// 异常类生成模板
#define DEV_SIMPLE_EXCEPTION(X)\
struct X : public std::runtime_error {\
    explicit X(const std::string& what_arg = "") : std::runtime_error(what_arg) {}\
}

DEV_SIMPLE_EXCEPTION(BadSecKey);
DEV_SIMPLE_EXCEPTION(BadSignature);

}   // namespace dev