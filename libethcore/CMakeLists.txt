# 获取源文件和头文件列表
file(GLOB SRCS "*.cpp")
file(GLOB HEADERS "*.h")

# 添加库目标
add_library(ethcore STATIC ${SRCS} ${HEADERS})

# 添加依赖库
target_link_libraries(ethcore PUBLIC devcore)
target_link_libraries(ethcore PUBLIC crypto)