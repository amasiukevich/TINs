//
// Created by Joanna Sokołowska on 20.05.2021.
//

#ifndef AAA_PROTOCOL_CONFIGPARSER_H
#define AAA_PROTOCOL_CONFIGPARSER_H

#include <fstream>
#include <map>
#include <string>

struct configEntry{
    int max_msg_size;
    int port;
    std::string device_addr;
};

class ConfigParser{
public:
    std::map <std::string, configEntry> parseConfig(std::string configFile);
    ConfigParser() =default;
private:
    std::ifstream file;
    std::string line;

    void next_line();
};


#endif  //AAA_PROTOCOL_CONFIGPARSER_H
