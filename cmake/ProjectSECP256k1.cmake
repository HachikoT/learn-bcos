# 导入插件
include(ExternalProject)

# 下载构建安装依赖库
ExternalProject_Add(
    # 项目名称
    secp256k1
    # 项目根目录
    PREFIX ${CMAKE_SOURCE_DIR}/deps
    # 下载名
    DOWNLOAD_NAME secp256k1-328aaef2.tar.gz
    # 下载路径
    DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps/download
    # 下载链接（支持多源下载）
    URL https://github.com/bitcoin-core/secp256k1/archive/328aaef22a4378817dbc5b7f27cd6351858c0b6c.tar.gz
    # sha256哈希值校验
    URL_HASH SHA256=aea682ca98b46cc1b04971182ac8541dafc6a007d60fd516aff15d5d88fc0407
    # 在源码中构建
    BUILD_IN_SOURCE 1
    # 配置命令
    CONFIGURE_COMMAND ./autogen.sh
    # 构建命令
    BUILD_COMMAND ./configure
                  --prefix=<INSTALL_DIR>
                  --disable-shared
                  --enable-static
                  --disable-benchmark
                  --disable-coverage
                  --disable-tests
                  --disable-openssl-tests
                  --disable-exhaustive-tests
                  --enable-experimental
                  --enable-module-ecdh
                  --enable-module-recovery
                  --enable-module-extrakeys
                  --enable-module-schnorrsig
                  --without-PACKAGE
    # 安装命令
    INSTALL_COMMAND make install
    # 日志记录
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
)

# 设置secp256k1的库路径和头文件目录
ExternalProject_Get_Property(secp256k1 INSTALL_DIR)
set(SECP256K1_INCLUDE_DIR ${INSTALL_DIR}/include)
set(SECP256K1_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}secp256k1${CMAKE_STATIC_LIBRARY_SUFFIX})
file(MAKE_DIRECTORY ${SECP256K1_INCLUDE_DIR})  # Must exist.

# SECP256K1库
add_library(SECP256K1 STATIC IMPORTED GLOBAL)
set_property(TARGET SECP256K1 PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${SECP256K1_INCLUDE_DIR})
set_property(TARGET SECP256K1 PROPERTY IMPORTED_LOCATION ${SECP256K1_LIBRARY})
add_dependencies(SECP256K1 secp256k1)

# 取消临时定义
unset(INSTALL_DIR)