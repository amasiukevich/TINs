#include "server.h"

Server::Server() {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        std::cerr << "Error socket" << std::endl;
        return;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(1999);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (const sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error bind" << std::endl;
        return;
    }

    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(2000);
    inet_aton("127.0.0.1", (in_addr *)&proxy_addr.sin_addr.s_addr);
}

Server::~Server() {
}

void Server::Run() {
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(sockfd, buffer, sizeof(buffer), 0);

        if (bytes_received == -1) {
            std::cerr << "Recv: ERROR" << std::endl;
        } else if (bytes_received == 0) {
            std::cout << "Recv: EOF" << std::endl;
        } else {
            std::cout << "Recv: DATA, " << bytes_received << " bytes" << std::endl;
            request.append(buffer, bytes_received);
            bytes_sent = sendto(sockfd, "ACK\0", 4, 0, (const sockaddr *)&proxy_addr, sizeof(proxy_addr));
            std::cout << "Send: ACK" << std::endl;
        }

        if (bytes_received < 8) {
            TryParseRequest();
        }
    }
}

void Server::TryParseRequest() {
    std::cout << "Parsing..." << std::endl;
    std::stringstream ss(request);
    std::string method;
    std::string path;
    std::string query_string;
    std::string version;
    SimpleWeb::CaseInsensitiveMultimap header;

    bool res = SimpleWeb::RequestMessage::parse(ss, method, path, query_string, version, header);

    if (!res) {
        std::cerr << "Parsing error" << std::endl;
    }

    std::cout << method << std::endl;
    std::cout << path << std::endl;
    std::cout << query_string << std::endl;
    std::cout << version << std::endl;
}
