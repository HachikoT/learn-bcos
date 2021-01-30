// Aleth: Ethereum C++ client, tools and libraries.
// Copyright 2014-2019 Aleth Authors.
// Licensed under the GNU General Public License, Version 3.

/**
 * 互斥锁相关类型定义
 * @file: Guards.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-30
 */
#pragma once

#include <mutex>

namespace dev {

// 互斥锁
using Mutex = std::mutex;

// RAII加锁/解锁
using Guard = std::lock_guard<Mutex>;
using UniqueLock = std::unique_lock<Mutex>;

}   // namespace dev