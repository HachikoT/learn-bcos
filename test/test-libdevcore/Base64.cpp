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
    BOOST_CHECK("aGVsbG8gYmFzZTY0/+8=" == toBase64Std(str2));
    BOOST_CHECK("aGVsbG8gYmFzZTY0_-8=" == toBase64URL(str2));
}

BOOST_AUTO_TEST_CASE(fromBase64Test)
{
    // 空字符串
    std::string emptyStr;
    BOOST_CHECK(BytesConstRef(fromBase64Std(emptyStr)).toString() == "");
    BOOST_CHECK(BytesConstRef(fromBase64URL(emptyStr)).toString() == "");

    // 3N长输出
    std::string str0 = "aGVsbG8gYmFzZTY0";
    BOOST_CHECK(BytesConstRef(fromBase64Std(str0)).toString() == "hello base64");
    BOOST_CHECK(BytesConstRef(fromBase64URL(str0)).toString() == "hello base64");

    // 3N+1长输出
    std::string str1Std = "aGVsbG8gYmFzZTY0/w==";
    std::string str1URL = "aGVsbG8gYmFzZTY0_w==";
    BOOST_CHECK(BytesConstRef(fromBase64Std(str1Std)).toString() == "hello base64\xff");
    BOOST_CHECK(BytesConstRef(fromBase64URL(str1URL)).toString() == "hello base64\xff");

    // 3N+2长输出
    std::string str2Std = "aGVsbG8gYmFzZTY0/+8=";
    std::string str2URL = "aGVsbG8gYmFzZTY0_-8=";
    BOOST_CHECK(BytesConstRef(fromBase64Std(str2Std)).toString() == "hello base64\xff\xef");
    BOOST_CHECK(BytesConstRef(fromBase64URL(str2URL)).toString() == "hello base64\xff\xef");

    // 非法输入
    std::string str4N1STD = "aGVsbG8gYmFzZTY0/";
    std::string str4N1URL = "aGVsbG8gYmFzZTY0_";
    std::string str4N2STD = "aGVsbG8gYmFzZTY0/+";
    std::string str4N2URL = "aGVsbG8gYmFzZTY0_-";
    std::string str4N3STD = "aGVsbG8gYmFzZTY0/+8";
    std::string str4N3URL = "aGVsbG8gYmFzZTY0_-8";
    BOOST_CHECK_THROW(fromBase64Std(str4N1STD), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str4N1URL), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64Std(str4N2STD), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str4N2URL), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64Std(str4N3STD), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str4N3URL), BadBase64Ch);
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test