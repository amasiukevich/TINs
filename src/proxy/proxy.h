#pragma once

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/socket.h>

#include <iostream>

#include "utility.h"

class Proxy {
private:
    int sockfd;
    sockaddr_in proxy_addr;
    sockaddr_in server_addr;

    char buffer[8];
    ssize_t bytes_received;
    ssize_t bytes_sent;

public:
    Proxy();
    ~Proxy();
    void Run();
};
