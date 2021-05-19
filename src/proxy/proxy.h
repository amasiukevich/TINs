#pragma once

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "aaa.h"
#include "utility.h"

class Proxy {
private:
    int sockfd;
    sockaddr_in proxy_addr;
    sockaddr_in server_addr;

    char buffer[AAA_MAX_PACKET_SIZE];
    ssize_t bytes_received;
    ssize_t bytes_sent;

    std::string raw_http_response;

public:
    Proxy();
    ~Proxy();
    void Run();
    void SendData(std::string request);
    void ReceiveData();
    ssize_t SendPacket(AAA::PacketType type, char count, std::string data);
    ssize_t ReceivePacket();
};
