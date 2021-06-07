//
// Created by anton on 07.06.2021.
//

#ifndef AAA_PROTOCOL_DEVICEMOCK_H
#define AAA_PROTOCOL_DEVICEMOCK_H

#include <common/aaa.h>
#include <netinet/in.h>
#include <rapidjson/document.h>
#include <spdlog/logger.h>
#include <unistd.h>

#include <vector>
#include "common/utility.h"
#include "device/device.h"

class DeviceMock {

public:
    explicit DeviceMock(char const *);
    void Run();

private:

    rapidjson::Document config;
    std::shared_ptr<spdlog::logger> logger;

    std::vector<std::vector<std::string>> commands;

    std::string id = "1";

    int sockfd{};
    sockaddr_in device_addr;
    sockaddr_in proxy_addr;

    char buffer[AAA_MAX_PACKET_SIZE];

    ssize_t SendPacket(AAA::PacketType type, char count, char session_id, std::string data);
    ssize_t ReceivePacket();

    std::string getTypeAsStr(char);
};


#endif  //AAA_PROTOCOL_DEVICEMOCK_H
