#include "gtest/gtest.h"
#include "../src/proxy/proxy.h"


TEST(ModuleName, Testname) {
    ASSERT_EQ(1, 1);
}

int main(int argc, char* argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


TEST(Proxy, test_getting_device_id1) {

    Proxy proxy = Proxy(std::string("./src/config/global.json"));

    std::string raw_packet_str = std::string("GET /device0/info");

    std::string device_id = proxy.GetDeviceId(raw_packet_str);
    {}
    ASSERT_EQ(device_id, "device0");
}