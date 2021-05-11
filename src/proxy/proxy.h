#pragma once

#include <netinet/ip.h>
#include <string.h>
#include <sys/socket.h>

#include <iostream>

#include "utility.h"

class Proxy {
public:
    Proxy();
    ~Proxy();
    void Run();
};
