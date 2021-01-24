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
 * @file: SnappyCompress.h
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-24
 */
#pragma once

#include "Common.h"
#include "Exceptions.h"

namespace dev {

/**
 * snappy是由google开发的压缩/解压C++库，注重压缩速度，压缩后文件大小比其它算法大一些
 * snappy在64位x86并且是小端的cpu上性能最佳
 * - 在Intel(R) Core(TM)2 2.4GHz中测试数据:
 *      压缩速率：~200-500MB/s
 *      解压速率：~400-800MB/s
 * - 压缩比（压缩数据大小/原始数据大小）：
 *      对于HTML：~25%
 *      对于普通文本（plain text）：~50%
 *      对于JPEG等已经压缩过的文件：~100%
 */

class SnappyCompress {
public:
    /**
     * 压缩数据
     * @param src 输入的字节数组
     * @return 经过压缩的数据
     */
    static Bytes compress(BytesConstRef src);

    /**
     * 解压数据
     * @param src 经过压缩的字节数组
     * @return 经过解压的数据
     * @throw 输入的压缩数据损坏抛出CorruptedInput异常
     */
    static Bytes uncompress(BytesConstRef src);
};

}   // namespace dev