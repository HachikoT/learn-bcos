#include <boost/test/unit_test.hpp>
#include <libethcore/Address.h>
#include <libcrypto/ECDSA.h>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(AddressTests)

BOOST_AUTO_TEST_CASE(toAddressTest)
{
    // 计算外部账户地址测试
    SecKey sec("1f2b77e3a4b50120692912c94b204540ad44404386b10c615786a7efaa065d20");
    PubKey pub = toPubKey(sec);
    BOOST_CHECK(eth::toAddress(pub).hex0x() == "0xabcd68033a72978c1084e2d44d1fa06ddc4a2d57");

    // 计算合约账户地址测试
    eth::Address sender("6ac7ea33f8831ea9dcc53393aaa88b25a785dbf0");
    U256 nonce = 0;
    BOOST_CHECK(eth::toAddress(sender, nonce).hex0x() == "0xcd234a471b72ba2f1ccf0a70fcaba648a5eecd8d");
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test