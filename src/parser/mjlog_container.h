/**
 * @file        mjlog_container.h
 * @brief       MjlogGameContainer是parser的对外接口，存放了全部牌谱数据库的信息，外部stats模块基于该接口进行统计
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     test_bytes_output/test_xml_output用于校验xml/.hskmjlog的转换是否满足自等性
 *              如果加了新功能要debug，可以使用这两个函数测试一下，目前ver.1针对2025年的四人凤凰桌（type=169）试过没问题
 */
#ifndef HASAKI_MJLOG_PARSER_PARSER_MJLOG_CONTAINER_H_
#define HASAKI_MJLOG_PARSER_PARSER_MJLOG_CONTAINER_H_

#include "parser/mjlog_game.h"


namespace Hasaki {

struct MjlogGameContainer {
    vector<MjlogGame> m_games;

    void read_bytes(Buffer &q);
    void write_bytes(Buffer &q, uint32_t start_idx=0, uint32_t end_idx=UINT32_MAX) const;
    void read_one_xml(string &str);
    void write_one_xml(string &str, uint32_t start_idx=0) const;
    bool operator==(const MjlogGameContainer &data) const noexcept;
    bool operator!=(const MjlogGameContainer &data) const noexcept;

    bool test_bytes_output() const;
    bool test_xml_output() const;
};

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_PARSER_MJLOG_CONTAINER_H_
