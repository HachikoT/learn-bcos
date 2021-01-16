#include <iostream>
#include <libdevcore/Base64.h>

int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;

    std::cout << dev::toBase64Std(dev::Bytes()) << std::endl;

    return 0;
}
