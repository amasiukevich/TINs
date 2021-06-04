#include "gtest/gtest.h"
#include "../src/common/aaa.h"

TEST(ModuleName, TestName) {
    ASSERT_EQ(1, 1);
}

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