#include <boost/test/unit_test.hpp>
#include <libdevcore/Common.h>
#include <limits>
#include <chrono>
#include <thread>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(DevCoreCommonTests)

BOOST_AUTO_TEST_CASE(multipricisionTest)
{
    // 检查大整型位数是否正确
    BOOST_CHECK(std::numeric_limits<U160>::digits == 160);
    BOOST_CHECK(std::numeric_limits<S160>::digits == 160);
    BOOST_CHECK(std::numeric_limits<U256>::digits == 256);
    BOOST_CHECK(std::numeric_limits<S256>::digits == 256);
    BOOST_CHECK(std::numeric_limits<U512>::digits == 512);
    BOOST_CHECK(std::numeric_limits<S512>::digits == 512);

    // 有符号数为原码表示，并且额外用一个位表示符号
    BOOST_CHECK((BigInt)std::numeric_limits<U160>::max() == (BigInt)std::numeric_limits<S160>::max());
    BOOST_CHECK((BigInt)std::numeric_limits<S160>::max() == -(BigInt)std::numeric_limits<S160>::min());

    // 无符号数为补码表示
    BOOST_CHECK((static_cast<U160>(0) - std::numeric_limits<U160>::max()) == 1);

    // 当有符号数转换为无符号数，先是按位截断，再对无符号数的二进制补码求负数
    BOOST_CHECK((U160)std::numeric_limits<S160>::min() == 1);
}

BOOST_AUTO_TEST_CASE(timerTest)
{
    // 计时器的精度是毫秒级
    Timer tm;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    BOOST_CHECK(tm.elapsed() > 0);
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test