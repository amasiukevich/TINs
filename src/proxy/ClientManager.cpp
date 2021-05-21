//
// Created by asia on 20.05.2021.
//

#include "ClientManager.h"

void ClientManager::QueueUpClient(int socket_fd){
    boost::unique_lock<boost::mutex> lock(mutex);
    clients.push(socket_fd);
    cond.notify_one();
}   // mutex released automatically


// hangs if there is no clients waiting
int ClientManager::GetAwaitingClient(){
    boost::unique_lock<boost::mutex> lock(mutex);
    while(clients.empty()){
        cond.wait(lock);
    }
    int result = clients.front();
    clients.pop();
    return result;
} //lock released here