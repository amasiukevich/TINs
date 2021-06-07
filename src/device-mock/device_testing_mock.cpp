// Created on 07.06.2021
//
// authors: Anton Masiukevich, Joanna Sokołowska
//


#include <iostream>
#include <string>
#include <fstream>
#include "common/utility.h"

#include "DeviceMock.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cout << "Usage: " << std::endl;
        std::cout << "    ./device-mock instructions.txt" << std::endl;
        return 0;
    }

    DeviceMock mock_device(argv[1]);

    mock_device.Run();

    return 0;
}