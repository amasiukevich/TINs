#include "utility.h"

std::vector<std::string> chunk_data(std::string data, size_t chunk_size) {
    std::vector<std::string> chunks;

    for (size_t i = 0; i < data.length(); i += chunk_size) {
        chunks.push_back(data.substr(i, chunk_size));
    }

    return chunks;
}
