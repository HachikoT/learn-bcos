# 添加根目录到头文件搜索目录
include_directories(BEFORE ${PROJECT_SOURCE_DIR})

# 设置可执行文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

# 设置编译器选项
if (("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU") OR ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang"))
    # 各个编译模式下共用的编译选项
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -pthread")

    # 构建测试代码覆盖率目标
    if (WITH_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
        find_program(LCOV_TOOL lcov)
        message(STATUS "lcov tool: ${LCOV_TOOL}")
        if (LCOV_TOOL)
            add_custom_target(
                coverage
                COMMAND ${LCOV_TOOL} -o ${CMAKE_BINARY_DIR}/coverage.info.in -c -d ${CMAKE_BINARY_DIR}/
                COMMAND ${LCOV_TOOL} -r ${CMAKE_BINARY_DIR}/coverage.info.in '/usr*' '${CMAKE_SOURCE_DIR}/deps*' '${CMAKE_SOURCE_DIR}/test*' -o ${CMAKE_BINARY_DIR}/coverage.info
                COMMAND genhtml -q -o ${CMAKE_BINARY_DIR}/CodeCoverage ${CMAKE_BINARY_DIR}/coverage.info
            )
        else()
            message(FATAL_ERROR "Can't find lcov tool. Please install lcov")
        endif()
    endif()

    # 各编译模式专有的编译选项
    set(CMAKE_CXX_FLAGS_DEBUG          "-Og -g")
    set(CMAKE_CXX_FLAGS_MINSIZEREL     "-Os -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE        "-O3 -DNDEBUG")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O2 -g")
else()
    message(FATAL_ERROR "Your compiler is not supported.")
endif()