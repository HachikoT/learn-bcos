#include <iostream>
#include <string>
#include <boost/program_options.hpp>
#include <BuildInfo.h>
#include <libconfig/Version.h>

namespace bpo = boost::program_options;

int main(int argc, char const *argv[])
{
    // 解析命令行参数
    bpo::options_description myOptions("Usage of " PROJECT_NAME " " PROJECT_VERSION);
    myOptions.add_options()
        ("help,h", "Print help information")
        ("version,v", "Print version information")
        ("config,c", bpo::value<std::string>(), "Config file path. Default config.ini")
    ;
    bpo::variables_map vMap;
    bpo::store(bpo::parse_command_line(argc, argv, myOptions), vMap);
    // 帮助信息
    if (vMap.count("help") || vMap.count("h")) {
        std::cout << myOptions << std::endl;
        return 0;
    }
    // 版本信息
    if (vMap.count("version") || vMap.count("v")) {
        dev::printVersion();
        return 0;
    }
    // 配置文件路径
    std::string configPath = "./config.ini";
    if (vMap.count("config") || vMap.count("c")) {
        configPath = vMap["config"].as<std::string>();
    }

    return 0;
}
