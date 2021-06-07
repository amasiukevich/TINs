//
// Created by anton on 07.06.2021.
//

#include "DeviceMock.h"
DeviceMock::DeviceMock(char const * filename) {
    std::ifstream file(filename);
    std::string line;
    std::string  device_name = "device1";

    std::vector<std::string> command;

    while (std::getline(file, line)) {
        commands.push_back(split_string(line, ","));
    }

    config = load_config("config/global.json");
    logger = init_logger(device_name + "-mock");

    logger->flush_on(spdlog::level::info);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        logger->error("Could not create socket");
        exit(-1);
    }

    // device address
    memset(&device_addr, 0, sizeof(device_addr));
    device_addr.sin_family = AF_INET;
    device_addr.sin_port = htons(config["devices"][device_name.c_str()]["port"].GetInt());
    device_addr.sin_addr.s_addr = inet_addr(config["devices"][device_name.c_str()]["ip"].GetString());

    if (bind(sockfd, (const sockaddr *)&device_addr, sizeof(device_addr)) == -1) {
        std::cerr<<strerror(errno);
        logger->error("Could not bind socket to device port");
        exit(-1);
    }

    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(config["proxy"]["port_for_devices"].GetInt());
    proxy_addr.sin_addr.s_addr = inet_addr(config["proxy"]["ip_for_devices"].GetString());
}


ssize_t DeviceMock::SendPacket(AAA::PacketType type, char count, char session_id, std::string data) {
    char header[2] {0};

    AAA::SetType(header[0], type);
    AAA::SetCount(header, count);
    AAA::SetSessionId(header, session_id);

    std::string temp = header[1] + data;
    temp = header[0] + temp;

    return sendto(sockfd, temp.c_str(), temp.size(), 0, (const sockaddr *) & proxy_addr, sizeof(proxy_addr));
}

ssize_t DeviceMock::ReceivePacket() {
    memset(buffer, 0, sizeof(buffer));
    auto received = recv(sockfd, buffer, sizeof(buffer), 0);
    logger->info("Received packet. Type {}, counter {}, session_id {}",
         getTypeAsStr(buffer[0]),
         (int) AAA::GetCount(buffer),
         (int) AAA::GetSessionId(buffer)
    );
    return received;
}



void DeviceMock::Run() {
    for (auto & command : commands) {

        if (command.size() == 2) {
            int microseconds = std::stoi(command[1]);
            usleep(microseconds);
        } else if (command.size() == 3) {

            AAA::PacketType type;
            if (command[0] == "ERROR") {
                type = AAA::PacketType::ERROR;
            } else if (command[0] == "ACK") {
                type = AAA::PacketType::ACK;
            } else if (command[0] == "DATA") {
                type = AAA::PacketType::DATA;
            } else {
                close(sockfd);
                throw std::runtime_error("Wrong packet type in instructions");
            }

            char session_id = (char) std::stoi(command[1]);
            char counter = (char) std::stoi(command[2]);

            ReceivePacket();
            logger->info("Sending packet {} with count and session id {}", command[0], (int) counter, (int) session_id);
            SendPacket(type, counter, session_id, "test");
        } else {
            close(sockfd);
            logger->flush();
            throw std::runtime_error("Unknown instructions");
        }
    }
    close(sockfd);
    logger->flush();
}

std::string DeviceMock::getTypeAsStr(char buffer) {
    auto type = AAA::GetType(buffer);

    if (type == AAA::PacketType::ACK) {
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