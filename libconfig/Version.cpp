/**
 * 项目版本信息
 * @file: Version.cpp
 * @author: rancheng <rc4work@163.com>
 * @date: 2021-01-18
 */
#include "Version.h"
#include <iostream>
#include <BuildInfo.h>

namespace dev {

// 打印项目版本信息
void printVersion() {
    std::cout << PROJECT_NAME << std::endl;
    std::cout << "Version:        " << PROJECT_VERSION << std::endl;
    std::cout << "Platform:       " << TAEGET_PLATFORM << std::endl;
    std::cout << "Compiler:       " << COMPILER << std::endl;
    std::cout << "Build Type:     " << BUILD_TYPE << std::endl;
    std::cout << "Build Time:     " << BUILD_TIME << std::endl;
    std::cout << "Git Branch:     " << GIT_BRANCH << std::endl;
    std::cout << "Git Commit:     " << GIT_COMMIT << std::endl;
}

}   // namespace dev