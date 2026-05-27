#include <gtest/gtest.h>
#include "util/util.h"
#include "parser/mjlog_game.h"

using namespace std;
using namespace Hasaki;

void check_mjlog_result_xml(const MjlogGame *result1)
{
    string str1;
    result1->to_xml(str1);
    MjlogGame result2 = MjlogGame::from_xml(str1);
    string str2;
    result2.to_xml(str2);
    if (str1 != str2) {
        cout << str1 << endl;
        cout << str2 << endl;
    }
    ASSERT_STREQ(str1.c_str(), str2.c_str());
}

void check_mjlog_result_bytes(const MjlogGame *result1)
{
    deque<uint8_t> b1;
    result1->to_bytes(b1);
    deque<uint8_t> b1_(b1);
    MjlogGame result2 = MjlogGame::from_bytes(b1_);
    deque<uint8_t> b2;
    result2.to_bytes(b2);
    ASSERT_EQ(b1.size(), b2.size());
    for (uint32_t i = 0; i < b1.size(); ++i) {
        ASSERT_EQ(b1[i], b2[i]);
    }
}


TEST(ParserTest, RYUUKYOKU_NM_case) {
    auto path = "./data/nm-case-2025010410gm-00a9-0000-b84156ef.xml";
    auto xmlStr = readAsUtf8String(path);
    MjlogGame result = MjlogGame::from_xml(xmlStr);
    check_mjlog_result_xml(&result);
    check_mjlog_result_bytes(&result);
}

TEST(ParserTest, RYUUKYOKU_YAO9_case) {
    auto path = "./data/yao9-case-2025010123gm-00a9-0000-e11f5a1d.xml";
    auto xmlStr = readAsUtf8String(path);
    MjlogGame result = MjlogGame::from_xml(xmlStr);
    check_mjlog_result_xml(&result);
    check_mjlog_result_bytes(&result);
}

TEST(ParserTest, RYUUKYOKU_KAZE4_REACH4_case) {
    auto path = "./data/kaze4-reach4-case-2025060922gm-00a9-0000-d42666a4.xml";
    auto xmlStr = readAsUtf8String(path);
    MjlogGame result = MjlogGame::from_xml(xmlStr);
    check_mjlog_result_xml(&result);
    check_mjlog_result_bytes(&result);
}

TEST(ParserTest, RYUUKYOKU_RON3_case) {
    auto path = "./data/ron3-case-2025010116gm-00a9-0000-12f4eaf6.xml";
    auto xmlStr = readAsUtf8String(path);
    MjlogGame result = MjlogGame::from_xml(xmlStr);
    check_mjlog_result_xml(&result);
    check_mjlog_result_bytes(&result);
}

TEST(ParserTest, RYUUKYOKU_KAN4_case) {
    auto path = "./data/kan4-case-2025012500gm-00a9-0000-c294f3d0.xml";
    auto xmlStr = readAsUtf8String(path);
    MjlogGame result = MjlogGame::from_xml(xmlStr);
    check_mjlog_result_xml(&result);
    check_mjlog_result_bytes(&result);
}

TEST(ParserTest, AGARI2_case) {
    auto path = "./data/agari2-case-2025010100gm-00a9-0000-2f9c2895.xml";
    auto xmlStr = readAsUtf8String(path);
    MjlogGame result = MjlogGame::from_xml(xmlStr);
    check_mjlog_result_xml(&result);
    check_mjlog_result_bytes(&result);
}

TEST(ParserTest, AGARI_YAKUMAN_case) {
    auto path = "./data/yakuman-case-2025010123gm-00a9-0000-4697b56c.xml";
    auto xmlStr = readAsUtf8String(path);
    MjlogGame result = MjlogGame::from_xml(xmlStr);
    check_mjlog_result_xml(&result);
    check_mjlog_result_bytes(&result);
}

TEST(ParserTest, MULTIPLE_AGARI_YAKUMAN_case) {
    auto path = "./data/multiple-yakuman-case2025040717gm-00a9-0000-22a5a0e9.xml";
    auto xmlStr = readAsUtf8String(path);
    MjlogGame result = MjlogGame::from_xml(xmlStr);
    check_mjlog_result_xml(&result);
    check_mjlog_result_bytes(&result);
}
