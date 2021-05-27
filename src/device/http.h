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

// Source: https://developer.mozilla.org/en-US/docs/Web/HTTP/Status
const Response OK = {"1.1", "200", "OK", {}, ""};

const Response BAD_REQUEST = {"1.1", "400", "Bad Request", {}, ""};
const Response UNAUTHORIZED = {"1.1", "401", "Unauthorized", {}, ""};
const Response NOT_FOUND = {"1.1", "404", "Not Found", {}, ""};

const Response INTERNAL_SERVER_ERROR = {"1.1", "500", "Internal Server Error", {}, ""};
const Response NOT_IMPLEMENTED = {"1.1", "501", "Not Implemented", {}, ""};
const Response BAD_GATEWAY = {"1.1", "502", "Bad Gateway", {}, ""};
const Response SERVICE_UNAVAILABLE = {"1.1", "503", "Service Unavailable", {}, ""};
const Response GATEWAY_TIMEOUT = {"1.1", "504", "Gateway Timeout", {}, ""};

}  // namespace HTTP
