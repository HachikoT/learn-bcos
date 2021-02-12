/**
 * @CopyRight:
 * FISCO-BCOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FISCO-BCOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FISCO-BCOS.  If not, see <http://www.gnu.org/licenses/>
 * (c) 2016-2018 fisco-dev contributors.
 *
 */

/**
 * boost.log写日志接口
 * @file: Log.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-02-12
 */
#pragma once

#include <string>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/sources/severity_channel_logger.hpp>
#include <boost/log/trivial.hpp>

namespace dev {

// 写日志对象
extern boost::log::sources::severity_channel_logger_mt<boost::log::trivial::severity_level, std::string>
    g_fileLoggerHandler;

// 日志等级
enum LogLevel {
    Fatal = boost::log::trivial::fatal,
    Error = boost::log::trivial::error,
    Warning = boost::log::trivial::warning,
    Info = boost::log::trivial::info,
    Debug = boost::log::trivial::debug,
    Trace = boost::log::trivial::trace
};

// 日志过滤等级
extern LogLevel g_fileLogLevel;

// 写日志接口
#define LOG(level)                                   \
    if (dev::LogLevel::level >= dev::g_fileLogLevel) \
    BOOST_LOG_SEV(dev::g_fileLoggerHandler,          \
        (boost::log::v2s_mt_posix::trivial::severity_level)(dev::LogLevel::level))

// BCOS log format
#define LOG_BADGE(_NAME) "[" << (_NAME) << "]"
#define LOG_DESC(_DESCRIPTION) (_DESCRIPTION)
#define LOG_KV(_K, _V) "," << (_K) << "=" << (_V)

}   // namespace dev