#include "proxy.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    ./proxy config.json" << std::endl;
        return 0;
    }

    Proxy(std::string(argv[1])).Run();
}
