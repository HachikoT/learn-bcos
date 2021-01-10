# 构建类型，支持Debug Release RelWithDebInfo MinSizeRel，默认为RelWithDebInfo
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# C++标准，使用C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 是否构建并运行单元测试
option(WITH_TESTS "Build and run tests" ON)

# 是否构建检测代码覆盖率目标
option(WITH_COVERAGE "Test code coverage" OFF)

# 显示所有配置信息
macro(print_config)
    message("")
    message("------------------------------------------------------------------------")
    message("-- Configuring ${PROJECT_NAME} ${PROJECT_VERSION}${VERSION_SUFFIX}")
    message("------------------------------------------------------------------------")
    message("-- CMake              Cmake version and location   ${CMAKE_VERSION} (${CMAKE_COMMAND})")
    message("-- Compiler           C++ compiler version         ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
    message("-- TARGET_PLATFORM    Target platform              ${CMAKE_SYSTEM_NAME}")
    message("-- CMAKE_BUILD_TYPE   Build type                   ${CMAKE_BUILD_TYPE}")
    message("-- CMAKE_CXX_STANDARD C++ standard                 ${CMAKE_CXX_STANDARD}")
    message("-- WITH_TESTS         Build and run tests          ${WITH_TESTS}")
    message("-- WITH_COVERAGE      Test code coverage           ${WITH_COVERAGE}")
    message("------------------------------------------------------------------------")
    message("")
endmacro()