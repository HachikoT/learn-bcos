# 导入插件
include(ExternalProject)
include(ProcessorCount)

# 获取cpu核数
ProcessorCount(CORES)
if(CORES EQUAL 0)
  set(CORES 1)
endif()

# 仅下载依赖库
ExternalProject_Add(
    # 项目名称
    boost
    # 项目根目录
    PREFIX ${CMAKE_SOURCE_DIR}/deps
    # 下载名
    DOWNLOAD_NAME boost_1_74_0.tar.bz2
    # 下载路径
    DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps/download
    # 下载链接（支持多源下载）
    URL https://dl.bintray.com/boostorg/release/1.74.0/source/boost_1_74_0.tar.bz2
    # sha256哈希值校验
    URL_HASH SHA256=83bfc1507731a0906e387fc28b7ef5417d591429e51e788417fe9ff025e116b1
    # 在源码中构建
    BUILD_IN_SOURCE 1
    # 配置命令
    CONFIGURE_COMMAND ./bootstrap.sh
    # 构建命令
    BUILD_COMMAND ./b2 stage
                  cxxflags=-std=c++11
                  variant=release
                  link=static
                  threading=multi
                  --layout=system
                  -j${CORES}
                  --with-chrono
                  --with-date_time
                  --with-filesystem
                  --with-log
                  --with-program_options
                  --with-random
                  --with-regex
                  --with-serialization
                  --with-system
                  --with-test
                  --with-thread
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

# 添加boost头文件到头文件搜索目录
include_directories(BEFORE ${BOOST_INCLUDE_DIR})

# Boost::ASIO
add_library(Boost::ASIO INTERFACE IMPORTED GLOBAL)
set_property(TARGET Boost::ASIO PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(
    TARGET Boost::ASIO PROPERTY INTERFACE_LINK_LIBRARIES
    Boost::DateTime Boost::Regex Boost::Serialization Boost::System
)
add_dependencies(Boost::ASIO boost)

# Boost::Chrono
add_library(Boost::Chrono STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::Chrono PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::Chrono PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}chrono${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::Chrono boost)

# Boost::DateTime
add_library(Boost::DateTime STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::DateTime PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::DateTime PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}date_time${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET Boost::DateTime PROPERTY INTERFACE_LINK_LIBRARIES Boost::Serialization)
add_dependencies(Boost::DateTime boost)

# Boost::Filesystem
add_library(Boost::Filesystem STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::Filesystem PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::Filesystem PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}filesystem${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET Boost::Filesystem PROPERTY INTERFACE_LINK_LIBRARIES Boost::System)
add_dependencies(Boost::Filesystem boost)

# Boost::Log
add_library(Boost::Log STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::Log PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::Log PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}log${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET Boost::Log PROPERTY INTERFACE_LINK_LIBRARIES Boost::Filesystem Boost::Thread)
add_dependencies(Boost::Log boost)

# Boost::ProgramOptions
add_library(Boost::ProgramOptions STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::ProgramOptions PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::ProgramOptions PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}program_options${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::ProgramOptions boost)

# Boost::Random
add_library(Boost::Random STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::Random PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::Random PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}random${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::Random boost)

# Boost::Regex
add_library(Boost::Regex STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::Regex PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::Regex PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}regex${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::Regex boost)

# Boost::Serialization
add_library(Boost::Serialization STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::Serialization PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::Serialization PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}serialization${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::Serialization boost)

# Boost::System
add_library(Boost::System STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::System PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::System PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}system${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::System boost)

# Boost::UnitTestFramework
add_library(Boost::UnitTestFramework STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::UnitTestFramework PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::UnitTestFramework PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}unit_test_framework${CMAKE_STATIC_LIBRARY_SUFFIX})
add_dependencies(Boost::UnitTestFramework boost)

# Boost::Thread
add_library(Boost::Thread STATIC IMPORTED GLOBAL)
set_property(TARGET Boost::Thread PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${BOOST_INCLUDE_DIR})
set_property(TARGET Boost::Thread PROPERTY IMPORTED_LOCATION ${BOOST_LIBRARY_PRE}thread${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET Boost::Thread PROPERTY INTERFACE_LINK_LIBRARIES Boost::Chrono Boost::DateTime Boost::Regex)
add_dependencies(Boost::Thread boost)

# 取消临时定义
unset(INSTALL_DIR)