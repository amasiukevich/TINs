#include "aaa.h"


namespace AAA {

PacketType GetType(char header) {
    switch (header & TYPE_MASK) {
        case ACK_MASK:
            return ACK;
        case DATA_MASK:
            return DATA;
        case ERROR_MASK:
            return ERROR;
        default:
            std::cerr << "Invalid AAA packet type." << std::endl;
            return UNKNOWN;
    }
}

void SetType( char &header, PacketType type) {
    switch (type) {
        case UNKNOWN:
            std::cerr << "Invalid AAA packet type." << std::endl;
            break;
        case ACK:
            header = (header & ~TYPE_MASK) | ACK_MASK;
            break;
        case DATA:
            header = (header & ~TYPE_MASK) | DATA_MASK;
            break;
        case ERROR:
            header = (header & ~TYPE_MASK) | ERROR_MASK;
            break;
    }
}

char GetCount(char* header) {
    unsigned char upper = (header[0] & ~TYPE_MASK) << 1;
    unsigned char lower = ( (unsigned char) (header[1] & ~SESSION_MASK))>> 7;
    return upper | lower;
}

void SetCount(char* header, char count) {
    *header = (*header & TYPE_MASK) | (count>>1);
    header[1] = (header[1] & SESSION_MASK) | (count << 7);
}

char GetSessionId(char* header){
    return header[1] & SESSION_MASK;
}

void SetSessionId(char* header, char sessionId){
    header[1] = (header[1] & ~SESSION_MASK) | sessionId;
}

}  // namespace AAA
