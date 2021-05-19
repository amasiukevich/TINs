#include "proxy.h"

Proxy::Proxy() {
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        std::cerr << "Error socket" << std::endl;
        exit(-1);
    }

    memset(&proxy_addr, 0, sizeof(proxy_addr));
    proxy_addr.sin_family = AF_INET;
    proxy_addr.sin_port = htons(3000);
    proxy_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (const sockaddr *)&proxy_addr, sizeof(proxy_addr)) == -1) {
        std::cerr << "Error bind" << std::endl;
        exit(-1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2999);
    inet_aton("127.0.0.1", (in_addr *)&server_addr.sin_addr.s_addr);

    connect(sockfd, (const sockaddr *)&server_addr, sizeof(server_addr));
}

Proxy::~Proxy() {
}

void Proxy::Run() {
    while (true) {
        SendData("GET /info?test=1&ok=true HTTP/1.1\r\nHost: 127.0.0.1:1999\r\nUser-Agent: curl/7.68.0\r\nX-Test: Lorem ipsum dolor sit amet, consectetur adipiscing elit. Donec ultricies eleifend est, eget gravida magna ultricies id. Praesent metus felis, tempus nec mollis sed, semper vel justo. Mauris aliquet ipsum ut tincidunt interdum. In vulputate leo id elementum venenatis. Nulla nec massa ipsum. Proin gravida gravida auctor. Fusce vulputate lobortis leo, consectetur pharetra felis cursus a.\r\nAccept: */*\r\n\r\n");
        ReceiveData();
        SendData("GET /info?test=1&ok=true HTTP/1.1\r\nHost: 127.0.0.1:1999\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\n\r\n");
        ReceiveData();
        break;
    }
}

void Proxy::SendData(std::string data) {
    auto data_chunks = chunk_data(data, AAA_MAX_DATA_SIZE);

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
        }
    }
}

void Proxy::ReceiveData() {
    raw_http_response = "";

    while (true) {
        ssize_t bytes_received = ReceivePacket();

        if (bytes_received > 0) {
            AAA::PacketType type = AAA::GetType(buffer[0]);
            char count = AAA::GetCount(buffer[0]);

            if (type == AAA::PacketType::DATA) {
                std::cout << "Recv: DATA " << (int)count << std::endl;
                raw_http_response.append(buffer + 1, bytes_received - 1);
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
    char header = 0;
    AAA::SetType(header, type);
    AAA::SetCount(header, count);

    std::string temp = header + data;

    if (temp.size() > AAA_MAX_PACKET_SIZE) {
        std::cerr << "Packet too large." << std::endl;
        return -1;
    }

    return sendto(sockfd, temp.c_str(), temp.size(), 0, (const sockaddr *)&server_addr, sizeof(server_addr));
}

ssize_t Proxy::ReceivePacket() {
    memset(buffer, 0, sizeof(buffer));
    return recvfrom(sockfd, buffer, sizeof(buffer), 0, NULL, NULL);
}
