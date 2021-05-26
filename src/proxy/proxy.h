#pragma once

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <sys/socket.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>

#include "ClientManager.h"
#include "ConfigParser.h"
#include "aaa.h"
#include "utility.h"

class Proxy {
private:
    int sockfd;
    int client_sockfd;
    sockaddr_in proxy_addr;
    sockaddr_in proxy_addr_for_clients;
    sockaddr_in server_addr;

    char buffer[AAA_MAX_PACKET_SIZE];
    ssize_t bytes_received;
    ssize_t bytes_sent;

    std::string raw_http_response;
    std::shared_ptr<ClientManager> clientManager;
    std::map<std::string, configEntry> devices;

public:
    Proxy(std::shared_ptr<ClientManager> client);
    ~Proxy();
    void Run();
    int AcceptClient();
    void SendData(std::string request);
    void ReceiveData();
    ssize_t SendPacket(AAA::PacketType type, char count, std::string data);
    ssize_t ReceivePacket();

    std::string GetDeviceId(std::string rawPacket);
    void SetRecvTimeout(bool flag);
};
