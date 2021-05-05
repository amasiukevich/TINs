#include "server.h"

Server::Server() {
    std::cout << "server constructor" << std::endl;
}

Server::~Server() {
    std::cout << "server destructor" << std::endl;
}

void Server::Run() {
    std::cout << "server run" << std::endl;
}
