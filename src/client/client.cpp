#include "client.h"

Client::Client() {
    std::cout << "client constructor" << std::endl;
}

Client::~Client() {
    std::cout << "client destructor" << std::endl;
}

void Client::Run() {
    std::cout << "client run" << std::endl;
}
