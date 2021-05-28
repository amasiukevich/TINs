#include "device.h"

Device::Device(std::string config_path, std::string id)
    : id(id) {
    config = load_config(config_path);


    if(!config["devices"].HasMember(id.c_str())){
        std::cerr<<id<<" not present in config file"<<std::endl;
        exit(0);
    }
    max_packet_size = config["devices"][id.c_str()]["max_aaa_size"].GetInt();

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << "Error socket" << std::endl;
        exit(-1);
    }

    memset(&device_addr, 0, sizeof(device_addr));
    device_addr.sin_family = AF_INET;
    device_addr.sin_port = htons(config["devices"][id.c_str()]["port"].GetInt());
    device_addr.sin_addr.s_addr = inet_addr(config["devices"][id.c_str()]["ip"].GetString());

    if (bind(sockfd, (const sockaddr *)&device_addr, sizeof(device_addr)) == -1) {
        std::cerr << "Error bind" << std::endl;
        exit(-1);
    }

    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(config["proxy"]["port_for_devices"].GetInt());
    proxy_addr.sin_addr.s_addr = inet_addr(config["proxy"]["ip_for_devices"].GetString());

    connect(sockfd, (const sockaddr *)&proxy_addr, sizeof(proxy_addr));
}

Device::~Device() {
}

[[noreturn]] void Device::Run() {
    while (true) {
        ssize_t bytes_received = ReceivePacket();

        if (bytes_received > 0) {
            AAA::PacketType type = AAA::GetType(buffer[0]);
            char count = AAA::GetCount(buffer[0]);

            if (type == AAA::PacketType::DATA) {
                std::cout << "Recv: DATA " << (int)count << std::endl;
                raw_http_request.append(buffer + 1, bytes_received - 1);
                SendPacket(AAA::PacketType::ACK, count, "");
            }

            if (count == 1) {
                if (ParseRequest()) {
                    raw_http_request = "";
                    std::cout << http_request.to_string();
                    if (HandleRequest()) {
                        SendData(http_response.to_string());
                    }
                }
            }
        } else {
            std::cerr << "Error: failed to receive." << std::endl;
        }
    }
}

ssize_t Device::SendPacket(AAA::PacketType type, char count, std::string data) {
    char header = 0;
    AAA::SetType(header, type);
    AAA::SetCount(header, count);

    std::string temp = header + data;

    if (temp.size() > AAA_MAX_PACKET_SIZE) {
        std::cerr << "Packet too large." << std::endl;
        return -1;
    }

    return sendto(sockfd, temp.c_str(), temp.size(), 0, (const sockaddr *)&proxy_addr, sizeof(proxy_addr));
}

ssize_t Device::ReceivePacket() {
    memset(buffer, 0, sizeof(buffer));
    return recv(sockfd, buffer, sizeof(buffer), 0);
}

bool Device::ParseRequest() {
    http_request = {};
    std::stringstream stream(raw_http_request);
    return SimpleWeb::RequestMessage::parse(stream, http_request.method, http_request.path, http_request.query_string, http_request.version, http_request.header);
}

bool Device::HandleRequest() {
    auto path_items = split_string(http_request.path, "/");

    if (path_items.size() >= 2 && path_items[1] != id) {
        http_response = HTTP::BAD_REQUEST;
        http_response.body = "Wrong device.";
        return true;
    }

    if (path_items.size() >= 3 && path_items[2] == "info") {
        http_response = HTTP::OK;
        http_response.header.emplace(std::make_pair("Content-Type", "application/json"));
        http_response.body = json_to_string(config["devices"][id.c_str()]);
        return true;
    }

    http_response = HTTP::BAD_REQUEST;
    return false;
}

void Device::SendData(std::string data) {
    SetRecvTimeout(true);

    auto data_chunks = chunk_data(data, max_packet_size - 1);

    if (data_chunks.size() > AAA_MAX_COUNT) {
        std::cerr << "Data is too long, too many fragments." << std::endl;
        return;
    }

    for (unsigned char i = 0; i < data_chunks.size();) {
        SendPacket(AAA::PacketType::DATA, data_chunks.size() - i, data_chunks[i]);

        if (ReceivePacket() > 0 && AAA::GetType(buffer[0]) == AAA::PacketType::ACK) {
            char count = AAA::GetCount(buffer[0]);
            std::cout << "Recv: ACK " << (int)count << std::endl;
            ++i;
        } else {
            std::cout << "Didn't get ACK. Sending again." << std::endl;
        }
    }

    SetRecvTimeout(false);
}

void Device::SetRecvTimeout(bool flag) {
    struct timeval timeout;
    timeout.tv_sec = flag ? 2 : 0;
    timeout.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}
