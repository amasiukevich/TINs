#include "server.h"

Server::Server() {
    /*http.config.port = 1337;

    http.default_resource["GET"] = [](auto response, auto) {
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
    };

    http.default_resource["POST"] = [](auto response, auto) {
        *response << "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
    };

    http.resource["^/info$"]["GET"] = [](auto response, auto request) {
        std::stringstream stream;
        stream << "<h1>Request from " << request->remote_endpoint_address() << ":" << request->remote_endpoint_port() << "</h1>";

        stream << request->method << " " << request->path << " HTTP/" << request->http_version;

        stream << "<h2>Query Fields</h2>";
        auto query_fields = request->parse_query_string();
        for (auto &field : query_fields) {
            stream << field.first << ": " << field.second << "<br>";
        }

        stream << "<h2>Header Fields</h2>";
        for (auto &field : request->header) {
            stream << field.first << ": " << field.second << "<br>";
        }

        response->write(stream);
    };*/

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        std::cerr << "Error socket" << std::endl;
        return;
    }

    sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(1337);

    if (bind(sockfd, (const sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "Error bind" << std::endl;
        return;
    }

    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));

        ssize_t res = recv(sockfd, buffer, sizeof(buffer), 0);

        if (res == -1) {
            std::cerr << "Error recv" << std::endl;
        } else if (res == 0) {
            std::cout << "Recv EOF" << std::endl;
        } else {
            std::cout << "Server got: " << std::string(buffer) << std::endl;
        }
    }
}

Server::~Server() {
}

void Server::Run() {
    /*http_thread = std::thread([&]() {
        http.start();
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    SimpleWeb::Client<SimpleWeb::HTTP> client("localhost:1337");

    try {
        auto res = client.request("GET", "/info");
        std::cout << res->content.string() << std::endl;
    } catch (std::exception &) {
    }

    http_thread.join();*/
}
