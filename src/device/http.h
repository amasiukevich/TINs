#pragma once

#include <simple-web-server/utility.hpp>
#include <string>

namespace HTTP {

struct Request {
    std::string method;
    std::string path;
    std::string query_string;
    std::string version;
    SimpleWeb::CaseInsensitiveMultimap header;

    std::string to_string();
};

struct Response {
    std::string version;
    std::string status_code;
    std::string status_text;
    SimpleWeb::CaseInsensitiveMultimap header;
    std::string body;

    std::string to_string();
};

const Response BAD_REQUEST = {
    "1.1",
    "400",
    "Bad Request",
    {},
    "",
};

}  // namespace HTTP
