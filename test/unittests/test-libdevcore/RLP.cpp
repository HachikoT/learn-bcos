#include <boost/test/unit_test.hpp>
#include <libdevcore/RLP.h>
#include <libdevcore/Hex.h>
#include <limits>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(RLPTests)

BOOST_AUTO_TEST_CASE(encodeTest)
{
    RLPStream s;

    // 空字符串编码
    BOOST_CHECK(toHex0x(c_rlpEmptyData) == "0x80");
    s << "";
    BOOST_CHECK(toHex0x(s.take()) == "0x80");
    s << static_cast<uint32_t>(0);
    BOOST_CHECK(toHex0x(s.take()) == "0x80");
    s << static_cast<uint64_t>(0);
    BOOST_CHECK(toHex0x(s.take()) == "0x80");
    s << static_cast<U160>(0);
    BOOST_CHECK(toHex0x(s.take()) == "0x80");
    s << static_cast<U256>(0);
    BOOST_CHECK(toHex0x(s.take()) == "0x80");
    s << static_cast<U512>(0);
    BOOST_CHECK(toHex0x(s.take()) == "0x80");
    s.append(H160(), true);
    BOOST_CHECK(toHex0x(s.take()) == "0x80");
    s.append(H256(), true);
    BOOST_CHECK(toHex0x(s.take()) == "0x80");
    s.append(H512(), true);
    BOOST_CHECK(toHex0x(s.take()) == "0x80");
    s.append(H2048(), true);
    BOOST_CHECK(toHex0x(s.take()) == "0x80");

    // 空列表编码
    BOOST_CHECK(toHex0x(c_rlpEmptyList) == "0xc0");
    s.appendList(0);
    BOOST_CHECK(toHex0x(s.take()) == "0xc0");

    // 字符串长度为1且属于[0x00, 0x7f]
    s << static_cast<uint32_t>(0x01);
    BOOST_CHECK(toHex0x(s.take()) == "0x01");
    s << static_cast<uint64_t>(0x02);
    BOOST_CHECK(toHex0x(s.take()) == "0x02");
    s << static_cast<U160>(0x70);
    BOOST_CHECK(toHex0x(s.take()) == "0x70");
    s << static_cast<U256>(0x7f);
    BOOST_CHECK(toHex0x(s.take()) == "0x7f");
    s << static_cast<U512>(0x7f);
    BOOST_CHECK(toHex0x(s.take()) == "0x7f");
    s << fromHex("0x7f");
    BOOST_CHECK(toHex0x(s.take()) == "0x7f");

    // 字符串长度编码到前缀
    s << static_cast<uint32_t>(0x80);
    BOOST_CHECK(toHex0x(s.take()) == "0x8180");
    s << static_cast<uint64_t>(0xff);
    BOOST_CHECK(toHex0x(s.take()) == "0x81ff");
    s << static_cast<U160>(0x123456);
    BOOST_CHECK(toHex0x(s.take()) == "0x83123456");
    s << std::numeric_limits<U256>::max();
    BOOST_CHECK(toHex0x(s.take()) == "0xa0ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    s << static_cast<U512>(std::numeric_limits<U256>::max());
    BOOST_CHECK(toHex0x(s.take()) == "0xa0ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    s << fromHex("0x1234567890");
    BOOST_CHECK(toHex0x(s.take()) == "0x851234567890");

    // 字符串长度单独编码
    s << std::numeric_limits<U512>::max();
    BOOST_CHECK(toHex0x(s.take()) == "0xb840ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    s << fromHex("0x1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef");
    BOOST_CHECK(toHex0x(s.take()) == "0xb8381234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef");

    // 列表长度编码到前缀
    s.appendList(2);
    s << fromHex("0x1234567890") << fromHex("0x1234567890");
    BOOST_CHECK(toHex0x(s.take()) == "0xcc851234567890851234567890");

    // 列表长度单独编码
    s.appendList(7);
    s << fromHex("0x1234567890abcd")
      << fromHex("0x1234567890abcd")
      << fromHex("0x1234567890abcd")
      << fromHex("0x1234567890abcd")
      << fromHex("0x1234567890abcd")
      << fromHex("0x1234567890abcd")
      << fromHex("0x1234567890abcd");
    BOOST_CHECK(toHex0x(s.take()) == "0xf838871234567890abcd871234567890abcd871234567890abcd871234567890abcd871234567890abcd871234567890abcd871234567890abcd");

    // 测试下peek
    s << "";
    BOOST_CHECK(toHex0x(s.peek()) == "0x80");
    BOOST_CHECK(toHex0x(s.peek()) == "0x80");

    // 列表还没序列化完成异常
    s.appendList(1);
    BOOST_CHECK_THROW(s.take(), RLPIncompleteList);
    BOOST_CHECK_THROW(s.peek(), RLPIncompleteList);
}

BOOST_AUTO_TEST_CASE(decodeTest)
{
    // null
    Bytes bs = fromHex("");
    RLP nullItem(bs);
    BOOST_CHECK(nullItem.isNull());
    BOOST_CHECK(nullItem.actualSize() == 0);
    BOOST_CHECK_THROW(nullItem.convert<U160>(), RLPBadCast);
    BOOST_CHECK_THROW(nullItem.convert<H160>(), RLPBadCast);
    BOOST_CHECK_THROW(nullItem.convert<std::string>(), RLPBadCast);
    BOOST_CHECK_THROW(nullItem.convert<Bytes>(), RLPBadCast);

    // 空字符串
    bs = fromHex("0x80");
    RLP emptyItem(bs);
    BOOST_CHECK(emptyItem.convert<uint32_t>() == 0);
    BOOST_CHECK(emptyItem.convert<uint64_t>() == 0);
    BOOST_CHECK(emptyItem.convert<U160>() == 0);
    BOOST_CHECK(emptyItem.convert<U256>() == 0);
    BOOST_CHECK(emptyItem.convert<U512>() == 0);
    BOOST_CHECK(emptyItem.convert<Bytes>().size() == 0);
    BOOST_CHECK(emptyItem.convert<std::string>() == "");
    BOOST_CHECK(emptyItem.toFixedBytes<H160>(Align::c_right) == H160());

    // 单字节字符串
    bs = fromHex("0x7f");
    RLP singleByteItem(bs);
    BOOST_CHECK(singleByteItem.convert<uint32_t>() == 0x7f);
    BOOST_CHECK(singleByteItem.convert<uint64_t>() == 0x7f);
    BOOST_CHECK(singleByteItem.convert<U160>() == 0x7f);
    BOOST_CHECK(singleByteItem.convert<U256>() == 0x7f);
    BOOST_CHECK(singleByteItem.convert<U512>() == 0x7f);
    BOOST_CHECK(toHex0x(singleByteItem.convert<Bytes>()) == "0x7f");
    BOOST_CHECK(singleByteItem.convert<std::string>() == "\x7f");
    BOOST_CHECK(singleByteItem.toFixedBytes<H160>(Align::c_right).hex() == "000000000000000000000000000000000000007f");

    // 字符串长度编码到前缀
    bs = fromHex("0xa0ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    RLP immDataItem(bs);
    BOOST_CHECK_THROW(immDataItem.convert<U160>(), RLPBadCast);
    BOOST_CHECK(immDataItem.convert<U256>() == std::numeric_limits<U256>::max());
    BOOST_CHECK(toHex0x(immDataItem.convert<Bytes>()) == "0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    BOOST_CHECK(immDataItem.convert<H256>().hex0x() == "0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

    // 字符串长度单独编码
    bs = fromHex("0xb840ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    RLP indDataItem(bs);
    BOOST_CHECK_THROW(indDataItem.convert<U256>(), RLPBadCast);
    BOOST_CHECK(indDataItem.convert<U512>() == std::numeric_limits<U512>::max());
    BOOST_CHECK(toHex0x(indDataItem.convert<Bytes>()) == "0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    BOOST_CHECK(indDataItem.convert<H512>().hex0x() == "0xffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");

    // 列表长度编码到前缀
    bs = fromHex("0xcc851234567890851234567890");
    RLP immListItem(bs);
    auto immListItems = immListItem.splitList();
    BOOST_CHECK(immListItems.size() == 2);
    BOOST_CHECK(toHex0x(immListItems[0].convert<Bytes>()) == "0x1234567890");
    BOOST_CHECK(toHex0x(immListItems[1].convert<Bytes>()) == "0x1234567890");

    // 列表长度单独编码
    bs = fromHex("0xf838871234567890abcd871234567890abcd871234567890abcd871234567890abcd871234567890abcd871234567890abcd871234567890abcd");
    RLP indListItem(bs);
    auto indListItems = indListItem.splitList();
    BOOST_CHECK(indListItems.size() == 7);
    BOOST_CHECK(toHex0x(indListItems[0].convert<Bytes>()) == "0x1234567890abcd");

    // 本来应该是单字节编码但是长度编码到前缀
    bs = fromHex("0x817f");
    BOOST_CHECK_THROW(RLP rlp100(bs), BadRLP);

    // 输入大于实际的数据项长度
    bs = fromHex("0x8080");
    BOOST_CHECK_THROW(RLP rlp200(bs), BadRLP);
    bs = fromHex("0xb7ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    BOOST_CHECK_THROW(RLP rlp201(bs), BadRLP);
    bs = fromHex("0xb840ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    BOOST_CHECK_THROW(RLP rlp202(bs), BadRLP);
    bs = fromHex("0xcc85123456789085123456789000");
    BOOST_CHECK_THROW(RLP rlp203(bs), BadRLP);
    bs = fromHex("0xf838871234567890abcd871234567890abcd871234567890abcd871234567890abcd871234567890abcd871234567890abcd871234567890abcd00");
    BOOST_CHECK_THROW(RLP rlp204(bs), BadRLP);

    // 输入小于实际的数据项长度
    bs = fromHex("0x81");
    BOOST_CHECK_THROW(RLP rlp300(bs), BadRLP);
    bs = fromHex("0xb8");
    BOOST_CHECK_THROW(RLP rlp301(bs), BadRLP);
    bs = fromHex("0xb838");
    BOOST_CHECK_THROW(RLP rlp302(bs), BadRLP);
    bs = fromHex("0xb80101");
    BOOST_CHECK_THROW(RLP rlp303(bs), BadRLP);
    bs = fromHex("0xc1");
    BOOST_CHECK_THROW(RLP rlp304(bs), BadRLP);
    bs = fromHex("0xf8");
    BOOST_CHECK_THROW(RLP rlp305(bs), BadRLP);
    bs = fromHex("0xf838");
    BOOST_CHECK_THROW(RLP rlp306(bs), BadRLP);
    bs = fromHex("0xf80101");
    BOOST_CHECK_THROW(RLP rlp307(bs), BadRLP);

    // 数字包含前导零
    bs = fromHex("0xa000ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
    RLP zeroLeadingItem(bs);
    BOOST_CHECK_THROW(zeroLeadingItem.convert<U256>(), RLPBadCast);

    // 不是列表数据项进行列表分割
    bs = fromHex("0x7f");
    RLP notListItem(bs);
    BOOST_CHECK_THROW(notListItem.splitList(), RLPBadCast);

    // 列表中碰到错误编码的数据项
    bs = fromHex("0xcc851234567890861234567890");
    RLP wrongListItem(bs);
    BOOST_CHECK_THROW(wrongListItem.splitList(), BadRLP);
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test