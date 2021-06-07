//Created on 06.06.2021
//
// author: Anton Masiukevich
//

#include <iostream>
#include <string>
#include <fstream>
#include "common/utility.h"

#include "ProxyMock.h"

int main(int argc, char ** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << std::endl;
        std::cout << "    ./proxy-mock instructions.txt" << std::endl;

        return 0;
    }

    ProxyMock mock(argv[1]);

    mock.Run();
    return 0;
}