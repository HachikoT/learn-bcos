# 获取源文件和头文件列表
file(GLOB SRCS "*.cpp")
file(GLOB HEADERS "*.h")

# 添加库目标
add_library(crypto STATIC ${SRCS} ${HEADERS})

# 添加依赖库
target_link_libraries(crypto PUBLIC devcore)
target_link_libraries(crypto PUBLIC SECP256K1)