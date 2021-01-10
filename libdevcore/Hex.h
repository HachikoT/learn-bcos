/**
 * 16进制编解码
 * @file: Hex.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-03
 */
#pragma once

#include "Common.h"

namespace dev {

/**
 * 将字节数组转换为16进制字符串（小写，不带前缀0x）
 * @param bs 字节数组
 * @return 对应的16进制字符串
 */
std::string toHex(BytesConstRef bs);

/**
 * 将字节数组转换为16进制字符串（小写，带前缀0x）
 * @param bs 字节数组
 * @return 对应的16进制字符串
 */
std::string toHexPrefixed(BytesConstRef bs);

/**
 * 将16进制字符串转换为字节数组
 * @param hex 16进制字符串（允许前缀0x或0X，不允许空白符）
 * @return 对应的字节数组
 * @throw 遇到非法16进制字符抛出BadHexCh异常
 */
Bytes fromHex(const std::string& hex);

}   // namespace dev