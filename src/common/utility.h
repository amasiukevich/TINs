#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <vector>

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

std::vector<std::string> chunk_data(std::string &data, size_t chunk_size);

rapidjson::Document load_config(std::string filepath);

template <typename T>
std::string json_to_string(T &o) {
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    o.Accept(writer);
    return sb.GetString();
}
