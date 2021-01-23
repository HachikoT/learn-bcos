# 添加CreateBuildInfo目标，并且设置每次make都会自动执行
add_custom_target(
    CreateBuildInfo ALL
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND ${CMAKE_COMMAND}
            -DBUILD_INFO_H_IN="${CMAKE_SOURCE_DIR}/cmake/templates/BuildInfo.h.in"
            -DBUILD_INFO_H="${CMAKE_BINARY_DIR}/include/BuildInfo.h"
            -DPROJECT_NAME="${PROJECT_NAME}"
            -DPROJECT_VERSION="${PROJECT_VERSION}"
            -DVERSION_SUFFIX="${VERSION_SUFFIX}"
            -DCMAKE_SYSTEM_NAME="${CMAKE_SYSTEM_NAME}"
            -DCMAKE_CXX_COMPILER_ID="${CMAKE_CXX_COMPILER_ID}"
            -DCMAKE_CXX_COMPILER_VERSION="${CMAKE_CXX_COMPILER_VERSION}"
            -DCMAKE_BUILD_TYPE="${CMAKE_BUILD_TYPE}"
            -P "${CMAKE_SOURCE_DIR}/cmake/scripts/CreateBuildInfo.cmake"
)

# 添加CreateBuildInfo动态生成的include目录到头文件搜索目录
include_directories(BEFORE ${CMAKE_BINARY_DIR}/include)