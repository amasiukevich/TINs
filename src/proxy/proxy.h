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
#include "common/http.h"
#include "common/utility.h"
#include "exceptions.h"

class Proxy {
private:
    rapidjson::Document config;
    std::shared_ptr<spdlog::logger> logger;

    int sockfd;
    int client_sockfd;
    sockaddr_in proxy_addr_for_devices;
    sockaddr_in proxy_addr_for_clients;
    sockaddr_in device_addr;
    uint device_chunk_size;
    int current_session_id;
    std::map<std::string, char> session_ids;

    char buffer[AAA_MAX_PACKET_SIZE];
    ssize_t bytes_sent;

    std::string raw_http_response;

    void SetDeviceSocket(const std::string &device_id);

public:
    Proxy(std::string config_path);
    ~Proxy();
    [[noreturn]] void Run();
    int AcceptClient();
    void SendDataDevice(std::string data);
    void ReceiveDataDevice();
    ssize_t SendPacket(AAA::PacketType type, char count, char session, std::string data);
    ssize_t SendPacket(AAA::PacketType type, char count, char session, const char error_code);
    ssize_t ReceivePacket();
    std::string GetDeviceId(std::string rawPacket);
    void SetRecvTimeoutForDevices(bool flag);
};
