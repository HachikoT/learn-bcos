#include <boost/test/unit_test.hpp>
#include <libdevcore/Hex.h>
#include <boost/algorithm/string.hpp>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(HexTests)

BOOST_AUTO_TEST_CASE(toHexTest)
{
    // 空数组
    Bytes emptyBS;
    BOOST_CHECK("" == toHex(emptyBS));
    BOOST_CHECK("0x" == toHex0x(emptyBS));

    // 非空数组
    Bytes bs = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef };
    BOOST_CHECK("1234567890abcdef" == toHex(bs));
    BOOST_CHECK("0x1234567890abcdef" == toHex0x(bs));
}

BOOST_AUTO_TEST_CASE(fromHexTest)
{
    // 空数组
    Bytes emptyBS;
    BOOST_CHECK(emptyBS == fromHex(toHex(emptyBS)));
    BOOST_CHECK(emptyBS == fromHex(toHex0x(emptyBS)));

    // 非空数组
    Bytes bs = { 0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef };
    BOOST_CHECK(bs == fromHex(toHex(bs)));
    BOOST_CHECK(bs == fromHex(toHex0x(bs)));

    // 大写16进制字符
    BOOST_CHECK(bs == fromHex(boost::to_upper_copy(toHex(bs))));
    BOOST_CHECK(bs == fromHex(boost::to_upper_copy(toHex0x(bs))));

    // 奇数个字符
    std::string oddStr = "0x234567890abcdef";
    BOOST_CHECK("0234567890abcdef" == toHex(fromHex(oddStr)));

    // 非法字符
    BOOST_CHECK_THROW(fromHex("0xabcdefg"), BadHexCh);
    BOOST_CHECK_THROW(fromHex("0xg"), BadHexCh);
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test