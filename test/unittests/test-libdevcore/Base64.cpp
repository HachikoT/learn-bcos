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

    // 4N长输入
    std::string str0 = "aGVsbG8gYmFzZTY0";
    BOOST_CHECK(BytesConstRef(fromBase64Std(str0)).toString() == "hello base64");
    BOOST_CHECK(BytesConstRef(fromBase64URL(str0)).toString() == "hello base64");

    // 4N长输入，但是有两个填充字符
    std::string str0Pad2Std = "aGVsbG8gYmFzZTY0/w==";
    std::string str0Pad2URL = "aGVsbG8gYmFzZTY0_w==";
    BOOST_CHECK(BytesConstRef(fromBase64Std(str0Pad2Std)).toString() == "hello base64\xff");
    BOOST_CHECK(BytesConstRef(fromBase64URL(str0Pad2URL)).toString() == "hello base64\xff");

    // 4N长输入，但是有一个填充字符
    std::string str0Pad1Std = "aGVsbG8gYmFzZTY0/+8=";
    std::string str0Pad1URL = "aGVsbG8gYmFzZTY0_-8=";
    BOOST_CHECK(BytesConstRef(fromBase64Std(str0Pad1Std)).toString() == "hello base64\xff\xef");
    BOOST_CHECK(BytesConstRef(fromBase64URL(str0Pad1URL)).toString() == "hello base64\xff\xef");

    // 4N长输入，允许两段编码拼接起来
    std::string str0CatStd = "aGVsbG8gYmFzZTY0/+8=aGVsbG8gYmFzZTY0/+8=";
    std::string str0CatURL = "aGVsbG8gYmFzZTY0_-8=aGVsbG8gYmFzZTY0_-8=";
    BOOST_CHECK(BytesConstRef(fromBase64Std(str0CatStd)).toString() == "hello base64\xff\xefhello base64\xff\xef");
    BOOST_CHECK(BytesConstRef(fromBase64URL(str0CatURL)).toString() == "hello base64\xff\xefhello base64\xff\xef");

    // 4N+1长输入
    std::string str1Std = "aGVsbG8gYmFzZTY0/";
    std::string str1URL = "aGVsbG8gYmFzZTY0_";
    BOOST_CHECK_THROW(fromBase64Std(str1Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str1URL), BadBase64Ch);

    // 4N+2长输入
    std::string str2Std = "aGVsbG8gYmFzZTY0/w";
    std::string str2URL = "aGVsbG8gYmFzZTY0_w";
    BOOST_CHECK(BytesConstRef(fromBase64Std(str2Std)).toString() == "hello base64\xff");
    BOOST_CHECK(BytesConstRef(fromBase64URL(str2URL)).toString() == "hello base64\xff");

    // 4N+3长输入
    std::string str3Std = "aGVsbG8gYmFzZTY0/+8";
    std::string str3URL = "aGVsbG8gYmFzZTY0_-8";
    BOOST_CHECK(BytesConstRef(fromBase64Std(str3Std)).toString() == "hello base64\xff\xef");
    BOOST_CHECK(BytesConstRef(fromBase64URL(str3URL)).toString() == "hello base64\xff\xef");

    // 4N长输入，但有一个四元组中前两个是非法字符
    std::string str0Fault1Std = "aGVsbG8gYmFz?TY0";
    std::string str0Fault1URL = "aGVsbG8gYmFz?TY0";
    BOOST_CHECK_THROW(fromBase64Std(str0Fault1Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str0Fault1URL), BadBase64Ch);

    // 4N长输入，但有一个四元组中前两个是非法字符
    std::string str0Fault2Std = "aGVsbG8gYmFzZ?Y0";
    std::string str0Fault2URL = "aGVsbG8gYmFzZ?Y0";
    BOOST_CHECK_THROW(fromBase64Std(str0Fault2Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str0Fault2URL), BadBase64Ch);

    // 4N长输入，但有一个四元组中后两个是非法字符
    std::string str0Fault3Std = "aGVsbG8gYmFzZT?0";
    std::string str0Fault3URL = "aGVsbG8gYmFzZT?0";
    BOOST_CHECK_THROW(fromBase64Std(str0Fault3Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str0Fault3URL), BadBase64Ch);

    // 4N长输入，但有一个四元组中后两个是非法字符
    std::string str0Fault4Std = "aGVsbG8gYmFzZTY?";
    std::string str0Fault4URL = "aGVsbG8gYmFzZTY?";
    BOOST_CHECK_THROW(fromBase64Std(str0Fault4Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str0Fault4Std), BadBase64Ch);

    // 4N+2长输入，但是最后两个字符是非法字符
    std::string str2Fault1Std = "aGVsbG8gYmFzZTY0?w";
    std::string str2Fault1URL = "aGVsbG8gYmFzZTY0?w";
    BOOST_CHECK_THROW(fromBase64Std(str2Fault1Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str2Fault1URL), BadBase64Ch);

    // 4N+2长输入，但是最后两个字符是非法字符
    std::string str2Fault2Std = "aGVsbG8gYmFzZTY0/?";
    std::string str2Fault2URL = "aGVsbG8gYmFzZTY0_?";
    BOOST_CHECK_THROW(fromBase64Std(str2Fault2Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str2Fault2URL), BadBase64Ch);

    // 4N+3长输入，但是最后三个字符是非法字符
    std::string str3Fault1Std = "aGVsbG8gYmFzZTY0?+8";
    std::string str3Fault1URL = "aGVsbG8gYmFzZTY0?-8";
    BOOST_CHECK_THROW(fromBase64Std(str3Fault1Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str3Fault1URL), BadBase64Ch);

    // 4N+3长输入，但是最后三个字符是非法字符
    std::string str3Fault2Std = "aGVsbG8gYmFzZTY0/?8";
    std::string str3Fault2URL = "aGVsbG8gYmFzZTY0_?8";
    BOOST_CHECK_THROW(fromBase64Std(str3Fault2Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str3Fault2URL), BadBase64Ch);

    // 4N+3长输入，但是最后三个字符是非法字符
    std::string str3Fault3Std = "aGVsbG8gYmFzZTY0/+?";
    std::string str3Fault3URL = "aGVsbG8gYmFzZTY0_-?";
    BOOST_CHECK_THROW(fromBase64Std(str3Fault3Std), BadBase64Ch);
    BOOST_CHECK_THROW(fromBase64URL(str3Fault3URL), BadBase64Ch);
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test