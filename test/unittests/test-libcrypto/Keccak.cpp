#include <boost/test/unit_test.hpp>
#include <libcrypto/Keccak.h>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(KeccakTests)

BOOST_AUTO_TEST_CASE(keccak256Test)
{
    // 空字符串的keccak256值
    BOOST_CHECK(keccak256("").hex() == "c5d2460186f7233c927e7db2dcc703c0e500b653ca82273b7bfad8045d85a470");

    // hello
    BOOST_CHECK(keccak256("hello").hex() == "1c8aff950685c2ed4bc3174f3472287b56d9517b9c948127319a09a7a36deac8");

    // 长字符串 > blockSize=136
    std::string longStr(200, 'r');
    BOOST_CHECK(keccak256(longStr).hex() == "aac9a41d73145d4163a16b74db2e559b49158aac08932f04ed58db939c303c0d");
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test