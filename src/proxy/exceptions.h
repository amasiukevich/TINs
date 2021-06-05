//
// Created by asia on 05.06.2021.
//

#ifndef AAA_PROTOCOL_EXCEPTIONS_H
#define AAA_PROTOCOL_EXCEPTIONS_H
struct MaxRetransmissionsReachedException : public std::exception{
    [[nodiscard]] const char * what() const noexcept{
        return "Max number of retransmissions reached";
    }
};

#endif  //AAA_PROTOCOL_EXCEPTIONS_H
