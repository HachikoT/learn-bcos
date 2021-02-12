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
 * @file: Log.cpp
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-02-12
 */
#include "Log.h"

namespace dev {

// 写日志对象
boost::log::sources::severity_channel_logger_mt<boost::log::trivial::severity_level, std::string>
    g_fileLoggerHandler(boost::log::keywords::channel = "fileLogger");

// 日志过滤等级
LogLevel g_fileLogLevel = LogLevel::Trace;

}   // namespace dev