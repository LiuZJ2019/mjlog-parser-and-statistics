/**
 * @file        mjlog_action.cpp
 * @brief       Action的实现，主要实现from_xml/to_xml/from_bytes/to_bytes接口
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     不采用继承，而是采用模板。from_bytes/to_bytes的接口是共用的，只需要实现from_xml/to_xml
 *              Action是对外暴露的接口，其他是实现，副露需要实现get_m接口，这个设计详见parser/mjlog_meta_type.h
 */
#include "parser/mjlog_action.h"


namespace Hasaki {

template<>
ActionTUVW ActionTUVW::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint8_t hai = get_u8_from_attrs(m, "hai");
    return {
        .m_type = static_cast<uint8_t>(MetaType::TUVW + who),
        .m_value = hai
    };
}

template<>
void ActionTUVW::to_xml(string &str) const
{
    str += "<"s + "TUVW"[m_type & 0x03] + to_string(m_value);
    str += "/>";
}

template<>
ActionDEFG ActionDEFG::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint8_t hai = get_u8_from_attrs(m, "hai");
    return {
        .m_type = static_cast<uint8_t>(MetaType::DEFG + who),
        .m_value = hai
    };
}

template<>
void ActionDEFG::to_xml(string &str) const
{
    str += "<"s + "DEFG"[m_type & 0x03] + to_string(m_value);
    str += "/>";
}

template<>
uint16_t ActionChi::get_m() const noexcept
{
    uint8_t m3_8 = m_value & 0x3f;
    uint8_t m10_11 = m_value >> 6;
    uint8_t m12_15 = ((m_type - MetaType::CHI) >> 2) & 0x0f;
    return 0x07 | (m3_8 << 3) | (m10_11 << 10) | (m12_15 << 12);
}

template<>
ActionChi ActionChi::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint16_t meld = get_u16_from_attrs(m, "m");
    uint8_t m3_8 = (meld >> 3) & 0x3f;
    uint8_t m10_11 = (meld >> 10) & 0x03;
    uint8_t m12_15 = meld >> 12;
    return {
        .m_type = static_cast<uint8_t>(MetaType::CHI + who + m12_15 * 4),   // CHI + who + m[12:15] * 4
        .m_value = static_cast<uint8_t>(m3_8 | (m10_11 << 6))               // m[3:8] m[10:11]
    };
}

template<>
uint16_t ActionPon::get_m() const noexcept
{
    uint8_t m0_1 = ((m_type - MetaType::PON) >> 2) % 3 + 1;
    uint8_t m2_4 = 0b010;
    uint8_t m5_6 = m_value & 0x03;
    uint8_t m9_14 = (m_value >> 2) & 0x3f;
    uint8_t m15 = ((m_type - MetaType::PON) >> 2) >= 3;
    return m0_1 | (m2_4 << 2) | (m5_6 << 5) | (m9_14 << 9) | (m15 << 15);
}

template<>
ActionPon ActionPon::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint16_t meld = get_u16_from_attrs(m, "m");
    uint8_t m0_1_ = (meld & 0x03) - 1;
    if (m0_1_ > 2) {
        throw runtime_error("error PON tag from self");
    }
    uint8_t m5_6 = (meld >> 5) & 0x03;
    uint8_t m9_14 = (meld >> 9) & 0x3f;
    uint8_t m15 = meld >> 15;
    return {
        .m_type = static_cast<uint8_t>(MetaType::PON + who + m0_1_ * 4 + m15 * 12), // PON + who + (m[0:1] - 1) * 4 + m[15] * 12
        .m_value = static_cast<uint8_t>(m5_6 | (m9_14 << 2))                        // m[5:6] m[9:14]
    };
}

template<>
uint16_t ActionKaKan::get_m() const noexcept
{
    uint8_t m0_1 = ((m_type - MetaType::KA_KAN) >> 2) % 3 + 1;
    uint8_t m2_4 = 0b100;
    uint8_t m5_6 = m_value & 0x03;
    uint8_t m9_14 = (m_value >> 2) & 0x3f;
    uint8_t m15 = ((m_type - MetaType::KA_KAN) >> 2) >= 3;
    return m0_1 | (m2_4 << 2) | (m5_6 << 5) | (m9_14 << 9) | (m15 << 15);
}

template<>
ActionKaKan ActionKaKan::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint16_t meld = get_u16_from_attrs(m, "m");
    uint8_t m0_1_ = (meld & 0x03) - 1;
    if (m0_1_ > 2) {
        throw runtime_error("error KA_KAN tag from self");
    }
    uint8_t m5_6 = (meld >> 5) & 0x03;
    uint8_t m9_14 = (meld >> 9) & 0x3f;
    uint8_t m15 = meld >> 15;
    return {
        .m_type = static_cast<uint8_t>(MetaType::KA_KAN + who + m0_1_ * 4 + m15 * 12),  // KA_KAN + who + (m[0:1] - 1) * 4 + m[15] * 12
        .m_value = static_cast<uint8_t>(m5_6 | (m9_14 << 2))                            // m[5:6] m[9:14]
    };
}

template<>
uint16_t ActionPei::get_m() const noexcept
{
    return 0x20;
}

template<>
ActionPei ActionPei::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint16_t meld = get_u16_from_attrs(m, "m");
    if (meld != 0x20) {
        throw runtime_error("error PEI tag from self");
    }
    return {
        .m_type = static_cast<uint8_t>(MetaType::PEI + who),
        .m_value = 0
    };
}

template<>
uint16_t ActionAnKan::get_m() const noexcept
{
    uint8_t m8_15 = m_value;
    return m8_15 << 8;
}

template<>
ActionAnKan ActionAnKan::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint16_t meld = get_u16_from_attrs(m, "m");
    uint8_t m8_15 = meld >> 8;
    return {
        .m_type = static_cast<uint8_t>(MetaType::AN_KAN + who), // AN_KAN + who
        .m_value = m8_15                                        // m[8:15]
    };
}

template<>
uint16_t ActionMinKan::get_m() const noexcept
{
    uint8_t m0_1 = ((m_type - MetaType::MIN_KAN) >> 2) + 1;
    uint8_t m8_15 = m_value;
    return m0_1 | (m8_15 << 8);
}

template<>
ActionMinKan ActionMinKan::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint16_t meld = get_u16_from_attrs(m, "m");
    uint8_t m0_1_ = (meld & 0x03) - 1;
    if (m0_1_ > 2) {
        throw runtime_error("error MIN_KAN tag from self");
    }
    uint8_t m8_15 = meld >> 8;
    return {
        .m_type = static_cast<uint8_t>(MetaType::MIN_KAN + who + m0_1_ * 4),    // MIN_KAN + who + (m[0:1] - 1) * 4
        .m_value = m8_15                                                        // m[8:15]
    };
}

template<>
ActionRichi1 ActionRichi1::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint8_t step = get_u8_from_attrs(m, "step");
    if (step != 1) {
        throw runtime_error("error RICHI1 tag: invalid step");
    }
    return {
        .m_type = static_cast<uint8_t>(MetaType::RICHI1 + who),
        .m_value = 0
    };
}

template<>
void ActionRichi1::to_xml(string &str) const
{
    uint8_t who = (m_type - MetaType::RICHI1) & 0x03;
    str += R"(<REACH who=")" + to_string(who) + R"(" step="1" />)";
}

template<>
ActionRichi2 ActionRichi2::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    uint8_t step = get_u8_from_attrs(m, "step");
    if (step != 2) {
        throw runtime_error("error RICHI2 tag: invalid step");
    }
    return {
        .m_type = static_cast<uint8_t>(MetaType::RICHI2 + who),
        .m_value = 0
    };
}

template<>
void ActionRichi2::to_xml(string &str) const
{
    uint8_t who = (m_type - MetaType::RICHI2) & 0x03;
    str += R"(<REACH who=")" + to_string(who) + R"(" step="2" />)";
}

template<>
ActionBye ActionBye::from_xml(const XmlMap &m)
{
    uint8_t who = get_u2_from_attrs(m, "who");
    return {
        .m_type = static_cast<uint8_t>(MetaType::BYE + who),
        .m_value = 0
    };
}

template<>
void ActionBye::to_xml(string &str) const
{
    uint8_t who = (m_type - MetaType::BYE) & 0x03;
    str += R"(<BYE who=")" + to_string(who) + R"(" />)";
}

template<>
ActionReconnect ActionReconnect::from_xml(const XmlMap &m)
{
    uint8_t who = 4;
    for (uint8_t i = 0; i < 4; ++i) {
        if (m.find("n" + to_string(i)) != m.end()) {
            if (who != 4) {
                throw runtime_error("error RECONNECT tag: multiple player reconnect");
            }
            who = i;
        }
    }
    if (who >= 4) {
        throw runtime_error("error RECONNECT tag: no invalid player reconnect");
    }
    return {
        .m_type = static_cast<uint8_t>(MetaType::RECONNECT + who),
        .m_value = 0
    };
}

template<>
void ActionReconnect::to_xml(string &str) const
{
    uint8_t who = (m_type - MetaType::RECONNECT) & 0x03;
    str += R"(<UN n)" + to_string(who) + R"(="" />)";
}

template<>
ActionDora ActionDora::from_xml(const XmlMap &m)
{
    uint8_t hai = get_u8_from_attrs(m, "hai");
    return {
        .m_type = MetaType::DORA,
        .m_value = hai
    };
}

template<>
void ActionDora::to_xml(string &str) const
{
    str += R"(<DORA hai=")" + to_string(m_value) + R"(" />)";
}


Action Action::from_bytes(Buffer &q)
{
    uint8_t type = q.front(); q.pop_front();
    uint8_t value = q.front(); q.pop_front();
    return {type, value};
}

void Action::to_bytes(Buffer &q) const
{
    q.push_back(bytes[0]);
    q.push_back(bytes[1]);
}

Action Action::from_xml(const string &name, XmlMap &xmlMap)
{
    if ('T' <= name[0] && name[0] <= 'W' && name.size() > 1 && '0' <= name[1] && name[1] <= '9') {
        xmlMap["who"] = static_cast<char>(name[0] - 'T' + '0');
        xmlMap["hai"] = name.substr(1);
        return {.get=ActionTUVW::from_xml(xmlMap)};
    } else if ('D' <= name[0] && name[0] <= 'G' && name.size() > 1 && '0' <= name[1] && name[1] <= '9') {
        xmlMap["who"] = static_cast<char>(name[0] - 'D' + '0');
        xmlMap["hai"] = name.substr(1);
        return {.put=ActionDEFG::from_xml(xmlMap)};
    } else if (name == "N") {
        uint16_t m = get_u16_from_attrs(xmlMap, "m");
        if (m & 0x04) {
            return {.chi=ActionChi::from_xml(xmlMap)};
        } else if (m & 0x08) {
            return {.pon=ActionPon::from_xml(xmlMap)};
        } else if (m & 0x10) {
            return {.ka_kan=ActionKaKan::from_xml(xmlMap)};
        } else if (m & 0x20) {
            return {.pei=ActionPei::from_xml(xmlMap)};
        } else {
            if ((m & 0x03) == 0) {
                return {.an_kan=ActionAnKan::from_xml(xmlMap)};
            } else {
                return {.min_kan=ActionMinKan::from_xml(xmlMap)};
            }
        }
    } else if (name == "REACH") {
        uint8_t step = get_u2_from_attrs(xmlMap, "step");
        if (step == 1) {
            return {.richi1=ActionRichi1::from_xml(xmlMap)};
        } else if (step == 2) {
            return {.richi2=ActionRichi2::from_xml(xmlMap)};
        } else {
            throw runtime_error("Unknown REACH step in Action::from_xml: " + to_string(step));
        }
    } else if (name == "BYE") {
        return {.bye=ActionBye::from_xml(xmlMap)};
    } else if (name == "UN") {
        return {.reconnect=ActionReconnect::from_xml(xmlMap)};
    } else if (name == "DORA") {
        return {.dora=ActionDora::from_xml(xmlMap)};
    } else if (name == "AGARI" || name == "RYUUKYOKU") {
        return {0xff, 0xff};    // 如果是AGARI或者RYUUKYOKU，说明Action解析完毕，返回无效值提示调用者
    } else {
        throw runtime_error("Unknown tag in Action::from_xml: " + name);
    }
}

void Action::to_xml(string &str) const
{
    if (MetaType::TUVW <= bytes[0] && bytes[0] < MetaType::DEFG) {
        get.to_xml(str);
    } else if (MetaType::DEFG <= bytes[0] && bytes[0] < MetaType::CHI) {
        put.to_xml(str);
    } else if (MetaType::CHI <= bytes[0] && bytes[0] < MetaType::PON) {
        chi.to_xml(str);
    } else if (MetaType::PON <= bytes[0] && bytes[0] < MetaType::KA_KAN) {
        pon.to_xml(str);
    } else if (MetaType::KA_KAN <= bytes[0] && bytes[0] < MetaType::PEI) {
        ka_kan.to_xml(str);
    } else if (MetaType::PEI <= bytes[0] && bytes[0] < MetaType::AN_KAN) {
        pei.to_xml(str);
    } else if (MetaType::AN_KAN <= bytes[0] && bytes[0] < MetaType::MIN_KAN) {
        an_kan.to_xml(str);
    } else if (MetaType::MIN_KAN <= bytes[0] && bytes[0] < MetaType::RICHI1) {
        min_kan.to_xml(str);
    } else if (MetaType::RICHI1 <= bytes[0] && bytes[0] < MetaType::RICHI2) {
        richi1.to_xml(str);
    } else if (MetaType::RICHI2 <= bytes[0] && bytes[0] < MetaType::BYE) {
        richi2.to_xml(str);
    } else if (MetaType::BYE <= bytes[0] && bytes[0] < MetaType::RECONNECT) {
        bye.to_xml(str);
    } else if (MetaType::RECONNECT <= bytes[0] && bytes[0] < MetaType::DORA) {
        reconnect.to_xml(str);
    } else if (MetaType::DORA <= bytes[0] && bytes[0] < MetaType::AGARI) {
        dora.to_xml(str);
    } else {
        throw runtime_error("Unknown Action.type = " + to_string(bytes[0]));
    }
}

string Action::str(StringType type) const
{
    string ans;
    switch (type) {
        case StringType::XML: {
            to_xml(ans);
            break;
        }
        case StringType::BYTE: {
            Buffer q;
            to_bytes(q);
            ans = to_string(q);
            break;
        }
        default:
            break;
    }
    return ans;
}

bool Action::is_tuvw() const noexcept
{
    return MetaType::TUVW <= bytes[0] && bytes[0] < MetaType::DEFG;
}

bool Action::is_defg() const noexcept
{
    return MetaType::DEFG <= bytes[0] && bytes[0] < MetaType::CHI;
}

bool Action::is_meld() const noexcept
{
    return MetaType::CHI <= bytes[0] && bytes[0] < MetaType::RICHI1;
}

bool Action::is_richi() const noexcept
{
    return MetaType::RICHI1 <= bytes[0] && bytes[0] < MetaType::BYE;
}

bool Action::is_richi1() const noexcept
{
    return MetaType::RICHI1 <= bytes[0] && bytes[0] < MetaType::RICHI2;
}

bool Action::is_richi2() const noexcept
{
    return MetaType::RICHI2 <= bytes[0] && bytes[0] < MetaType::BYE;
}

bool Action::is_dora() const noexcept
{
    return bytes[0] == MetaType::DORA;
}

bool Action::is_normal() const noexcept
{
    return (MetaType::TUVW <= bytes[0] && bytes[0] < MetaType::BYE) || is_dora();
}

uint8_t Action::get_who() const noexcept
{
    return bytes[0] % 4;
}

bool Action::operator==(const Action &action) const noexcept
{
    return bytes[0] == action.bytes[0] && bytes[1] == action.bytes[1];
}

bool Action::operator!=(const Action &action) const noexcept
{
    return !(*this == action);
}

}   // namespace Hasaki
