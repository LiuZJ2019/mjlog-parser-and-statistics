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

#define DECLARE_FUNC_NUM_OCCUR_TIME(func_name)  \
map<int32_t, int32_t> func_name(const MjlogGameContainer &container);

#define DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(func_name)  \
map<int32_t, map<int32_t, int32_t> > func_name(const MjlogGameContainer &container);

#define DECLARE_FUNC_HAI_BIT_SET_TO_OCCUR_TIME(func_name)  \
map<int32_t, map<uint64_t, int32_t> > func_name(const MjlogGameContainer &container);


DECLARE_FUNC_NUM_OCCUR_TIME(stats_richi_player_num)                         // 立直的人数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_game_round)                               // 每局麻将有多少小局
DECLARE_FUNC_NUM_OCCUR_TIME(stats_round_continue_oya)                       // 连庄数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_round_end_type)                           // 终局类型（流局/立直荣和/立直自摸/默听荣和/默听自摸）

// 胡牌玩家的宝牌数（只算表宝牌和红宝牌）
DECLARE_FUNC_NUM_OCCUR_TIME(stats_agari_richi_ok_dora_num)                  // 胡牌+立直的宝牌数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_agari_first_richi_ok_dora_num)            // 胡牌+先制立直的宝牌数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_agari_chasing_richi_ok_dora_num)          // 胡牌+追立直的宝牌数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_agari_be_chased_richi_ok_dora_num)        // 胡牌+被追立直的宝牌数

// 立直巡目
DECLARE_FUNC_NUM_OCCUR_TIME(stats_richi_num)                                // 立直的巡目数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_richi_ok_num)                             // 立直的巡目数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_first_richi_num)                          // 先制立直的巡目数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_first_richi_ok_num)                       // 先制立直成功的巡目数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_chasing_richi_num)                        // 追立直的巡目数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_chasing_richi_ok_num)                     // 追立直的成功巡目数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_be_chased_richi_num)                      // 被追立直的巡目数
DECLARE_FUNC_NUM_OCCUR_TIME(stats_be_chased_richi_ok_num)                   // 被追立直成功的巡目数

// 立直收益
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_gain)                     // 第n巡立直的局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ron_gain)                 // 第n巡立直的荣和局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_tsumo_gain)               // 第n巡立直的自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_ron_gain)              // 第n巡立直的铳局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_tsumo_gain)            // 第n巡立直的被自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_draw_gain)                // 第n巡立直的纯横移动局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ryuukyoku_gain)           // 第n巡立直的流局的局收支

// 庄家立直收益
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_gain)                 // 第n巡庄家立直的局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_ron_gain)             // 第n巡庄家立直的荣和局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_tsumo_gain)           // 第n巡庄家立直的自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_be_ron_gain)          // 第n巡庄家立直的铳局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_be_tsumo_gain)        // 第n巡庄家立直的被自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_draw_gain)            // 第n巡庄家立直的纯横移动局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_ryuukyoku_gain)       // 第n巡庄家立直的流局的局收支

// 闲家立直收益
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_gain)             // 第n巡闲家立直的局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_ron_gain)         // 第n巡闲家立直的荣和局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_tsumo_gain)       // 第n巡闲家立直的自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_be_ron_gain)      // 第n巡闲家立直的铳局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_be_tsumo_gain)    // 第n巡闲家立直的被自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_draw_gain)        // 第n巡闲家立直的纯横移动局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_ryuukyoku_gain)   // 第n巡闲家立直的流局的局收支

// 立直结局
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ron_rate)                 // 第n巡立直的荣和率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_tsumo_rate)               // 第n巡立直的自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_ron_rate)              // 第n巡立直的铳率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_tsumo_rate)            // 第n巡立直的被自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_draw_rate)                // 第n巡立直的纯横移动率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ryuukyoku_rate)           // 第n巡立直的流局率

// 庄家立直结局
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_rate)                 // 第n巡庄家立直率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_ron_rate)             // 第n巡庄家立直的荣和率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_tsumo_rate)           // 第n巡庄家立直的自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_be_ron_rate)          // 第n巡庄家立直的铳率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_be_tsumo_rate)        // 第n巡庄家立直的被自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_draw_rate)            // 第n巡庄家立直的纯横移动率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_ryuukyoku_rate)       // 第n巡庄家立直的流局率

// 闲家立直结局
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_rate)             // 第n巡闲家立直的率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_ron_rate)         // 第n巡闲家立直的荣和率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_tsumo_rate)       // 第n巡闲家立直的自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_be_ron_rate)      // 第n巡闲家立直的铳率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_be_tsumo_rate)    // 第n巡闲家立直的被自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_draw_rate)        // 第n巡闲家立直的纯横移动率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_ryuukyoku_rate)   // 第n巡闲家立直的流局率

// 先制立直收益
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_gain)               // 第n巡先制立直的局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_ron_gain)           // 第n巡先制立直的荣和局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_tsumo_gain)         // 第n巡先制立直的自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_be_ron_gain)        // 第n巡先制立直的铳局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_be_tsumo_gain)      // 第n巡先制立直的被自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_draw_gain)          // 第n巡先制立直的纯横移动局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_ryuukyoku_gain)     // 第n巡先制立直的流局的局收支

// 先制立直结局
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_ron_rate)           // 第n巡先制立直的荣和率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_tsumo_rate)         // 第n巡先制立直的自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_be_ron_rate)        // 第n巡先制立直的铳率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_be_tsumo_rate)      // 第n巡先制立直的被自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_draw_rate)          // 第n巡先制立直的纯横移动率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_ryuukyoku_rate)     // 第n巡先制立直的流局率

// 追立直收益
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_gain)               // 第n巡追立直的局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_ron_gain)           // 第n巡追立直的荣和局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_tsumo_gain)         // 第n巡追立直的自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_be_ron_gain)        // 第n巡追立直的铳局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_be_tsumo_gain)      // 第n巡追立直的被自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_draw_gain)          // 第n巡追立直的纯横移动局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_ryuukyoku_gain)     // 第n巡追立直的流局的局收支

// 追立直结局
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_ron_rate)           // 第n巡追立直的荣和率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_tsumo_rate)         // 第n巡追立直的自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_be_ron_rate)        // 第n巡追立直的铳率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_be_tsumo_rate)      // 第n巡追立直的被自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_draw_rate)          // 第n巡追立直的纯横移动率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_ryuukyoku_rate)     // 第n巡追立直的流局率

// 被追立直收益
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_gain)               // 第n巡被追立直的局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_ron_gain)           // 第n巡被追立直的荣和局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_tsumo_gain)         // 第n巡被追立直的自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_be_ron_gain)        // 第n巡被追立直的铳局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_be_tsumo_gain)      // 第n巡被追立直的被自摸局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_draw_gain)          // 第n巡被追立直的纯横移动局收支
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_ryuukyoku_gain)     // 第n巡被追立直的流局的局收支

// 被追立直结局
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_ron_rate)           // 第n巡被追立直的荣和率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_tsumo_rate)         // 第n巡被追立直的自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_be_ron_rate)        // 第n巡被追立直的铳率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_be_tsumo_rate)      // 第n巡被追立直的被自摸率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_draw_rate)          // 第n巡被追立直的纯横移动率
DECLARE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_ryuukyoku_rate)     // 第n巡被追立直的流局率

DECLARE_FUNC_HAI_BIT_SET_TO_OCCUR_TIME(stats_richi_tenpai_content)              // 立直听哪些牌

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_PARSER_MJLOG_STATS_ALGORITHM_H_
