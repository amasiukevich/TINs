//
// Created by asia on 20.05.2021.
//

#ifndef AAA_PROTOCOL_CONNECTION_MANAGER_H
#define AAA_PROTOCOL_CONNECTION_MANAGER_H

#include <semaphore.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <queue>

class ConnectionManager {
private:
    std::queue<int> clients;
    sem_t *client_queue;
    int current_client;

public:
    ConnectionManager();
    ~ConnectionManager();
    void QueueUpClient(int socket_fd);
    int GetAwaitingClient(int socket_fd);
};

#endif  //AAA_PROTOCOL_CONNECTION_MANAGER_H
