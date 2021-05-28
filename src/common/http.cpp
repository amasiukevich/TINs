#include "http.h"

namespace HTTP {

std::string Request::to_string() {
    std::string s = method + " " + path + "?" + query_string + " HTTP/" + version + "\r\n";

    for (auto a : header) {
        s += a.first + ": " + a.second + "\r\n";
    }

    s += "\r\n";

    return s;
}

std::string Response::to_string() {
    std::string s = "HTTP/" + version + " " + status_code + " " + status_text + "\r\n";

    for (auto a : header) {
        s += a.first + ": " + a.second + "\r\n";
    }

    if (body.size() > 0) {
        s += "Content-Length: " + std::to_string(body.size()) + "\r\n";
    }

    s += "\r\n" + body;

    return s;
}

}  // namespace HTTP
