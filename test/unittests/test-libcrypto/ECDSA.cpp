#include <boost/test/unit_test.hpp>
#include <libcrypto/ECDSA.h>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(ECDSATests)

BOOST_AUTO_TEST_CASE(contextTest)
{
    // 测试公钥私钥长度是否正确
    BOOST_CHECK(PubKey().size() == 64);
    BOOST_CHECK(SecKey().size() == 32);

    // 测试Signature是否可以正确转换为原始字节数组
    BOOST_CHECK(sizeof(Signature) == 65);
    Signature sig;
    BOOST_CHECK(static_cast<void*>(&sig) == static_cast<void*>(&sig.r));

    // 测试secp256k1的阶数n是否正确
    BOOST_CHECK(H256(c_secp256k1n).hex0x() == "0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
    BOOST_CHECK(c_secp256k1nHalf == c_secp256k1n / 2);
}

BOOST_AUTO_TEST_CASE(ecdsaTest)
{
    // 测试私钥转换为公钥
    SecKey sec("1f2b77e3a4b50120692912c94b204540ad44404386b10c615786a7efaa065d20");
    PubKey pub = toPubKey(sec);
    BOOST_CHECK(pub.hex() == "dfa13518ff965498743f3a01439dd86bc34ff9969c7a3f0430bbf8865734252953c9884af787b2cadd45f92dff2b81e21cfdf98873e492e5fdc07e9eb67ca74d");

    // 非法私钥
    SecKey badSec;
    BOOST_CHECK_THROW(toPubKey(badSec), BadSecKey);

    // 数字签名
    H256 digest = keccak256("hello");
    Signature sig = sign(sec, digest);
    BOOST_CHECK(pub == recover(sig, digest));
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test