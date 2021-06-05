#include "proxy.h"

Proxy::Proxy(std::string config_path) {
    config = load_config(config_path);
    logger = init_logger("proxy");
    logger->info("Created proxy - device port:{}, client port:{}", config["proxy"]["port_for_clients"].GetInt(), config["proxy"]["port_for_devices"].GetInt());

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        logger->error("Error creating sockets");
        exit(-1);
    }

    for(auto &d : config["devices"].GetObject()){
        std::string name = d.name.GetString();
        session_ids[name] = (char)0;
    }

    memset(&proxy_addr_for_devices, 0, sizeof(proxy_addr_for_devices));
    proxy_addr_for_devices.sin_family = AF_INET;
    proxy_addr_for_devices.sin_port = htons(config["proxy"]["port_for_devices"].GetInt());
    proxy_addr_for_devices.sin_addr.s_addr = inet_addr(config["proxy"]["ip_for_devices"].GetString());

    if (bind(sockfd, (const sockaddr *)&proxy_addr_for_devices, sizeof(proxy_addr_for_devices)) == -1) {
        logger->error("Could not bind proxy address for devices");
        exit(-1);
    }

    if ((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        logger->error("Error creating client socket");
        exit(-1);
    }

    memset(&proxy_addr_for_clients, 0, sizeof(proxy_addr_for_clients));
    proxy_addr_for_clients.sin_family = AF_INET;
    proxy_addr_for_clients.sin_port = htons(config["proxy"]["port_for_clients"].GetInt());
    proxy_addr_for_clients.sin_addr.s_addr = inet_addr(config["proxy"]["ip_for_clients"].GetString());

    if (bind(client_sockfd, (const sockaddr *)&proxy_addr_for_clients, sizeof(proxy_addr_for_clients)) == -1) {
        logger->error("Could not bind proxy address for clients");
        exit(-1);
    }

    if (listen(client_sockfd, config["proxy"]["client_queue_size"].GetInt()) == -1) {
        logger->error("Client queue not empty on startup");
        exit(-1);
    }
    logger->flush();
}

Proxy::~Proxy() {
    logger->flush();
}

[[noreturn]] void Proxy::Run() {
    while (true) {
        int connfd = AcceptClient();

        while (true) {
            char buff[4096];
            memset(buff, 0, sizeof(buff));
            ssize_t bytes_received = recv(connfd, buff, sizeof(buff), 0);

            if (bytes_received > 0) {
                std::string s = std::string(buff);

                std::string device_id = GetDeviceId(s);
                if(config["devices"].HasMember(device_id.c_str())){
                    SetDeviceSocket(device_id);
                    logger->info("New client request. Routing to: {}", device_id);
                    try{
                        SendDataDevice(s);
                        ReceiveDataDevice();
                        logger->info("Communication successful, sending response back to client.");
                    }catch(MaxRetransmissionsReachedException &ignored){
                           logger->error("Max retransmissions reached. Sending back default response.");
                           HTTP::Response response = HTTP::SERVICE_UNAVAILABLE;
                           raw_http_response = response.to_string();
                    }
                }else{
                    logger->info("{} unknown, sending BAD::GATEWAY", device_id);
                    HTTP::Response response = HTTP::BAD_GATEWAY;
                    raw_http_response = response.to_string();
                }
                send(connfd, raw_http_response.c_str(), raw_http_response.size(), 0);
                close(connfd);
            } else {
                logger->info("TCP connection closed");
                break;
            }
        }
        logger->flush();
    }
}

int Proxy::AcceptClient() {
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int connfd = accept(client_sockfd, (sockaddr *)&client_addr, &len);

    if (connfd == -1) {
        logger->error("Error accepting client connection");
        exit(-1);
    }

    return connfd;
}

void Proxy::SendDataDevice(std::string data) {
    SetRecvTimeout(true);

    auto data_chunks = chunk_data(data, device_chunk_size-2);

    if (data_chunks.size() > AAA_MAX_COUNT) {
        logger->error("Data is too long, too many fragments.");
        return;
    }

    int retry_counter = 0;
    for (unsigned char i = 0; i < data_chunks.size();) {
        SendPacket(AAA::PacketType::DATA, data_chunks.size() - i, data_chunks[i]);

        if (ReceivePacket() > 0) {
            char type = AAA::GetType(buffer[0]);
            if (type == AAA::PacketType::ACK) {
                char incoming_session_id = AAA::GetSessionId(buffer);
                if (incoming_session_id == current_session_id) {
                    unsigned int ack_num = (unsigned char)AAA::GetCount(buffer);
                    if (ack_num == data_chunks.size() - i) {
                        logger->info("Session {}, ACK {} received and accepted", current_session_id, (int)ack_num);
                        ++i;
                    } else {
                        logger->info("Received ACK packet with wrong number. Got {}, expected{}. Resending data. Retransmission counter: {}",
                                     ack_num, data_chunks.size() - i, retry_counter);

                    }
                } else {
                    //todo send back error packet
                    logger->info("Recived packet with wrong session id. Received: {}, expected: {}. Ignored and resending. Retransmission counter: {}",
                                 (int)incoming_session_id, (int)current_session_id, retry_counter);
                    retry_counter++;
                }
            } else {
                //todo what is an appropriate reaction for wrong packet type? Does immediate resending comply with plans?
                logger->info("Received unexpected packet type {}. Incoming packet ignored. Resending. Retransmission counter: {}", (int)type, retry_counter);
                retry_counter++;
            }
        } else {
            logger->info("Timed out while waiting for ACK. Resending. Retransmission counter: {}", retry_counter);
            if (retry_counter > AAA_MAX_RETRANSMISSIONS) {
                logger->error("Received max retransmission number. Aborting...");
                throw MaxRetransmissionsReachedException();
            }
            retry_counter++;
        }
    }
    SetRecvTimeout(false);
}

void Proxy::ReceiveDataDevice(){
    raw_http_response = "";

    while (true) {
        ssize_t bytes_received = ReceivePacket();

        if (bytes_received > 0) {
            AAA::PacketType type = AAA::GetType(buffer[0]);
            char count = AAA::GetCount(buffer);

            if (type == AAA::PacketType::DATA) {
                logger->info("Session {}, DATA {} received and accepted", current_session_id, (int) count);
                raw_http_response.append(buffer + AAA_HEADER_SIZE, bytes_received - AAA_HEADER_SIZE);
                logger->info("Session {}, sending back ACK {}", current_session_id, (int) count);
                SendPacket(AAA::PacketType::ACK, count, "");
            }

            if (count == 1) {
                logger->info("Received full response from device: {}", raw_http_response);
                return;
            }
        } else {
            logger->error("Session id {} -  Failed to receive data", current_session_id);
        }
    }
}

ssize_t Proxy::SendPacket(AAA::PacketType type, char count, std::string data) {
    char header[2]{0};
    AAA::SetType(header[0], type);
    AAA::SetCount(header, count);
    AAA::SetSessionId(header, current_session_id);

    std::string temp = header[1] + data;
    temp =  header[0] + temp;

    if (temp.size() > device_chunk_size) {
        logger->error("Packet too large.");
        return -1;
    }

    return sendto(sockfd, temp.c_str(), temp.size(), 0, (const sockaddr *)&device_addr, sizeof(device_addr));
}

ssize_t Proxy::ReceivePacket() {
    memset(buffer, 0, sizeof(buffer));
    return recv(sockfd, buffer, sizeof(buffer), 0);
}

std::string Proxy::GetDeviceId(std::string raw_packet) {
    std::string device_id;

    auto pos1 = raw_packet.find('/');
    auto remain_packet = raw_packet.substr(pos1 + 1);

    auto pos2 = remain_packet.find('/');
    device_id = remain_packet.substr(0, pos2);

    return device_id;
}

void Proxy::SetRecvTimeout(bool flag) {
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = flag? AAA_RETRANSMISSION_TIMEOUT : 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}
void Proxy::SetDeviceSocket(const std::string& device_id) {
    memset(&device_addr, 0, sizeof (device_addr));
    device_addr.sin_family = AF_INET;
    device_addr.sin_port = htons(config["devices"][device_id.c_str()]["port"].GetInt());
    device_addr.sin_addr.s_addr = inet_addr(config["devices"][device_id.c_str()]["ip"].GetString());
    device_chunk_size =config["devices"][device_id.c_str()]["max_aaa_size"].GetUint();
    session_ids[device_id]++;
    current_session_id = session_ids[device_id];
}
