#pragma once

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <string.h>
#include <sys/socket.h>

#include <iostream>
#include <sstream>

#include "simple-web-server/utility.hpp"
#include "utility.h"

class Server {
private:
    int sockfd;
    sockaddr_in server_addr;
    sockaddr_in proxy_addr;

    char buffer[8];
    ssize_t bytes_received;
    ssize_t bytes_sent;

    std::string request;

public:
    Server();
    ~Server();
    void Run();
    void TryParseRequest();
};
