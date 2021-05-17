#include "proxy.h"

Proxy::Proxy() {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        std::cerr << "Error socket" << std::endl;
        return;
    }

    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(2000);
    proxy_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (const sockaddr *)&proxy_addr, sizeof(proxy_addr)) == -1) {
        std::cerr << "Error bind" << std::endl;
        return;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1999);
    inet_aton("127.0.0.1", (in_addr *)&server_addr.sin_addr.s_addr);
}

Proxy::~Proxy() {
}

void Proxy::Run() {
    std::string request = "GET /info HTTP/1.1\r\nHost: 127.0.0.1:1999\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n";

    for (auto chunk : chunk_data(request, 8)) {
        bytes_sent = sendto(sockfd, (const char *)chunk.c_str(), chunk.length(), 0, (const sockaddr *)&server_addr, sizeof(server_addr));

        memset(buffer, 0, sizeof(buffer));
        bytes_received = recvfrom(sockfd, buffer, sizeof(buffer), 0, (sockaddr *)&server_addr, NULL);

        if (bytes_received == 0) {
            std::cout << "Recv: EOF" << std::endl;
        }

        if (std::string(buffer) == "ACK") {
            std::cout << "Recv: ACK" << std::endl;
        } else {
            std::cerr << "Error: didn't receive ACK." << std::endl;
        }
    }

    if (bytes_sent == 8) {
        sendto(sockfd, "", 0, 0, (const sockaddr *)&server_addr, sizeof(server_addr));
    }
}
