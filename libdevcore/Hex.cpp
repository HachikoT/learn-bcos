/**
 * 16进制编解码
 * @file: Hex.cpp
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-03
 */
#include "Hex.h"
#include <cctype>

namespace dev {

// 16进制字符表（小写）
static const char* s_hexDigits = "0123456789abcdef";

/**
 * 将字节数组转换为16进制字符串（小写，不带前缀0x）
 * @param bs 字节数组
 * @return 对应的16进制字符串
 */
std::string toHex(BytesConstRef bs) {
    // 提前分配好内存
    std::string ret(bs.size() * 2, '\0');

    // 每一个字节转换为两个字符
    size_t off = 0;
    for (auto b : bs) {
        ret[off++] = s_hexDigits[(b >> 4) & 0x0f];
        ret[off++] = s_hexDigits[b & 0x0f];
    }

    return ret;
}

/**
 * 将字节数组转换为16进制字符串（小写，带前缀0x）
 * @param bs 字节数组
 * @return 对应的16进制字符串
 */
std::string toHexPrefixed(BytesConstRef bs) {
    // 提前分配好内存
    std::string ret = "0x";
    ret.resize(bs.size() * 2 + 2);

    // 每一个字节转换为两个字符
    size_t off = 2;
    for (auto b : bs) {
        ret[off++] = s_hexDigits[(b >> 4) & 0x0f];
        ret[off++] = s_hexDigits[b & 0x0f];
    }

    return ret;
}

/**
 * 将16进制字符转换为对应的int数值
 * @param hexCh 16进制字符（0-9|a-z|A-Z）
 * @return 对应的int数值，非法字符抛出BadHexCh异常
 */
static int hexCh2i(int hexCh) {
    if (hexCh >= '0' && hexCh <= '9') return hexCh - '0';
    if (hexCh >= 'a' && hexCh <= 'f') return hexCh - 'a' + 10;
    if (hexCh >= 'A' && hexCh <= 'F') return hexCh - 'A' + 10;
    throw BadHexCh();
}

/**
 * 将16进制字符串转换为字节数组
 * @param hex 16进制字符串（允许前缀0x或0X，不允许空白符）
 * @return 对应的字节数组
 * @throw 遇到非法16进制字符抛出BadHexCh异常
 */
Bytes fromHex(const std::string& hex) {
    // 跳过0x开头
    size_t endCh = hex.size();
    size_t curCh = endCh >= 2 && hex[0] == '0' && tolower(hex[1]) == 'x' ? 2 : 0;

    // 提前分配好内存
    Bytes ret((endCh - curCh + 1) / 2);
    size_t curByte = 0;

    // 奇数个字符，先处理一个
    if (endCh % 2) {
        int l = hexCh2i(hex[curCh++]);
        ret[curByte++] = l;
    }

    // 每两个字符转换为一个字节
    while (curCh != endCh) {
        int h = hexCh2i(hex[curCh++]);
        int l = hexCh2i(hex[curCh++]);
        ret[curByte++] = h << 4 | l;
    }

    return ret;
}

}   // namespace dev