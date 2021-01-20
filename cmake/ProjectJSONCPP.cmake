# 导入插件
include(ExternalProject)
include(GNUInstallDirs)

# 下载安装依赖库
ExternalProject_Add(jsoncpp
    # 项目根目录
    PREFIX ${CMAKE_SOURCE_DIR}/deps
    # 下载名
    DOWNLOAD_NAME jsoncpp-1.9.4.tar.gz
    # 下载链接（支持多源下载）
    URL https://github.com/open-source-parsers/jsoncpp/archive/1.9.4.tar.gz
    # sha256哈希值校验
    URL_HASH SHA256=e34a628a8142643b976c7233ef381457efad79468c67cb1ae0b83a33d7493999
    # cmake命令
    CMAKE_COMMAND ${CMAKE_COMMAND}
    # cmake参数
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
               -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
               -DJSONCPP_WITH_TESTS=OFF
               -DJSONCPP_WITH_POST_BUILD_UNITTEST=OFF
               -DJSONCPP_WITH_PKGCONFIG_SUPPORT=OFF
               -DJSONCPP_WITH_CMAKE_PACKAGE=OFF
               -DBUILD_SHARED_LIBS=OFF
               -DBUILD_STATIC_LIBS=ON
               -DBUILD_OBJECT_LIBS=OFF
    # 日志记录
    LOG_CONFIGURE 1
    LOG_BUILD 1
    LOG_INSTALL 1
)

# jsoncpp的库路径和头文件目录
ExternalProject_Get_Property(jsoncpp INSTALL_DIR)
set(JSONCPP_INCLUDE_DIR ${INSTALL_DIR}/include)
set(JSONCPP_LIBRARY ${INSTALL_DIR}/${CMAKE_INSTALL_LIBDIR}/${CMAKE_STATIC_LIBRARY_PREFIX}jsoncpp${CMAKE_STATIC_LIBRARY_SUFFIX})
file(MAKE_DIRECTORY ${JSONCPP_INCLUDE_DIR})  # Must exist.

# JSONCPP库
add_library(JSONCPP STATIC IMPORTED GLOBAL)
set_property(TARGET JSONCPP PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${JSONCPP_INCLUDE_DIR})
set_property(TARGET JSONCPP PROPERTY IMPORTED_LOCATION ${JSONCPP_LIBRARY})
add_dependencies(JSONCPP jsoncpp)

# 取消临时定义
unset(INSTALL_DIR)