#pragma once

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

#include "common/aaa.h"
#include "common/utility.h"
#include "common/http.h"

class Proxy {
private:
    rapidjson::Document config;

    int sockfd;
    int client_sockfd;
    sockaddr_in proxy_addr_for_devices;
    sockaddr_in proxy_addr_for_clients;
    sockaddr_in device_addr;
    uint device_chunk_size;
    std::map<std::string, int> session_id;

    char buffer[AAA_MAX_PACKET_SIZE];
    ssize_t bytes_received;
    ssize_t bytes_sent;

    std::string raw_http_response;

    void set_device_data(std::string device_id);

public:
    Proxy(std::string config_path);
    ~Proxy();
    [[noreturn]] void Run();
    int AcceptClient();
    void SendData(std::string request);
    void ReceiveData();
    ssize_t SendPacket(AAA::PacketType type, char count, std::string data);
    ssize_t ReceivePacket();
    std::string GetDeviceId(std::string rawPacket);
    void SetRecvTimeout(bool flag);
};
