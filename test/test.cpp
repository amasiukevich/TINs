#include "gtest/gtest.h"
#include "../src/common/aaa.h"
#include "../src/common/utility.h"

int main(int argc, char *argv[]) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(AAA_Packet, test_get_sessionId){
    char header[2]{0};
    header[1] = (char) 85;
    ASSERT_EQ((char) 85, AAA::GetSessionId(header));
}

TEST(AAA_Packet, test_set_count){
    char header[2]{0};
    header[0] = (char)0b11000000;
    header[1] = (char)0b00111111;
    AAA::SetCount(header, 123);
    ASSERT_EQ(header[0], (char)(61 + 0b11000000));
    ASSERT_EQ(header[1], (char)(128 + 0b00111111));
}

TEST(AAA_Packet, test_get_count){
    char header[2]{0};
    header[0] = (char) 0b10110011;
    header[1] = (char) 0b11011011;
    ASSERT_EQ((char)103, AAA::GetCount(header));
}

TEST(AAA_Packet, test_set_sessionId){
    char header[2]{0};
    AAA::SetSessionId(header, (char) 123);
    ASSERT_EQ(header[1], (char)123);
}

TEST(AAA_Packet, test_get_type){
   char header = (char) 0b00101010;
    ASSERT_EQ(AAA::GetType(header), AAA::UNKNOWN);

    header = (char) 0b01010101;
    ASSERT_EQ(AAA::GetType(header), AAA::ACK);

    header = (char) 0b10010101;
    ASSERT_EQ(AAA::GetType(header), AAA::DATA);

    header = (char) 0b11010101;
    ASSERT_EQ(AAA::GetType(header), AAA::ERROR);
}

TEST(AAA_Packet, test_set_type){
    char header = (char) 0b11111111;
    AAA::SetType(header, AAA::ACK);
    ASSERT_EQ(AAA::ACK_MASK, header & AAA::TYPE_MASK);

    header = (char) 0b11111111;
    AAA::SetType(header, AAA::ERROR);
    ASSERT_EQ(AAA::ERROR_MASK, header & AAA::TYPE_MASK);

    header = (char) 0b11111111;
    AAA::SetType(header, AAA::DATA);
    ASSERT_EQ(AAA::DATA_MASK, header & AAA::TYPE_MASK);
}

TEST(UTIL, test_chunk_data){
    std::string data = "12345678901234567890";
    size_t chunk_size = 5;
    std::vector<std::string> chunked = chunk_data(data, chunk_size);
    std::string reconcat;
    for(auto & chunk : chunked){
        ASSERT_LE(chunk.size(), chunk_size);
        reconcat += chunk;
    }
    ASSERT_EQ(data, reconcat);
}

TEST(UTIL, test_split_string){
    std::string str1 = "/api/device0/info";
    std::vector<std::string > items1{"", "api", "device0", "info"};
    std::vector<std::string> result1 = split_string(str1, "/");

    ASSERT_EQ(items1.size(), result1.size());
    for(auto& it : result1){
        ASSERT_EQ(std::count(items1.begin(), items1.end(),it), 1);
    }


    std::string str2 = "/api";
    std::vector<std::string > items2{"", "api"};
    std::vector<std::string> result2 = split_string(str2, "/");

    ASSERT_EQ(items2.size(), result2.size());
    for(auto& it : result2){
        ASSERT_EQ(std::count(items2.begin(), items2.end(),it), 1);
    }

    std::string str3 = "aaaaaaaaa";
    std::vector<std::string> result3 = split_string(str3, "/");
    ASSERT_EQ(result3.size(), 1);
    ASSERT_EQ(result3[0], str3);
}