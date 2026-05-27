#include <gtest/gtest.h>
#include "parser/mjlog_action.h"

using namespace std;
using namespace Hasaki;


template<class T>
void check_meld_impl(uint8_t who, uint16_t m)
{
    XmlMap xmlMap;
    xmlMap["who"] = to_string(who);
    xmlMap["m"] = to_string(m);
    auto action = T::from_xml(xmlMap);
    ASSERT_EQ(m, action.get_m());

    string xmlStr = "<N who="s + '"' + xmlMap["who"] + '"' + " m=" + '"' + xmlMap["m"] + '"' + " />";
    Action action1 = *reinterpret_cast<Action*>(&action);
    string actionStr = action1.str(StringType::XML);
    ASSERT_STREQ(xmlStr.c_str(), actionStr.c_str());

    deque<uint8_t> q;
    action1.to_bytes(q);
    Action action2 = Action::from_bytes(q);
    ASSERT_STREQ(action1.str(StringType::XML).c_str(), action2.str(StringType::XML).c_str());
    ASSERT_STREQ(action1.str(StringType::BYTE).c_str(), action2.str(StringType::BYTE).c_str());
    ASSERT_EQ(action1 == action2, true);
    ASSERT_EQ(action1 != action2, false);
}

TEST(TestParserAction, meld_impl)
{
    // 吃
    for (uint16_t m3_8 = 0; m3_8 < 64; m3_8++) {
        for (uint16_t m10_15 = 0; m10_15 < 64; m10_15++) {
            uint16_t m = 0x07 | (m3_8 << 3) | (m10_15 << 10);
            for (uint8_t who = 0; who < 4; who++) {
                check_meld_impl<ActionChi>(who, m);
            }
        }
    }

    // 碰
    for (uint16_t m0_1 = 1; m0_1 < 4; m0_1++) {
        for (uint16_t m5_6 = 0; m5_6 < 4; m5_6++) {
            for (uint16_t m9_15 = 0; m9_15 < 128; m9_15++) {
                uint16_t m = m0_1 | 0x08 | (m5_6 << 5) | (m9_15 << 9);
                for (uint8_t who = 0; who < 4; who++) {
                    check_meld_impl<ActionPon>(who, m);
                }
            }
        }
    }

    // 加杠
    for (uint16_t m0_1 = 1; m0_1 < 4; m0_1++) {
        for (uint16_t m5_6 = 0; m5_6 < 4; m5_6++) {
            for (uint16_t m9_15 = 0; m9_15 < 128; m9_15++) {
                uint16_t m = m0_1 | 0x10 | (m5_6 << 5) | (m9_15 << 9);
                for (uint8_t who = 0; who < 4; who++) {
                    check_meld_impl<ActionKaKan>(who, m);
                }
            }
        }
    }

    // 拔北
    for (uint8_t who = 0; who < 4; who++) {
        check_meld_impl<ActionPei>(who, 0b100000);
    }

    // 暗杠
    for (uint16_t m8_15 = 0; m8_15 < 256; m8_15++) {
        for (uint8_t who = 0; who < 4; who++) {
            uint16_t m = m8_15 << 8;
            check_meld_impl<ActionAnKan>(who, m);
        }
    }

    // 明杠
    for (uint16_t m0_1 = 1; m0_1 < 4; m0_1++) {
        for (uint16_t m8_15 = 0; m8_15 < 256; m8_15++) {
            for (uint8_t who = 0; who < 4; who++) {
                uint16_t m = m0_1 | (m8_15 << 8);
                check_meld_impl<ActionMinKan>(who, m);
            }
        }
    }
}
