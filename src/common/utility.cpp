#include "utility.h"

std::vector<std::string> chunk_data(std::string &data, size_t chunk_size) {
    std::vector<std::string> chunks;

    for (size_t i = 0; i < data.length(); i += chunk_size) {
        chunks.push_back(data.substr(i, chunk_size));
    }

    return chunks;
}

// Source: https://stackoverflow.com/a/2602060
rapidjson::Document load_config(std::string filepath) {
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
