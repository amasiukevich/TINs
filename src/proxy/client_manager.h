//
// Created by Joanna Sokołowska on 20.05.2021.
//

#ifndef AAA_PROTOCOL_CLIENT_MANAGER_H
#define AAA_PROTOCOL_CLIENT_MANAGER_H

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

    void QueueUpClient(int socket_fd){
        boost::unique_lock<boost::mutex> lock(mutex);
        clients.push(socket_fd);
        cond.notify_one();
    }   // mutex released automatically

    // hangs if there is no clients waiting
    int GetAwaitingClient(){
        boost::unique_lock<boost::mutex> lock(mutex);
        while(clients.empty()){
            cond.wait(lock);
        }
        int result = clients.front();
        clients.pop();
        return result;
    } //lock released here

};

#endif  //AAA_PROTOCOL_CLIENT_MANAGER_H
