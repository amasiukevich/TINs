#include "proxy.h"

int main() {
    std::shared_ptr<ClientManager> sharedPtr(new ClientManager);
    Proxy(sharedPtr).Run();
    return 0;
}
