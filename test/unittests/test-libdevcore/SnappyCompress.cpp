#include <boost/test/unit_test.hpp>
#include <libdevcore/SnappyCompress.h>
#include <string>
#include <algorithm>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(SnappyCompressTests)

BOOST_AUTO_TEST_CASE(snappyCompressTest)
{
    // 空输入
    std::string emptyInput = "";
    auto compressedEmptyInput = SnappyCompress::compress(emptyInput);
    auto uncompressedEmptyInput = SnappyCompress::uncompress(compressedEmptyInput);
    BOOST_CHECK(
        emptyInput.size() == uncompressedEmptyInput.size() &&
        std::equal(emptyInput.begin(), emptyInput.end(), uncompressedEmptyInput.begin())
    );

    // 非空输入
    std::string input = "hello snappy";
    auto compressedInput = SnappyCompress::compress(input);
    auto uncompressedInput = SnappyCompress::uncompress(compressedInput);
    BOOST_CHECK(
        input.size() == uncompressedInput.size() &&
        std::equal(input.begin(), input.end(), uncompressedInput.begin())
    );

    // 被破坏的输入
    std::string originInput = "hello snappy";
    auto compressedOriginInput = SnappyCompress::compress(originInput);
    compressedOriginInput.resize(compressedOriginInput.size() - 1);
    BOOST_CHECK_THROW(SnappyCompress::uncompress(compressedOriginInput), CorruptedInput);
    BOOST_CHECK_THROW(SnappyCompress::uncompress(""), CorruptedInput);
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test