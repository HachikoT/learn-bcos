# 导入插件
include(ExternalProject)
include(ProcessorCount)

# 获取cpu核数
ProcessorCount(CORES)
if(CORES EQUAL 0)
  set(CORES 1)
endif()

# 配置命令
set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
# 构建命令
set(BOOST_BUILD_TOOL ./b2)
# 编译选项
set(BOOST_CXXFLAGS "cxxflags=-std=c++11")

# 下载构建安装依赖库
ExternalProject_Add(boost
    # 项目根目录
    PREFIX ${CMAKE_SOURCE_DIR}/deps
    # 下载名
    DOWNLOAD_NAME boost_1_74_0.tar.bz2
    # 下载链接（支持多源下载）
    URL https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.bz2
    # sha256哈希值校验
    URL_HASH SHA256=83bfc1507731a0906e387fc28b7ef5417d591429e51e788417fe9ff025e116b1
    # 在源码中构建
    BUILD_IN_SOURCE 1
    # 配置命令
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_COMMAND}
    # 构建命令
    BUILD_COMMAND ${BOOST_BUILD_TOOL} stage
                  ${BOOST_CXXFLAGS}
                  threading=multi
                  variant=release
                  link=static
                  --layout=system
                  --with-date_time
                  --with-regex
                  --with-serialization
                  --with-system
                  --with-test
                  -j${CORES}
    # 安装命令
    INSTALL_COMMAND ""
    # 日志记录
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
)

# boost库头文件目录和库目录
ExternalProject_Get_Property(boost SOURCE_DIR)
set(BOOST_INCLUDE_DIR ${SOURCE_DIR})
set(BOOST_LIBRARY_PRE ${SOURCE_DIR}/stage/lib/${CMAKE_STATIC_LIBRARY_PREFIX}boost_)

# Boost::Algorithm库
add_library(Boost::Algorithm INTERFACE IMPORTED GLOBAL)
set_property(TARGET Boost::Algorithm PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
add_dependencies(Boost::Algorithm boost)

# Boost::ASIO库
add_library(Boost::ASIO INTERFACE IMPORTED GLOBAL)
set_property(TARGET Boost::ASIO PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::ASIO PROPERTY INTERFACE_LINK_LIBRARIES Boost::DateTime Boost::Regex Boost::Serialization
                                                                  Boost::System OpenSSL::SSL)
add_dependencies(Boost::ASIO boost)

# Boost::DateTime库
add_library(Boost::DateTime STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::DateTime PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::DateTime PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}date_time${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET Boost::DateTime PROPERTY INTERFACE_LINK_LIBRARIES Boost::Serialization)
add_dependencies(Boost::DateTime boost)

# Boost::Functional库
add_library(Boost::Functional INTERFACE IMPORTED GLOBAL)
set_property(TARGET Boost::Functional PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
add_dependencies(Boost::Functional boost)

# Boost::Multiprecision库
add_library(Boost::Multiprecision INTERFACE IMPORTED GLOBAL)
set_property(TARGET Boost::Multiprecision PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
add_dependencies(Boost::Multiprecision boost)

# Boost::Regex库
add_library(Boost::Regex STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::Regex PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::Regex PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}regex${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::Regex boost)

# Boost::Serialization库
add_library(Boost::Serialization STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::Serialization PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::Serialization PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}serialization${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::Serialization boost)

# Boost::System库
add_library(Boost::System STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::System PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::System PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}system${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::System boost)

# Boost::UnitTestFramework库
add_library(Boost::UnitTestFramework STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::UnitTestFramework PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::UnitTestFramework PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}unit_test_framework${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::UnitTestFramework boost)

# 取消临时定义
unset(SOURCE_DIR)