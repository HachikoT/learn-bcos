# 获取源文件和头文件列表
file(GLOB SRCS "*.cpp")
file(GLOB HEADERS "*.h")

# 添加可执行目标
add_executable(${PROJECT_NAME} ${SRCS} ${HEADERS})

# 添加依赖库