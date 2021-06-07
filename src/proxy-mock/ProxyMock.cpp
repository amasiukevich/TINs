#include "ProxyMock.h"

ProxyMock::ProxyMock(char const * fileName) {
    std::ifstream file(filename);
    std::string line;

    std::vector<std::string> command;

    while (std::getline(file, line)) {
        commands.push_back(split_string(line, ","));
    }

    config = load_config("config/global.json");
    logger = init_logger("proxy-mock");
    logger->flush_on(spdlog::level::info);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        logger->error("Error creating sockets");
        exit(-1);
    }

    memset(&proxy_addr_for_devices, 0, sizeof(proxy_addr_for_devices));
    proxy_addr_for_devices.sin_family = AF_INET;
    proxy_addr_for_devices.sin_port = htons(config["proxy"]["port_for_devices"].GetInt());
    proxy_addr_for_devices.sin_addr.s_addr = inet_addr(config["proxy"]["ip_for_devices"].GetString());
    if (bind(sockfd, (const sockaddr *)&proxy_addr_for_devices, sizeof(proxy_addr_for_devices)) == -1) {
        logger->error("Could not bind proxy address for devices");
        exit(-1);
    }

    memset(&device_addr, 0, sizeof(device_addr));
    device_addr.sin_family = AF_INET;
    device_addr.sin_port = htons(config["devices"]["device0"]["port"].GetInt())
    device_addr.sin_addr.s_addr = inet_addr(config["devices"]["device0"]["ip"].GetString());
}

ssize_t ProxyMock::SendPacket(AAA::PacketType type, char count, char session, std::string data) {
    char header[2] {0};

    AAA::SetType(header[0], type);
    AAA::SetCount(header, count);
    AAA::SetSessionId(header, session);

    std::string temp = header[1] + data;
    temp = header[0] + temp;

    return sendto(sockfd, temp.c_str(), temp.size(), 0, (const sockaddr *) &device_addr, sizeof(device_addr));
}

ssize_t ProxyMock::ReceivePacket() {
    memset(buffer, 0, sizeof(buffer));
    auto received = recv(sockfd, buffer, sizeof(buffer), 0);
    logger->info("Received packet. Type {}, counter {}, session_id {}", getTypeAsStr(buffer[0]), (int) AAA::GetCount(buffer), (int) AAA::GetSessionId(buffer));
    return received;
}


void ProxyMock::Run() {

    for (auto & cmd : commands) {
        if (cmd.size() == 2) {
            int microseconds = std::stoi(cmd[1]);
            usleep(microseconds);
        } else if (cmd.size() == 3) {
            AAA::PacketType type;
            if (cmd[0] == "ERROR") {
                type = AAA::PacketType::ERROR;
            } else if (cmd[0] == "ACK") {
                type = AAA::PacketType::ACK;
            } else if (cmd[0] == "DATA") {
                type = AAA::PacketType::DATA;
            } else {
                close(sockfd);
                throw std::runtime_error("Wrong packet type in instructions");
            }
            char session_id = (char) std::stoi(cmd[1]);
            char counter = (char) std::stoi(cmd[2]);

            logger->info("Sending packet {} with count {} and session id {}", cmd[0], (int) counter, (int) session_id);
            SendPacket(type, counter, session_id, "test");
            ReceivePacket();
        } else {
            close(sockfd);
            logger->flush();
            throw std::runtime_error("Unrecognized instructions");
        }
    }
    close(sockfd);
    logger->flush();
}

std::string ProxyMock::getTypeAsStr(char buffer) {
    auto type = AAA::GetType(buffer);
    if (type == AAA:;PacketType::ACK) {
        return "ACK";
    }

    if (type == AAA::PacketType::DATA) {
        return "DATA";
    }

    if (type == AAA::PacketType::ERROR) {
        return "ERROR";
    }
    return "UNKNOWN";
}