#include <boost/test/unit_test.hpp>
#include <libdevcore/vector_ref.h>
#include <vector>
#include <utility>

namespace dev { namespace test {

BOOST_AUTO_TEST_SUITE(VectorRefTests)

BOOST_AUTO_TEST_CASE(constructorTest)
{
    // 默认构造函数
    vector_ref<int> emptyRef;
    vector_ref<const int> emptyConstRef;
    BOOST_CHECK(emptyRef.data() == nullptr && emptyRef.size() == 0);
    BOOST_CHECK(emptyConstRef.data() == nullptr && emptyConstRef.size() == 0);

    // 通过字符串构造
    std::string str = "hello";
    const std::string constStr = "hello";
    std::string rvalueStr = "hello";
    vector_ref<char> strRef1(str);
    // vector_ref<char> strRef2(constStr);
    // vector_ref<char> strRef3(std::move(rvalueStr));
    vector_ref<const char> constStrRef1(str);
    vector_ref<const char> constStrRef2(constStr);
    vector_ref<const char> constStrRef3(std::move(rvalueStr));
    BOOST_CHECK(strRef1.data() == &str[0] && strRef1.size() == str.size());
    BOOST_CHECK(constStrRef1.data() == &str[0] && constStrRef1.size() == str.size());
    BOOST_CHECK(constStrRef2.data() == &constStr[0] && constStrRef2.size() == constStr.size());
    BOOST_CHECK(constStrRef3.data() == &rvalueStr[0] && constStrRef3.size() == rvalueStr.size());

    // 通过POD容器构造
    std::vector<int> vec = { 1, 2, 3 };
    const std::vector<int> constVec = { 1, 2, 3 };
    std::vector<int> rvalueVec = { 1, 2, 3 };
    vector_ref<int> vecRef1(vec);
    vector_ref<const int> constVecRef1(vec);
    vector_ref<const int> constVecRef2(constVec);
    vector_ref<const int> constVecRef3(std::move(rvalueVec));
    BOOST_CHECK(vecRef1.data() == &vec[0] && vecRef1.size() == vec.size());
    BOOST_CHECK(constVecRef1.data() == &vec[0] && constVecRef1.size() == vec.size());
    BOOST_CHECK(constVecRef2.data() == &constVec[0] && constVecRef2.size() == constVec.size());
    BOOST_CHECK(constVecRef3.data() == &rvalueVec[0] && constVecRef3.size() == rvalueVec.size());

    // 通过POD数组构造
    int arr[] = { 1, 2, 3 };
    const int constArr[] = { 1, 2, 3 };
    vector_ref<int> arrRef1(arr);
    vector_ref<const int> constArrRef1(arr);
    vector_ref<const int> constArrRef2(constArr);
    BOOST_CHECK(arrRef1.data() == &arr[0] && arrRef1.size() == 3);
    BOOST_CHECK(constArrRef1.data() == &arr[0] && constArrRef1.size() == 3);
    BOOST_CHECK(constArrRef2.data() == &constArr[0] && constArrRef2.size() == 3);
}

BOOST_AUTO_TEST_CASE(croppedTest)
{
    // 空数组引用
    vector_ref<int> emptyRef;
    BOOST_CHECK(emptyRef.cropped(0).data() == nullptr && emptyRef.cropped(0).size() == 0);
    BOOST_CHECK(emptyRef.cropped(0, 10).data() == nullptr && emptyRef.cropped(0, 10).size() == 0);
    BOOST_CHECK_THROW(emptyRef.cropped(1), OutOfRange);
    BOOST_CHECK_THROW(emptyRef.cropped(1, 10), OutOfRange);

    // 非空数组引用
    int arr[] = { 1, 2, 3, 4, 5 };
    vector_ref<int> arrRef(arr);
    BOOST_CHECK(arrRef.cropped(2).data() == &arr[2] && arrRef.cropped(2).size() == 3);
    BOOST_CHECK(arrRef.cropped(2, 10).data() == &arr[2] && arrRef.cropped(2, 10).size() == 3);
    BOOST_CHECK(arrRef.cropped(arrRef.size()).size() == 0);
    BOOST_CHECK_THROW(arrRef.cropped(arrRef.size() + 1), OutOfRange);
}

BOOST_AUTO_TEST_CASE(toStringTest)
{
    std::string str = "hello world";
    vector_ref<char> strRef(str);
    vector_ref<const char> constStrRef(str);
    BOOST_CHECK(strRef.toString() == str);
    BOOST_CHECK(constStrRef.toString() == str);
}

BOOST_AUTO_TEST_SUITE_END()

}}   // namespace dev::test