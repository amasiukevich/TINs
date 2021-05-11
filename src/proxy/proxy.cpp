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
    inet_aton("127.0.0.1", (in_addr *)&server_addr.sin_addr.s_addr);

    char buffer[1024];
    socklen_t addr_len;

    for (auto chunk : chunk_data(request, 4)) {
        std::cout << "Sending chunk." << std::endl;

        sendto(sockfd, (const char *)chunk.c_str(), chunk.length(), MSG_CONFIRM, (const sockaddr *)&server_addr, sizeof(server_addr));
        recvfrom(sockfd, buffer, sizeof(buffer), 0, (sockaddr *)&server_addr, &addr_len);

        if (std::string(buffer) == "ACK") {
            std::cout << "Got ACK." << std::endl;
        } else {
            std::cerr << "Error, instead got: " << std::string(buffer) << std::endl;
        }
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
