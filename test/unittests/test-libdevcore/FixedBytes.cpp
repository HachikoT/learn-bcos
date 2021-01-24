#include <boost/test/unit_test.hpp>
#include <libdevcore/FixedBytes.h>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(FixedBytesTests)

BOOST_AUTO_TEST_CASE(constructorTest)
{
    // 位数是否正确
    BOOST_CHECK(H160().size() * 8 == 160);
    BOOST_CHECK(H256().size() * 8 == 256);
    BOOST_CHECK(H512().size() * 8 == 512);
    BOOST_CHECK(H2048().size() * 8 == 2048);

    // 默认构造函数
    FixedBytes<20> zerofbs;
    BOOST_CHECK(!static_cast<bool>(zerofbs));
    BOOST_CHECK(zerofbs.hex0x() == "0x0000000000000000000000000000000000000000");

    // 随机构造
    auto randfbs1 = H160::random();
    auto randfbs2 = H160::random();
    BOOST_CHECK(randfbs1.hex0x() != randfbs2.hex0x());

    // 字符串构造
    FixedBytes<5> fromHexStr("0x1234567890");
    BOOST_CHECK(fromHexStr.hex0x() == "0x1234567890");

    // 字符串构造，左右对齐测试
    FixedBytes<5> fromHexStrLeft1("0x123456", Align::c_left);
    FixedBytes<5> fromHexStrLeft2("0x1234567890abcd", Align::c_left);
    FixedBytes<5> fromHexStrRight1("0x123456", Align::c_right);
    FixedBytes<5> fromHexStrRight2("0x1234567890abcd", Align::c_right);
    BOOST_CHECK(fromHexStrLeft1.hex0x() == "0x1234560000");
    BOOST_CHECK(fromHexStrLeft2.hex0x() == "0x1234567890");
    BOOST_CHECK(fromHexStrRight1.hex0x() == "0x0000123456");
    BOOST_CHECK(fromHexStrRight2.hex0x() == "0x567890abcd");

    // 算术类型构造
    U256 u1("0xf170d8e0ae1b57d7ecc121f6fe5ceb03c1267801ff720edd2f8463e7effac6c6");
    H256 h1(u1);
    BOOST_CHECK(h1.hex0x() == "0xf170d8e0ae1b57d7ecc121f6fe5ceb03c1267801ff720edd2f8463e7effac6c6");
    BOOST_CHECK(h1.toArith() == u1);
    U256 u2("0xf170d8e0ae1b57d7ecc121f6fe5ceb03c");
    H256 h2(u2);
    BOOST_CHECK(h2.hex0x() == "0x0000000000000000000000000000000f170d8e0ae1b57d7ecc121f6fe5ceb03c");
    BOOST_CHECK(h2.toArith() == u2);

    // 没有对齐
    BOOST_CHECK_THROW(FixedBytes<5>("0x123456"), Unaligned);
    BOOST_CHECK_THROW(FixedBytes<5>("0x1234567890abcd"), Unaligned);
}

BOOST_AUTO_TEST_CASE(boolwiseTest)
{
    H160 h1("0x1234567890123456789012345678901234567890");
    H160 h2("0x0234567890123456789012345678901234567890");
    BOOST_CHECK(h1 != h2);
    BOOST_CHECK(h1 > h2);
    BOOST_CHECK(h1 >= h2);
    BOOST_CHECK(h2 < h1);
    BOOST_CHECK(h2 <= h1);
    BOOST_CHECK(h1 == h1);

    H256 h3("0xf170d8e0ae1b57d7ecc121f6fe5ceb03c1267801ff720edd2f8463e7effac6c6");
    H256 h4("0xe170d8e0ae1b57d7ecc121f6fe5ceb03c1267801ff720edd2f8463e7effac6c6");
    BOOST_CHECK(h3 != h4);
    BOOST_CHECK(h3 > h4);
    BOOST_CHECK(h3 >= h4);
    BOOST_CHECK(h4 < h3);
    BOOST_CHECK(h4 <= h3);
    BOOST_CHECK(h3 == h3);
}

BOOST_AUTO_TEST_CASE(bitwiseTest)
{
    // not
    H160 h1;
    BOOST_CHECK("ffffffffffffffffffffffffffffffffffffffff" == (~h1).hex());

    // or
    H160 h2r("0x00000000000000000000ffffffffffffffffffff");
    H160 h2l("0xffffffffffffffffffff00000000000000000000");
    BOOST_CHECK("ffffffffffffffffffffffffffffffffffffffff" == (h2l | h2r).hex());

    // and
    H160 h3r("0x00000000000000000000ffffffffffffffffffff");
    H160 h3l("0xffffffffffffffffffff00000000000000000000");
    BOOST_CHECK("0000000000000000000000000000000000000000" == (h3l & h3r).hex());

    // xor
    H160 h4r("0xffffffffffffffffffff00000000000000000000");
    H160 h4l("0xffffffffffffffffffffffffffffffffffffffff");
    BOOST_CHECK("00000000000000000000ffffffffffffffffffff" == (h4l ^ h4r).hex());

    // right160
    H256 h5("0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb");
    BOOST_CHECK("aaaaaaaabbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb" == right160(h5).hex());
}

BOOST_AUTO_TEST_CASE(hashTest)
{
    auto h1 = H160::random();
    auto h2 = H160::random();
    BOOST_CHECK(std::hash<H160>()(h1) == std::hash<H160>()(h1));
    BOOST_CHECK(std::hash<H160>()(h1) != std::hash<H160>()(h2));

    auto h3 = H256::random();
    auto h4 = H256::random();
    BOOST_CHECK(std::hash<H256>()(h3) == std::hash<H256>()(h3));
    BOOST_CHECK(std::hash<H256>()(h3) != std::hash<H256>()(h4));

    auto h5 = H512::random();
    auto h6 = H512::random();
    BOOST_CHECK(std::hash<H512>()(h5) == std::hash<H512>()(h5));
    BOOST_CHECK(std::hash<H512>()(h5) != std::hash<H512>()(h6));

    auto h7 = H2048::random();
    auto h8 = H2048::random();
    BOOST_CHECK(std::hash<H2048>()(h7) == std::hash<H2048>()(h7));
    BOOST_CHECK(std::hash<H2048>()(h7) != std::hash<H2048>()(h8));
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test