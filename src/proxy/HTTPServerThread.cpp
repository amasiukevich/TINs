//
// Created by Joanna Sokołowska on 21.05.2021.
//

#include "HTTPServerThread.h"

#include <utility>
HTTPServerThread::HTTPServerThread(std::shared_ptr<ClientManager> client):clientManager(std::move(client)){
    //open server socket
}

void HTTPServerThread::run(){
    while(true){
        std::cout<<"HTTP thread Listening \n";
        sleep(2);
        //listen on socket
        //accept connection int incoming;
        // pass it on to manager clientManager->QueueUpClient(incoming);
    }
}

