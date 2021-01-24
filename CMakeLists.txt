# cmake版本要求
cmake_minimum_required(VERSION 3.7)

# 项目信息
project(
    learn-bcos
    VERSION 1.0.0
    LANGUAGES CXX
)

# 添加模块目录（CMAKE_CURRENT_SOURCE_DIR表示当前CMakeList.txt的路径）
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 基本设置
include(Options)
include(BuildInfo)
include(CompilerSettings)

# 下载安装依赖库
include(ProjectBoost)
include(ProjectJSONCPP)

# 添加子目录
add_subdirectory(libdevcore)
add_subdirectory(libcrypto)
add_subdirectory(libconfig)
add_subdirectory(${PROJECT_NAME})

# 构建并运行单元测试
if (WITH_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()