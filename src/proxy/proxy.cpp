#include "proxy.h"

Proxy::Proxy() {
    std::cout << "proxy constructor" << std::endl;
}

Proxy::~Proxy() {
    std::cout << "proxy destructor" << std::endl;
}

void Proxy::Run() {
    std::cout << "proxy run" << std::endl;
}
