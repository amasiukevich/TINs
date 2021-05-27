#pragma once

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>

#include "common/aaa.h"
#include "common/utility.h"
#include "http.h"

class Device {
private:
    rapidjson::Document config;
    std::string id;

    int sockfd;

    sockaddr_in device_addr;
    sockaddr_in proxy_addr;

    char buffer[AAA_MAX_PACKET_SIZE];

    std::string raw_http_request;
    HTTP::Request http_request;

    std::string raw_http_response;
    HTTP::Response http_response;

public:
    Device(std::string config_path, std::string id);
    ~Device();
    void Run();
    ssize_t SendPacket(AAA::PacketType type, char count, std::string data);
    ssize_t ReceivePacket();
    bool ParseRequest();
    bool HandleRequest();
    void SendData(std::string data);
    void SetRecvTimeout(bool flag);
};