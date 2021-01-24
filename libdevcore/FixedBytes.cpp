// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 定长字节数组类型
 * @file: FixedBytes.cpp
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-24
 */
#include "FixedBytes.h"

namespace dev {

// 真随机数生成引擎，linux上通过读取/dev/urandom设备实现
thread_local std::random_device t_randomEngine;

}   // namespace dev