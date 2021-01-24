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
 */

/**
 * snappy压缩/解压算法
 * @file: SnappyCompress.cpp
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-24
 */
#include "SnappyCompress.h"
#include <snappy.h>

namespace dev {

/**
 * 压缩数据
 * @param src 输入的字节数组
 * @return 经过压缩的数据
 */
Bytes SnappyCompress::compress(BytesConstRef src) {
    // 提前分配足够的空间
    Bytes dst(snappy::MaxCompressedLength(src.size()));
    size_t compressedLen = 0;

    // 进行压缩
    snappy::RawCompress(
        reinterpret_cast<const char*>(src.data()),
        src.size(),
        reinterpret_cast<char*>(dst.data()),
        &compressedLen
    );

    // 调整为实际的压缩长度
    dst.resize(compressedLen);

    return dst;
}

/**
 * 解压数据
 * @param src 经过压缩的字节数组
 * @return 经过解压的数据
 * @throw 输入的压缩数据损坏抛出CorruptedInput异常
 */
Bytes SnappyCompress::uncompress(BytesConstRef src) {
    // 解析出解压数据的长度（花费O(1)时间）
    size_t uncompressedLen = 0;
    bool status = snappy::GetUncompressedLength(
        reinterpret_cast<const char*>(src.data()),
        src.size(),
        &uncompressedLen
    );

    if (!status) {
        // 解析长度编码出错
        throw CorruptedInput();
    }

    // 提前分配空间
    Bytes dst(uncompressedLen);

    // 进行解压
    status = snappy::RawUncompress(
        reinterpret_cast<const char*>(src.data()),
        src.size(),
        reinterpret_cast<char*>(dst.data())
    );

    if (!status) {
        // 压缩数据损坏
        throw CorruptedInput();
    }

    return dst;
}

}   // namespace dev