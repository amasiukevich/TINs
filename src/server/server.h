#pragma once

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#include "aaa.h"
#include "http.h"
#include "utility.h"

class Server {
private:
    int sockfd;

    sockaddr_in server_addr;
    sockaddr_in proxy_addr;

    char buffer[AAA_MAX_PACKET_SIZE];

    std::string raw_http_request;
    HTTP::Request http_request;

    std::string raw_http_response;
    HTTP::Response http_response;

public:
    Server();
    ~Server();
    void Run();
    ssize_t SendPacket(AAA::PacketType type, char count, std::string data);
    ssize_t ReceivePacket();
    bool ParseRequest();
    bool HandleRequest();
    void SendData(std::string data);
    void SetRecvTimeout(bool flag);
};
