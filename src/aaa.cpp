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

void SetType(char &header, PacketType type) {
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

char GetCount(char header) {
    return header & ~TYPE_MASK;
}

void SetCount(char &header, char count) {
    header = (header & TYPE_MASK) | (count & ~TYPE_MASK);
}

}  // namespace AAA
