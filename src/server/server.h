#pragma once

#include <netinet/ip.h>
#include <string.h>
#include <sys/socket.h>

#include <iostream>

#include "simple-web-server/client_http.hpp"
#include "simple-web-server/server_http.hpp"
#include "utility.h"

class Server {
private:
    SimpleWeb::Server<SimpleWeb::HTTP> http;
    std::thread http_thread;

public:
    Server();
    ~Server();
    void Run();
};
