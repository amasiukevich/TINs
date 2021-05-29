#pragma once

#include <iostream>
#include <cstring>

#define AAA_MAX_PACKET_SIZE 512
#define AAA_MAX_COUNT 127

namespace AAA {

const char TYPE_MASK =  (char)0b11000000;
const char ACK_MASK =   (char)0b01000000;
const char DATA_MASK =  (char)0b10000000;
const char ERROR_MASK = (char)0b11000000;
const char SESSION_MASK = (char) 0b01111111;

enum PacketType {
    UNKNOWN,  // 00
    ACK,      // 01
    DATA,     // 10
    ERROR,    // 11
};

PacketType GetType(char header);

void SetType(char &header, PacketType type);

char GetCount(char* header);

void SetCount(char* header, char count);

char GetSessionId(char* header);

void SetSessionId(char* header, char sessionId);

}  // namespace AAA
