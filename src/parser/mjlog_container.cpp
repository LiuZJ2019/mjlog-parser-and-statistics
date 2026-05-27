/**
 * @file        mjlog_container.cpp
 * @brief       MjlogGameContainer是parser的对外接口，存放了全部牌谱数据库的信息，外部stats模块基于该接口进行统计
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     test_bytes_output/test_xml_output用于校验xml/.hskmjlog的转换是否满足自等性
 *              如果加了新功能要debug，可以使用这两个函数测试一下，目前ver.1针对2025年的四人凤凰桌（type=169）试过没问题
 */
#include "mjlog_container.h"


namespace Hasaki {

void MjlogGameContainer::read_bytes(Buffer &q)
{
    while (q.size() > 0) {
        m_games.emplace_back(MjlogGame::from_bytes(q));
    }
}

void MjlogGameContainer::write_bytes(Buffer &q, uint32_t start_idx, uint32_t end_idx) const
{
    if (end_idx == -1) {
        end_idx = m_games.size();
    }
    for (uint32_t i = start_idx; i < end_idx; ++i) {
        m_games[i].to_bytes(q);
    }
}

void MjlogGameContainer::read_one_xml(string &str)
{
    m_games.emplace_back(MjlogGame::from_xml(str));
}

void MjlogGameContainer::write_one_xml(string &str, uint32_t start_idx) const
{
    m_games[start_idx].to_xml(str);
}

bool MjlogGameContainer::operator==(const MjlogGameContainer &data) const noexcept
{
    return m_games == data.m_games;
}

bool MjlogGameContainer::operator!=(const MjlogGameContainer &data) const noexcept
{
    return !(*this == data);
}

// 测试from_bytes和to_bytes是否满足自等性，如果不满足，说明代码肯定有bug
bool MjlogGameContainer::test_bytes_output() const
{
    Buffer q;
    write_bytes(q);
    MjlogGameContainer container;
    container.read_bytes(q);
    if (*this == container) {
        return true;
    }
    cerr << "In MjlogGameContainer::test_bytes_output failure:";
    cerr << " m_games.size() = " << m_games.size();
    cerr << " container.m_games.size() = " << container.m_games.size() << endl;
    if (m_games.size() != container.m_games.size()) {
        return false;
    }
    for (uint32_t i = 0; i < m_games.size(); ++i) {
        if (m_games[i] != container.m_games[i]) {
            cerr << "first diff idx = " << i << endl;
            cerr << m_games[i].str(XML) << endl;
            cerr << container.m_games[i].str(XML) << endl;
            cerr << m_games[i].str(BYTE) << endl;
            cerr << container.m_games[i].str(BYTE) << endl;
            break;
        }
    }
    return false;
}

bool MjlogGameContainer::test_xml_output() const
{
    for (const auto &game: m_games) {
        string xml_str;
        game.to_xml(xml_str);
        MjlogGame new_game = MjlogGame::from_xml(xml_str);
        if (game != new_game) {
            cerr << "In MjlogGameContainer::test_bytes_output failure:";
            cerr << game.str(XML) << endl;
            cerr <<  new_game.str(BYTE) << endl;
            return false;
        }
    }
    return true;
}

}   // namespace Hasaki

