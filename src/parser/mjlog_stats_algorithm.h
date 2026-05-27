/**
 * @file        mjlog_stats_algorithm.h
 * @brief       统计算法库
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     存放所有main.cpp可以直接调用的统计算法
 */
#ifndef HASAKI_MJLOG_PARSER_PARSER_MJLOG_STATS_ALGORITHM_H_
#define HASAKI_MJLOG_PARSER_PARSER_MJLOG_STATS_ALGORITHM_H_

#include "parser/mjlog_container.h"


namespace Hasaki {

// 立直的巡目数
map<int32_t, int32_t> stats_richi_ok_num(const MjlogGameContainer &container);

// 先制立直的巡目数
map<int32_t, int32_t> stats_first_richi_ok_num(const MjlogGameContainer &container);

// 追立直的巡目数
map<int32_t, int32_t> stats_chasing_richi_ok_num(const MjlogGameContainer &container);

// 被追立直的巡目数
map<int32_t, int32_t> stats_be_chased_richi_ok_num(const MjlogGameContainer &container);

// 第n巡立直的局收支
map<int32_t, map<int32_t, int32_t> > stats_richi_n_gain(const MjlogGameContainer &container);

// 第n巡立直的荣和率
map<int32_t, map<int32_t, int32_t> > stats_richi_n_ron_rate(const MjlogGameContainer &container);

// 第n巡立直的自摸率
map<int32_t, map<int32_t, int32_t> > stats_richi_n_tsumo_rate(const MjlogGameContainer &container);

// 第n巡立直的铳率
map<int32_t, map<int32_t, int32_t> > stats_richi_n_be_ron_rate(const MjlogGameContainer &container);

// 第n巡立直的被自摸率
map<int32_t, map<int32_t, int32_t> > stats_richi_n_be_tsumo_rate(const MjlogGameContainer &container);

// 第n巡立直的纯横移动
map<int32_t, map<int32_t, int32_t> > stats_richi_n_draw_rate(const MjlogGameContainer &container);

// 第n巡立直的流局率
map<int32_t, map<int32_t, int32_t> > stats_richi_n_ryuukyoku_rate(const MjlogGameContainer &container);

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_PARSER_MJLOG_STATS_ALGORITHM_H_
