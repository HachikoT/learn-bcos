/*
   base64.cpp and base64.h
   base64 encoding and decoding with C++.
   More information at
     https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
   Version: 2.rc.04 (release candidate)
   Copyright (C) 2004-2017, 2020 René Nyffenegger
   This source code is provided 'as-is', without any express or implied
   warranty. In no event will the author be held liable for any damages
   arising from the use of this software.
   Permission is granted to anyone to use this software for any purpose,
   including commercial applications, and to alter it and redistribute it
   freely, subject to the following restrictions:
   1. The origin of this source code must not be misrepresented; you must not
      claim that you wrote the original source code. If you use this source code
      in a product, an acknowledgment in the product documentation would be
      appreciated but is not required.
   2. Altered source versions must be plainly marked as such, and must not be
      misrepresented as being the original source code.
   3. This notice may not be removed or altered from any source distribution.
   René Nyffenegger rene.nyffenegger@adp-gmbh.ch
*/

/**
 * Base64编解码
 * @file: Base64.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-16
 */
#pragma once

#include "Common.h"

namespace dev {

// 将字节数组转换为Base64编码的字符串（标准base64字符集）
std::string toBase64Std(BytesConstRef bs);

// /**
//  * 将Base64编码的字符串转换为字节数组
//  * @param base64 Base64编码的字符串（标准字符集）
//  * @return 对应的字节数组
//  * @throw 遇到非法Base64字符抛出BadBase64Ch异常
//  */
// Bytes fromBase64STD(const std::string& base64);

// // 将字节数组转换为Base64编码的字符串（对url合法的base64字符集）
// std::string toBase64URL(BytesConstRef bs);

// /**
//  * 将Base64编码的字符串转换为字节数组
//  * @param base64 Base64编码的字符串（对url合法的字符集）
//  * @return 对应的字节数组
//  * @throw 遇到非法Base64字符抛出BadBase64Ch异常
//  */
// Bytes fromBase64URL(const std::string& base64);

}   // namespace dev