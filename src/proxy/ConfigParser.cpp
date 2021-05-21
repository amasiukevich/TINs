//
// Created by Joanna Sokołowska on 20.05.2021.
//

#include "ConfigParser.h"

#include <iostream>
std::map<std::string, configEntry> ConfigParser::parseConfig(std::string configFile) {
    file = std::ifstream (configFile);
    if(file.is_open()){
        std::map<std::string, configEntry> map;
        struct configEntry structConfigEntry;
        next_line();

        //strings
        std::string device = line;
        next_line();


        auto delimiterPos = line.find('=');
        auto key = line.substr(0, delimiterPos);
        auto value = line.substr(delimiterPos + 1);

        if(key == "port"){
            int port = std::stoi(value);
            structConfigEntry.port = port;
        }else{
            throw std::length_error("Expected key: port");
        }

        next_line();
        delimiterPos = line.find('=');
        key = line.substr(0, delimiterPos);
        value = line.substr(delimiterPos + 1);

        if(key == "ip"){
            structConfigEntry.device_addr = value;
        }else{
            throw std::length_error("Expected key: ip");
        }
        next_line();
        delimiterPos = line.find('=');
        key = line.substr(0, delimiterPos);
        value = line.substr(delimiterPos + 1);

        if(key == "max_size"){
            int max_size = std::stoi(value);
            structConfigEntry.max_msg_size = max_size;
        }else{
            throw std::length_error("Expected key: max_size");
        }
        map[device] = structConfigEntry;
        return map;
    }
    throw std::length_error("Could not open file");
}


void ConfigParser::next_line() {
    while(true){
        if(! getline(file, line)){
            throw std::length_error("Unexpected EOF while parsing config file");
        }
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if(!line.empty() && line[0]!='#'){
            break;
        }
    }
}
