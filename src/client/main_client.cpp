#include <iostream>
#include <sstream>

#include "client.h"
#include "rapidjson/document.h"

int main() {
    rapidjson::Document d;
    d.Parse("{\"project\":\"rapidjson\",\"stars\":10}");

    std::cout << d["stars"].GetInt() << std::endl;

    //Client().Run();
    return 0;
}
