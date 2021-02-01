# 导入插件
include(ExternalProject)
include(GNUInstallDirs)

# 下载构建安装依赖库
ExternalProject_Add(
    # 项目名称
    snappy
    # 项目根目录
    PREFIX ${CMAKE_SOURCE_DIR}/deps
    # 下载名
    DOWNLOAD_NAME snappy-1.1.8.tar.gz
    # 下载路径
    DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps/download
    # 下载链接（支持多源下载）
    URL https://github.com/google/snappy/archive/1.1.8.tar.gz
    # sha256哈希值校验
    URL_HASH SHA256=16b677f07832a612b0836178db7f374e414f94657c138e6993cbfc5dcc58651f
    # cmake命令
    CMAKE_COMMAND ${CMAKE_COMMAND}
    # cmake参数
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DSNAPPY_BUILD_TESTS=OFF
    # 日志记录
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
)

# 设置snappy的库路径和头文件目录
ExternalProject_Get_Property(snappy INSTALL_DIR)
set(SNAPPY_INCLUDE_DIR ${INSTALL_DIR}/include)
set(SNAPPY_LIBRARY ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}snappy${CMAKE_STATIC_LIBRARY_SUFFIX})
file(MAKE_DIRECTORY ${SNAPPY_INCLUDE_DIR})  # Must exist.

# Snappy库
add_library(Snappy STATIC IMPORTED GLOBAL)
set_property(TARGET Snappy PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SNAPPY_INCLUDE_DIR})
set_property(TARGET Snappy PROPERTY IMPORTED_LOCATION ${SNAPPY_LIBRARY})
add_dependencies(Snappy snappy)

# 取消临时定义
unset(INSTALL_DIR)