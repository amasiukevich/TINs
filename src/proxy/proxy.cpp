#include "proxy.h"

Proxy::Proxy() {
    std::string request = "GET /info HTTP/1.1\r\nHost: localhost:1337\r\nUser-Agent: curl/7.68.0\r\nAccept: */*";

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        std::cerr << "Error socket" << std::endl;
        return;
    }

    sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1337);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    for (auto chunk : chunk_data(request, 4)) {
        ssize_t res = sendto(sockfd, (const char *)chunk.c_str(), chunk.length(), MSG_CONFIRM, (const sockaddr *)&server_addr, sizeof(server_addr));
    }

    /*ssize_t res = sendto(sockfd, (const char *)request.c_str(), request.length(), MSG_CONFIRM, (const sockaddr *)&server_addr, sizeof(server_addr));

    if (res == -1) {
        std::cerr << "Error send" << std::endl;
        return;
    }*/
}

Proxy::~Proxy() {
}

void Proxy::Run() {
}
