# 获取源文件和头文件列表
file(GLOB_RECURSE SRCS "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")

# 添加可执行目标
add_executable(test-${PROJECT_NAME} ${SRCS} ${HEADERS})

# 添加依赖库
target_link_libraries(test-${PROJECT_NAME} PUBLIC Boost::UnitTestFramework)
target_link_libraries(test-${PROJECT_NAME} PUBLIC devcore)

# 添加测试目标
add_test(NAME test-${PROJECT_NAME} WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND ./bin/test-${PROJECT_NAME})

# 自动运行测试
add_custom_command(TARGET test-${PROJECT_NAME} POST_BUILD WORKING_DIRECTORY ${CMAKE_BINARY_DIR} COMMAND ./bin/test-${PROJECT_NAME})