# 获取编译时间
execute_process(
    COMMAND date "+%Y%m%d %H:%M:%S"
    OUTPUT_VARIABLE BUILD_TIME OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
)

# 获取git branch
execute_process(
    COMMAND git rev-parse --abbrev-ref HEAD
    OUTPUT_VARIABLE GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
)

# 获取git commit
execute_process(
    COMMAND git rev-parse HEAD
    OUTPUT_VARIABLE GIT_COMMIT OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET
)

# 动态生成BuildInfo.h
configure_file("${BUILD_INFO_H_IN}" "${BUILD_INFO_H}")