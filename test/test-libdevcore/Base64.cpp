#include <boost/test/unit_test.hpp>
#include <libdevcore/Base64.h>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(Base64Tests)

BOOST_AUTO_TEST_CASE(toBase64Test)
{
    // 空数组
    Bytes emptyBS;
    BOOST_CHECK("" == toBase64Std(emptyBS));
    BOOST_CHECK("" == toBase64URL(emptyBS));

    // 3N长输入
    std::string str0 = "hello base64";
    BOOST_CHECK("aGVsbG8gYmFzZTY0" == toBase64Std(str0));
    BOOST_CHECK("aGVsbG8gYmFzZTY0" == toBase64URL(str0));

    // 3N+1长输入
    std::string str1 = "hello base64\xff";
    BOOST_CHECK("aGVsbG8gYmFzZTY0/w==" == toBase64Std(str1));
    BOOST_CHECK("aGVsbG8gYmFzZTY0_w==" == toBase64URL(str1));

    // 3N+2长输入
    std::string str2 = "hello base64\xff\xef";
    BOOST_CHECK("aGVsbG8gYmFzZTY0/+8=" == toBase64Std(str1));
    BOOST_CHECK("aGVsbG8gYmFzZTY0_-8=" == toBase64URL(str1));
}

// BOOST_AUTO_TEST_CASE(fromBase64Test)
// {

// }

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test