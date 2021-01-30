#include <boost/test/unit_test.hpp>
#include <libdevcore/ThreadPool.h>
#include <algorithm>
#include <future>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(ThreadPoolTests)

BOOST_AUTO_TEST_CASE(ThreadPoolTest)
{
    ThreadPool pool(std::max(1U, std::thread::hardware_concurrency()));
    std::promise<int> p1;
    auto fut1 = p1.get_future();
    std::promise<int> p2;
    auto fut2 = p2.get_future();
    std::promise<int> p3;
    auto fut3 = p3.get_future();
    std::promise<int> p4;
    auto fut4 = p4.get_future();
    pool.enqueue([&] {
        p1.set_value(1);
    });
    pool.enqueue([&] {
        p2.set_value(2);
    });
    pool.enqueue([&] {
        p3.set_value(3);
    });
    pool.enqueue([&] {
        p4.set_value(4);
    });
    BOOST_CHECK(fut1.get() == 1);
    BOOST_CHECK(fut2.get() == 2);
    BOOST_CHECK(fut3.get() == 3);
    BOOST_CHECK(fut4.get() == 4);

    // 析构函数会终止线程池运行
    for (int i = 0; i < 10000; ++i) {
        pool.enqueue([] {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        });
    }
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test