# 获取源文件和头文件列表
file(GLOB SRCS "*.cpp")
file(GLOB HEADERS "*.h")

# 添加库目标
add_library(devcore STATIC ${SRCS} ${HEADERS})

# 添加依赖库
target_link_libraries(devcore PUBLIC JSONCPP)
target_link_libraries(devcore PUBLIC Snappy)
target_link_libraries(devcore PUBLIC Boost::Log)