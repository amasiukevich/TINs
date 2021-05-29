#include "proxy.h"

Proxy::Proxy(std::string config_path) {
    config = load_config(config_path);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << "Error socket" << std::endl;
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
        std::cerr << "Error bind for devcies" << std::endl;
        exit(-1);
    }

    if ((client_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << "Error client socket" << std::endl;
        exit(-1);
    }

    memset(&proxy_addr_for_clients, 0, sizeof(proxy_addr_for_clients));
    proxy_addr_for_clients.sin_family = AF_INET;
    proxy_addr_for_clients.sin_port = htons(config["proxy"]["port_for_clients"].GetInt());
    proxy_addr_for_clients.sin_addr.s_addr = inet_addr(config["proxy"]["ip_for_clients"].GetString());

    if (bind(client_sockfd, (const sockaddr *)&proxy_addr_for_clients, sizeof(proxy_addr_for_clients)) == -1) {
        std::cerr << "Error bind for clients" << std::endl;
        exit(-1);
    }

    if (listen(client_sockfd, config["proxy"]["client_queue_size"].GetInt()) == -1) {
        std::cerr << "Error listen" << std::endl;
        exit(-1);
    }
}

Proxy::~Proxy() {
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
                    set_device_data(device_id);
                    std::cout<<"Routing request to "<<device_id<<std::endl;
                    SendData(s);
                    ReceiveData();
                }else{
                    std::cout<<"Device unknown, sending back default response"<<std::endl;
                    HTTP::Response response = HTTP::BAD_GATEWAY;
                    raw_http_response = response.to_string();
                }
                send(connfd, raw_http_response.c_str(), raw_http_response.size(), 0);
                close(connfd);
            } else {
                std::cout << "TCP closed" << std::endl;
                break;
            }
        }
    }
}

int Proxy::AcceptClient() {
    sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);
    int connfd = accept(client_sockfd, (sockaddr *)&client_addr, &len);

    if (connfd == -1) {
        std::cerr << "Error accept" << std::endl;
        exit(-1);
    }

    return connfd;
}

void Proxy::SendData(std::string data) {
    SetRecvTimeout(true);

    auto data_chunks = chunk_data(data, device_chunk_size-2);

    if (data_chunks.size() > AAA_MAX_COUNT) {
        std::cerr << "Data is too long, too many fragments." << std::endl;
        return;
    }

    for (unsigned char i = 0; i < data_chunks.size();) {
        SendPacket(AAA::PacketType::DATA, data_chunks.size() - i, data_chunks[i]);

        if (ReceivePacket() > 0 && AAA::GetType(buffer[0]) == AAA::PacketType::ACK) {
            char16_t count = AAA::GetCount(buffer);
            std::cout << "Recv: ACK " << (int)count << std::endl;
            ++i;
        } else {
            std::cout << "Didn't get ACK. Sending again." << std::endl;
        }
    }

    SetRecvTimeout(false);
}

void Proxy::ReceiveData() {
    raw_http_response = "";

    while (true) {
        ssize_t bytes_received = ReceivePacket();

        if (bytes_received > 0) {
            AAA::PacketType type = AAA::GetType(buffer[0]);
            char16_t count = AAA::GetCount(buffer);

            if (type == AAA::PacketType::DATA) {
                std::cout << "Recv: DATA " << (int)count << std::endl;
                raw_http_response.append(buffer + 2, bytes_received - 2);
                SendPacket(AAA::PacketType::ACK, count, "");
            }

            if (count == 1) {
                std::cout << raw_http_response << std::endl;
                return;
            }
        } else {
            std::cerr << "Error: failed to receive." << std::endl;
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
        std::cerr << "Packet too large." << std::endl;
        return -1;
    }

    return sendto(sockfd, temp.c_str(), temp.size(), 0, (const sockaddr *)&device_addr, sizeof(device_addr));
}

ssize_t Proxy::ReceivePacket() {
    memset(buffer, 0, sizeof(buffer));
    return recv(sockfd, buffer, sizeof(buffer), 0);
}

std::string Proxy::GetDeviceId(std::string raw_packet) {
    std::string device_id = "";

    auto pos1 = raw_packet.find('/');
    auto remain_packet = raw_packet.substr(pos1 + 1);

    auto pos2 = remain_packet.find('/');
    device_id = remain_packet.substr(0, pos2);

    return device_id;
}

void Proxy::SetRecvTimeout(bool flag) {
    struct timeval timeout;
    timeout.tv_sec = flag ? 2 : 0;
    timeout.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}
void Proxy::set_device_data(std::string device_id) {
    memset(&device_addr, 0, sizeof (device_addr));
    device_addr.sin_family = AF_INET;
    device_addr.sin_port = htons(config["devices"][device_id.c_str()]["port"].GetInt());
    device_addr.sin_addr.s_addr = inet_addr(config["devices"][device_id.c_str()]["ip"].GetString());
    device_chunk_size =config["devices"][device_id.c_str()]["max_aaa_size"].GetUint();
    session_ids[device_id]++;
    current_session_id = session_ids[device_id];
}
