//  Created on 05.05.2021
//
//  authors: Adrian Brodzik, Joanna Sokołowska
//

#include "device.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Usage:" << std::endl;
        std::cout << "    ./device config.json deviceID" << std::endl;
        return 0;
    }

    Device(std::string(argv[1]), std::string(argv[2])).Run();
}
