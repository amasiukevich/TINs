#include "ConfigParser.h"

std::map<std::string, ConfigEntry> ConfigParser::parseConfig(std::string filename) {
    file = std::ifstream(filename);

    if (file.is_open()) {
        std::map<std::string, ConfigEntry> map;
        ConfigEntry entry;

        nextLine();

        std::string device = line;

        nextLine();

        auto delimiterPos = line.find('=');
        auto key = line.substr(0, delimiterPos);
        auto value = line.substr(delimiterPos + 1);

        if (key == "port") {
            int port = std::stoi(value);
            entry.port = port;
        } else {
            throw std::length_error("Expected key: port");
        }

        nextLine();

        delimiterPos = line.find('=');
        key = line.substr(0, delimiterPos);
        value = line.substr(delimiterPos + 1);

        if (key == "ip") {
            entry.device_addr = value;
        } else {
            throw std::length_error("Expected key: ip");
        }

        nextLine();

        delimiterPos = line.find('=');
        key = line.substr(0, delimiterPos);
        value = line.substr(delimiterPos + 1);

        if (key == "max_size") {
            int max_size = std::stoi(value);
            entry.max_msg_size = max_size;
        } else {
            throw std::length_error("Expected key: max_size");
        }

        map[device] = entry;

        return map;
    }

    throw std::length_error("Could not open file");
}

void ConfigParser::nextLine() {
    while (true) {
        if (!getline(file, line)) {
            throw std::length_error("Unexpected EOF while parsing config file");
        }

        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());

        if (!line.empty() && line[0] != '#') {
            break;
        }
    }
}
