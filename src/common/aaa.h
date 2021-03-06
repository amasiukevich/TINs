//  Created on 19.05.2021
//
//  authors: Adrian Brodzik, Joanna Sokołowska
//

#pragma once

#include <cstring>
#include <iostream>

#define AAA_MAX_PACKET_SIZE 512
#define AAA_MAX_COUNT 127
#define AAA_RETRANSMISSION_TIMEOUT 80 * 1000  //80ms = 80000 microseconds
#define PROXY_PASSIVE_STATE_TIMEOUT 2.5       //2.5s
#define DEVICE_PASSIVE_STATE_TIMEOUT 1.5      //1.5s
#define AAA_MAX_RETRANSMISSIONS (int)1 * 1000 * 1000 / (AAA_RETRANSMISSION_TIMEOUT)
#define AAA_CLIENT_MAX_INACTIVE_TIME  10  //1s
#define AAA_HEADER_SIZE 2

namespace AAA {
namespace Error {

const char UNKNOWN = 0;
const char WRONG_TYPE = 1;
const char WRONG_COUNT = 2;
const char WRONG_SESSION = 3;

}  // namespace Error

const char TYPE_MASK = (char)0b11000000;
const char ACK_MASK = (char)0b01000000;
const char DATA_MASK = (char)0b10000000;
const char ERROR_MASK = (char)0b11000000;
const char SESSION_MASK = (char)0b01111111;

enum PacketType{
    UNKNOWN,  // 00
    ACK,      // 01
    DATA,     // 10
    ERROR,    // 11
};

PacketType GetType(char header);

void SetType(char &header, PacketType type);

char GetCount(char *header);

void SetCount(char *header, char count);

char GetSessionId(char *header);

void SetSessionId(char *header, char sessionId);

}  // namespace AAA
