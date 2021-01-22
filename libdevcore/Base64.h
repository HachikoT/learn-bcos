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
std::string toBase64Std(BytesConstRef src);

// 将字节数组转换为Base64编码的字符串（对url合法的base64字符集）
std::string toBase64URL(BytesConstRef src);

/**
 * 将Base64编码的字符串转换为字节数组
 * @param src Base64编码的字符串（标准base64字符集）
 * @return 对应的字节数组
 * @throw 遇到非法Base64字符抛出BadBase64Ch异常
 */
Bytes fromBase64Std(const std::string& src);

/**
 * 将Base64编码的字符串转换为字节数组
 * @param src Base64编码的字符串（对url合法的base64字符集）
 * @return 对应的字节数组
 * @throw 遇到非法Base64字符抛出BadBase64Ch异常
 */
Bytes fromBase64URL(const std::string& src);

}   // namespace dev