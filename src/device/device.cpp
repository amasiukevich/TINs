#include "device.h"

Device::Device(std::string config_path, std::string id)
    : id(id), session_id(-1) {
    config = load_config(config_path);
    logger = init_logger(id);

    if(!config["devices"].HasMember(id.c_str())){
        logger->error("Device {} not present in config file", id);
        exit(0);
    }
    max_packet_size = config["devices"][id.c_str()]["max_aaa_size"].GetUint();

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        logger->error("Could not create socket");
        exit(-1);
    }

    memset(&device_addr, 0, sizeof(device_addr));
    device_addr.sin_family = AF_INET;
    device_addr.sin_port = htons(config["devices"][id.c_str()]["port"].GetInt());
    device_addr.sin_addr.s_addr = inet_addr(config["devices"][id.c_str()]["ip"].GetString());

    if (bind(sockfd, (const sockaddr *)&device_addr, sizeof(device_addr)) == -1) {
        logger->error("Could not bind socket to device port");
        exit(-1);
    }

    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(config["proxy"]["port_for_devices"].GetInt());
    proxy_addr.sin_addr.s_addr = inet_addr(config["proxy"]["ip_for_devices"].GetString());

    connect(sockfd, (const sockaddr *)&proxy_addr, sizeof(proxy_addr));

    logger->info("Created device [{}] on port {}", id, config["devices"][id.c_str()]["port"].GetInt());
    logger->flush();
}

Device::~Device() {
    logger->flush();
}

[[noreturn]] void Device::Run() {
    while (true) {
        ssize_t bytes_received = ReceivePacket();

        if (bytes_received > 0) {
            AAA::PacketType type = AAA::GetType(buffer[0]);
            char count = AAA::GetCount(buffer);
            if (type == AAA::PacketType::DATA) {
                char curr_session_id = AAA::GetSessionId(buffer);
                if(session_id == -1){
                    session_id = curr_session_id;
                    logger->info("New session id {} DATA {} received.", session_id, (int)count);
                    raw_http_request.append(buffer + 1, bytes_received - 1);
                    SendPacket(AAA::PacketType::ACK, count, "");
                    logger->info("Sent ACK{}", (int) count);
                }else if(curr_session_id == session_id){
                    logger->info("Session id {} DATA {} received.", session_id, (int)count);
                    raw_http_request.append(buffer + 1, bytes_received - 1);
                    SendPacket(AAA::PacketType::ACK, count, "");
                    logger->info("Sent ACK{}", (int) count);
                }else{
                    std::string data;
                    data.append(buffer + 1, bytes_received - 1);
                    logger->info("Sending ERROR, for packet: {}", data);
                    SendPacket(AAA::ERROR, 0, "");
                }
            }

            if (count == 1) {
                if (ParseRequest()) {
                    raw_http_request = "";
                    std::cout << http_request.to_string();
                    if (HandleRequest()) {
                        SendData(http_response.to_string());
                    }
                }
                session_id = -1;
            }
        } else {
            logger->info("Error while receiving packet");
        }
    }
}

ssize_t Device::SendPacket(AAA::PacketType type, char count, std::string data) {
    char header[2]{0};
    AAA::SetType(header[0], type);
    AAA::SetCount(header, count);
    AAA::SetSessionId(header, session_id);

    std::string temp = header + data;

    if (temp.size() > max_packet_size) {
        logger->error("Packet too large");
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

    auto data_chunks = chunk_data(data, max_packet_size - 2);

    if (data_chunks.size() > AAA_MAX_COUNT) {
        logger->error("Data is too long, too many fragments.");
        return;
    }

    int retransmission_counter = 0;
    for (unsigned char i = 0; i < data_chunks.size();) {
        SendPacket(AAA::PacketType::DATA, data_chunks.size() - i, data_chunks[i]);

        if (ReceivePacket() > 0 && AAA::GetType(buffer[0]) == AAA::PacketType::ACK) {
            char16_t count = AAA::GetCount(buffer);
            logger->info("Received ACK{}", (int)count);
            ++i;
            retransmission_counter = 0;
        } else {
            logger->error("Didnt receive ACK. Resending");
            retransmission_counter ++;
        }
    }

    SetRecvTimeout(false);
}

void Device::SetRecvTimeout(bool flag) {
    struct timeval timeout;
    timeout.tv_sec = flag ? 0.8 : 0;
    timeout.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}
