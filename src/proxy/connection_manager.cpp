//
// Created by asia on 20.05.2021.
//

#include "connection_manager.h"

ConnectionManager::ConnectionManager() {
    errno = 0;
    client_queue = sem_open("/client_queue", 0);
    if (client_queue == SEM_FAILED) {
        std::cerr << "An error has occurred while opening client queue semaphore: ";
        std::cerr << strerror(errno) << "\n";
        exit(1);
    }
}

ConnectionManager::~ConnectionManager() {
    sem_unlink("/client_queue");
}

void ConnectionManager::QueueUpClient(int socket_fd) {
    sem_wait(client_queue);
    clients.push(socket_fd);
    sem_post(client_queue);
}

int ConnectionManager::GetAwaitingClient(int socket_fd) {
    int result;

    sem_wait(client_queue);

    if (clients.empty()) {
        result = -1;
    } else {
        result = clients.front();
        clients.pop();
    }

    sem_post(client_queue);

    return result;
}
