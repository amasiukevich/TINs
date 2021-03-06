//  Created on 19.05.2021
//
//  author: Adrian Brodzik
//

#include "utility.h"

std::vector<std::string> chunk_data(std::string &data, size_t chunk_size) {
    std::vector<std::string> chunks;

    for (size_t i = 0; i < data.length(); i += chunk_size) {
        chunks.push_back(data.substr(i, chunk_size));
    }

    return chunks;
}

// Source: https://stackoverflow.com/a/2602060
rapidjson::Document load_config(const std::string &filepath) {
    rapidjson::Document doc;

    std::ifstream file(filepath);

    if (file.is_open()) {
        std::string json;

        file.seekg(0, std::ios::end);
        json.reserve(file.tellg());
        file.seekg(0, std::ios::beg);
        json.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

        doc.Parse(json.c_str());
    } else {
        throw std::runtime_error("Failed to open config file.");
    }

    file.close();

    return doc;
}

// Source: https://stackoverflow.com/a/14267455
std::vector<std::string> split_string(std::string s, std::string delimiter) {
    std::vector<std::string> items;

    size_t start = 0;
    size_t end = s.find(delimiter);

    while (end != std::string::npos) {
        items.push_back(s.substr(start, end - start));

        start = end + delimiter.length();
        end = s.find(delimiter, start);
    }

    items.push_back(s.substr(start, end));

    return items;
}

std::shared_ptr<spdlog::logger> init_logger(std::string deviceName) {
    std::shared_ptr<spdlog::logger> logger;
    try {
        logger = spdlog::basic_logger_mt(deviceName, "logs/" + deviceName + ".log");
    } catch (const spdlog::spdlog_ex &ex) {
        std::cout << "Failed to initialize logs: " << ex.what() << std::endl;
        exit(0);
    }
    return logger;
}

bool is_valid_filename(std::string filename) {
    if (filename.size() < 1 || filename.size() > 200) {
        return false;
    }

    if (!isalnum(filename[0])) {
        return false;
    }

    for (auto c : filename) {
        if (!isalnum(c) && c != '.' && c != '_') {
            return false;
        }
    }

    return true;
}

std::string file_to_string(std::string filename) {
    std::ifstream file(filename);
    std::string s;

    file.seekg(0, std::ios::end);
    s.reserve(file.tellg());
    file.seekg(0, std::ios::beg);

    s.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());

    return s;
}
