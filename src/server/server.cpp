#include "server.h"

Server::Server() {
    http.config.port = 1337;

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
    };
}

Server::~Server() {
}

void Server::Run() {
    http_thread = std::thread([&]() {
        http.start();
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));

    SimpleWeb::Client<SimpleWeb::HTTP> client("localhost:1337");

    try {
        auto res = client.request("GET", "/info");
        std::cout << res->content.string() << std::endl;
    } catch (std::exception &) {
    }

    http_thread.join();
}
