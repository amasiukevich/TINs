//
// Created by asia on 21.05.2021.
//

#ifndef AAA_PROTOCOL_HTTPSERVERTHREAD_H
#define AAA_PROTOCOL_HTTPSERVERTHREAD_H

#include "ClientManager.h"
class HTTPServerThread {
private:
    int server_socket;
    std::shared_ptr<ClientManager> clientManager;
public:
    HTTPServerThread(std::shared_ptr<ClientManager> client);
    void run();
};

#endif  //AAA_PROTOCOL_HTTPSERVERTHREAD_H
