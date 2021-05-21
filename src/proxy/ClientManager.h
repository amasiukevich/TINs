//
// Created by Joanna Sokołowska on 20.05.2021.
//

#ifndef AAA_PROTOCOL_CLIENTMANAGER_H
#define AAA_PROTOCOL_CLIENTMANAGER_H

#include <semaphore.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <queue>
#include <boost/thread.hpp>

class ClientManager {
private:
    std::queue<int> clients;
    boost::mutex mutex;
    boost::condition_variable cond;

public:
    ClientManager() = default;
    ~ClientManager() = default;

    void QueueUpClient(int socket_fd);
    int GetAwaitingClient();

};

#endif  //AAA_PROTOCOL_CLIENTMANAGER_H
