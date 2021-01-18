# 获取源文件和头文件列表
file(GLOB SRCS "*.cpp")
file(GLOB HEADERS "*.h")

# 添加库目标
add_library(config STATIC ${SRCS} ${HEADERS})

# 添加依赖库
# target_link_libraries(config PUBLIC xxx)