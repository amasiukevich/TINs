//
// Created by anton on 07.06.2021.
//

#ifndef AAA_PROTOCOL_PROXYMOCK_H
#define AAA_PROTOCOL_PROXYMOCK_H

#include <common/aaa.h>
#include <netinet/in.h>
#include <rapidjson/document.h>
#include <spdlog/logger.h>
#include <unistd.h>

#include <vector>
#include "common/utility.h"
#include "proxy/proxy.h"

class ProxyMock {
    public:
        explicit ProxyMock(char const *);
        void Run();
    private:
        int sockfd{};
        sockaddr_in proxy_addr_for_devices{};
        sockaddr_in device_addr{};

        rapidjson::Document config{};
        std::shared_ptr<spdlog::logger> logger{};

        char buffer[AAA_MAX_PACKET_SIZE]{};

        std::vector<std::vector<std::string>> commands;

        ssize_t SendPacket(AAA::PacketType type, char count, char session, std::string data);
        ssize_t ReceivePacket();

        std::string getTypeAsStr(char);
};

#endif  //AAA_PROTOCOL_PROXYMOCK_H
