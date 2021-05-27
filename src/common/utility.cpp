#include "utility.h"

std::vector<std::string> chunk_data(std::string &data, size_t chunk_size) {
    std::vector<std::string> chunks;

    for (size_t i = 0; i < data.length(); i += chunk_size) {
        chunks.push_back(data.substr(i, chunk_size));
    }

    return chunks;
}

rapidjson::Document load_config(std::string filepath) {
    rapidjson::Document doc;

    std::ifstream file(filepath);

    if (file.is_open()) {
        std::string json;

        // Source: https://stackoverflow.com/a/2602060
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
