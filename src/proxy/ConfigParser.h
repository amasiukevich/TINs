#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>

struct ConfigEntry {
    int max_msg_size;
    int port;
    std::string device_addr;
};

class ConfigParser {
public:
    std::map<std::string, ConfigEntry> parseConfig(std::string filename);
    ConfigParser() = default;

private:
    std::ifstream file;
    std::string line;

    void nextLine();
};
