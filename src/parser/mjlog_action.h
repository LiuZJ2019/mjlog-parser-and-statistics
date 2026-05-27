/**
 * @file        mjlog_action.h
 * @brief       Action是一局对局的主体，用2字节压缩存储，详见parser/mjlog_meta_type.h
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     用了模板的技巧，避免了虚表的动态多态开销，采用静态多态进行调度
 *              没有使用std::variant，因为std::variant有额外的空间开销
 *              因为Action是函数的主体，优化Action可以大幅降低空间开销，提高缓存命中率
 */
#ifndef HASAKI_MJLOG_PARSER_PARSER_MJLOG_ACTION_H_
#define HASAKI_MJLOG_PARSER_PARSER_MJLOG_ACTION_H_

#include <map>
#include <string>
#include <cstdint>
#include <stdexcept>
#include "util/util.h"
#include "parser/mjlog_meta_type.h"


namespace Hasaki {

// Action的2字节版本的模板定义，其他派生类只需要实现from_xml和to_xml就行（为了避免虚函数/虚表的开销，不用继承）
template<MetaType Type>
struct Action2B {
    uint8_t m_type = static_cast<uint8_t>(Type);
    uint8_t m_value = 0;

    // XML解析接口（无默认实现）
    static Action2B from_xml(const XmlMap &m);
    void to_xml(string &str) const;
};

// 副露的Action在此基础上新增一个get_m的接口
template<MetaType Type>
struct ActionMeld {
    uint8_t m_type = static_cast<uint8_t>(Type);
    uint8_t m_value = 0;

    // 副露处理的接口，因为m被压缩编码到type里了，通过该接口可以解码出m
    uint16_t get_m() const noexcept;

    // XML解析接口，to_xml有默认实现
    static ActionMeld from_xml(const XmlMap &m);
    void to_xml(string &str) const {
        uint8_t who = (m_type - Type) & 0x03;
        str += R"(<N)";
        str += R"( who=")" + to_string(who) + R"(")";
        str += R"( m=")" + to_string(ActionMeld::get_m()) + R"(")";
        str += R"( />)";
    }
};

using ActionTUVW = Action2B<MetaType::TUVW>;            // 摸牌
using ActionDEFG = Action2B<MetaType::DEFG>;            // 切牌
using ActionChi = ActionMeld<MetaType::CHI>;            // 吃
using ActionPon = ActionMeld<MetaType::PON>;            // 碰
using ActionKaKan = ActionMeld<MetaType::KA_KAN>;       // 加杠
using ActionPei = ActionMeld<MetaType::PEI>;            // 拔北
using ActionAnKan = ActionMeld<MetaType::AN_KAN>;       // 暗杠
using ActionMinKan = ActionMeld<MetaType::MIN_KAN>;     // 明杠
using ActionRichi1 = Action2B<MetaType::RICHI1>;        // 立直宣言(切牌前)
using ActionRichi2 = Action2B<MetaType::RICHI2>;        // 立直成功(切牌后且该牌没铳)
using ActionBye = Action2B<MetaType::BYE>;              // 玩家离开牌局
using ActionReconnect = Action2B<MetaType::RECONNECT>;  // 玩家重连牌局
using ActionDora = Action2B<MetaType::DORA>;            // 翻宝牌

// 对外暴露的接口
union Action {
    uint8_t bytes[2]{};
    ActionTUVW get;
    ActionDEFG put;
    ActionChi chi;
    ActionPon pon;
    ActionKaKan ka_kan;
    ActionPei pei;
    ActionAnKan an_kan;
    ActionMinKan min_kan;
    ActionRichi1 richi1;
    ActionRichi2 richi2;
    ActionBye bye;
    ActionReconnect reconnect;
    ActionDora dora;

    static Action from_bytes(Buffer &q);
    void to_bytes(Buffer &q) const;
    static Action from_xml(const string &name, XmlMap &xmlMap);
    void to_xml(string &str) const;
    string str(StringType type) const;
    bool is_tuvw() const noexcept;
    bool is_defg() const noexcept;
    bool is_meld() const noexcept;
    bool is_richi() const noexcept;
    bool is_richi1() const noexcept;
    bool is_richi2() const noexcept;
    bool is_dora() const noexcept;
    bool is_normal() const noexcept;
    uint8_t get_who() const noexcept;

    bool operator==(const Action &action) const noexcept;
    bool operator!=(const Action &action) const noexcept;
};

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_PARSER_MJLOG_ACTION_H_
