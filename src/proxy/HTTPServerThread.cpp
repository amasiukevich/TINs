//
// Created by Joanna Sokołowska on 21.05.2021.
//

#include "HTTPServerThread.h"

#include <utility>
HTTPServerThread::HTTPServerThread(std::shared_ptr<ClientManager> client):clientManager(std::move(client)){
    //open server socket
}

void HTTPServerThread::run(){
    int c = 0;
    while(true){
        std::cout<<"Listening "<< c;
        clientManager->QueueUpClient(c);
        c++;
        sleep(2);
        //listen on socket
        //accept connection int incoming;
        // pass it on to manager clientManager->QueueUpClient(incoming);
    }
}

