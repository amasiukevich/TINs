#include "gtest/gtest.h"
#include "../src/common/aaa.h"

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