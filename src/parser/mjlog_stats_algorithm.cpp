/**
 * @file        mjlog_stats_algorithm.cpp
 * @brief       统计算法库
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     存放所有main.cpp可以直接调用的统计算法
 */
#include "mjlog_stats_algorithm.h"


namespace Hasaki {

#define DEFINE_FUNC_NUM_OCCUR_TIME(func_name, func_impl, ...)   \
map<int32_t, int32_t>                                           \
func_name(const MjlogGameContainer &container)                  \
{                                                               \
    return func_impl(container, ##__VA_ARGS__);                 \
}

#define DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(func_name, func_impl, ...)   \
map<int32_t, map<int32_t, int32_t> >                                    \
func_name(const MjlogGameContainer &container)                          \
{                                                                       \
    return func_impl(container, ##__VA_ARGS__);                         \
}

auto PlayerCondTrue = [](const PlayerStatus &player) { return true; };
auto PlayerCondRichi = [](const PlayerStatus &player) { return player.richi; };
auto PlayerCondRichiOk = [](const PlayerStatus &player) { return player.richi_ok; };
auto PlayerCondFirstRichi = [](const PlayerStatus &player) { return player.first_richi; };
auto PlayerCondFirstRichiOk = [](const PlayerStatus &player) { return player.first_richi && player.richi_ok; };
auto PlayerCondChasingRichi = [](const PlayerStatus &player) { return player.chasing_richi; };
auto PlayerCondChasingRichiOk = [](const PlayerStatus &player) { return player.chasing_richi && player.richi_ok; };
auto PlayerCondBeChasedRichi = [](const PlayerStatus &player) { return player.be_chased_richi; };
auto PlayerCondBeChasedRichiOk = [](const PlayerStatus &player) { return player.be_chased_richi_ok; };

auto PlayerCondRon = [](const PlayerStatus &player) { return player.agari && player.agari_type != StatusAgariType::TSUMO; };
auto PlayerCondTsumo = [](const PlayerStatus &player) { return player.agari && player.agari_type == StatusAgariType::TSUMO; };
auto PlayerCondBeRon = [](const PlayerStatus &player) { return player.be_ron; };
auto PlayerCondBeTsumo = [](const PlayerStatus &player) { return player.be_tsumo; };
auto PlayerCondDraw = [](const PlayerStatus &player) { return player.draw && !player.agari; };
auto PlayerCondRyuukyoku = [](const PlayerStatus &player) { return player.ryuukyoku_type != 0; };


map<int32_t, int32_t> stats_richi_player_num(const MjlogGameContainer &container)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            int32_t num = 0;
            for (const auto &player: round_.m_players) {
                if (player.richi) {
                    num += 1;
                }
            }
            ans[num] += 1;
        }
    }
    return ans;
}


template <typename PlayerCond>
map<int32_t, int32_t> stats_richi_num_impl(const MjlogGameContainer &container, PlayerCond cond)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (cond(player)) {
                    ans[player.richi_num] += 1;
                }
            }
        }
    }
    return ans;
}

DEFINE_FUNC_NUM_OCCUR_TIME(stats_richi_num, stats_richi_num_impl, PlayerCondRichi)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_richi_ok_num, stats_richi_num_impl, PlayerCondRichiOk)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_first_richi_num, stats_richi_num_impl, PlayerCondFirstRichi)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_first_richi_ok_num, stats_richi_num_impl, PlayerCondFirstRichiOk)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_chasing_richi_num, stats_richi_num_impl, PlayerCondChasingRichi)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_chasing_richi_ok_num, stats_richi_num_impl, PlayerCondChasingRichiOk)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_be_chased_richi_num, stats_richi_num_impl, PlayerCondBeChasedRichi)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_be_chased_richi_ok_num, stats_richi_num_impl, PlayerCondBeChasedRichiOk)


template <typename PlayerCond1, typename PlayerCond2>
map<int32_t, map<int32_t, int32_t> >
stats_richi_gain_impl(const MjlogGameContainer &container, PlayerCond1 cond1, PlayerCond2 cond2)
{
    map<int32_t, map<int32_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (uint32_t i = 0; i < 4; ++i) {
                const auto &player = round_.m_players[i];
                if (cond1(player) && cond2(player)) {
                    int32_t init_ten = round_.m_init.m_ten[i];
                    auto &end_item = round_.m_ends[round_.get_last_end_idx()];
                    int32_t end_ten = end_item->m_sc[2 * i] + end_item->m_sc[2 * i + 1];
                    ans[player.richi_num][(end_ten - init_ten) * 100] += 1;
                }
            }
        }
    }
    return ans;
}


template <typename PlayerCond1, typename PlayerCond2>
map<int32_t, map<int32_t, int32_t> >
stats_richi_rate_impl(const MjlogGameContainer &container, PlayerCond1 cond1, PlayerCond2 cond2)
{
    map<int32_t, map<int32_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (cond1(player)) {
                    if (cond2(player)) {
                        ans[player.richi_num][1] += 1;
                    } else {
                        ans[player.richi_num][0] += 1;
                    }
                }
            }
        }
    }
    return ans;
}

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_gain, stats_richi_gain_impl, PlayerCondRichi, PlayerCondTrue)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ron_gain, stats_richi_gain_impl, PlayerCondRichi, PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_tsumo_gain, stats_richi_gain_impl, PlayerCondRichi, PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_ron_gain, stats_richi_gain_impl, PlayerCondRichi, PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_tsumo_gain, stats_richi_gain_impl, PlayerCondRichi, PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_draw_gain, stats_richi_gain_impl, PlayerCondRichi, PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ryuukyoku_gain, stats_richi_gain_impl, PlayerCondRichi, PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ron_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_tsumo_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_ron_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_tsumo_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_draw_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ryuukyoku_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_gain, stats_richi_gain_impl, PlayerCondFirstRichi, PlayerCondTrue)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_ron_gain, stats_richi_gain_impl, PlayerCondFirstRichi, PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_tsumo_gain, stats_richi_gain_impl, PlayerCondFirstRichi, PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_be_ron_gain, stats_richi_gain_impl, PlayerCondFirstRichi, PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_be_tsumo_gain, stats_richi_gain_impl, PlayerCondFirstRichi, PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_draw_gain, stats_richi_gain_impl, PlayerCondFirstRichi, PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_ryuukyoku_gain, stats_richi_gain_impl, PlayerCondFirstRichi, PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_ron_rate, stats_richi_rate_impl, PlayerCondFirstRichi, PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_tsumo_rate, stats_richi_rate_impl, PlayerCondFirstRichi, PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_be_ron_rate, stats_richi_rate_impl, PlayerCondFirstRichi, PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_be_tsumo_rate, stats_richi_rate_impl, PlayerCondFirstRichi, PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_draw_rate, stats_richi_rate_impl, PlayerCondFirstRichi, PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_first_richi_n_ryuukyoku_rate, stats_richi_rate_impl, PlayerCondFirstRichi, PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_gain, stats_richi_gain_impl, PlayerCondChasingRichi, PlayerCondTrue)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_ron_gain, stats_richi_gain_impl, PlayerCondChasingRichi, PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_tsumo_gain, stats_richi_gain_impl, PlayerCondChasingRichi, PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_be_ron_gain, stats_richi_gain_impl, PlayerCondChasingRichi, PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_be_tsumo_gain, stats_richi_gain_impl, PlayerCondChasingRichi, PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_draw_gain, stats_richi_gain_impl, PlayerCondChasingRichi, PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_ryuukyoku_gain, stats_richi_gain_impl, PlayerCondChasingRichi, PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_ron_rate, stats_richi_rate_impl, PlayerCondChasingRichi, PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_tsumo_rate, stats_richi_rate_impl, PlayerCondChasingRichi, PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_be_ron_rate, stats_richi_rate_impl, PlayerCondChasingRichi, PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_be_tsumo_rate, stats_richi_rate_impl, PlayerCondChasingRichi, PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_draw_rate, stats_richi_rate_impl, PlayerCondChasingRichi, PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_chasing_richi_n_ryuukyoku_rate, stats_richi_rate_impl, PlayerCondChasingRichi, PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_gain, stats_richi_gain_impl, PlayerCondBeChasedRichi, PlayerCondTrue)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_ron_gain, stats_richi_gain_impl, PlayerCondBeChasedRichi, PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_tsumo_gain, stats_richi_gain_impl, PlayerCondBeChasedRichi, PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_be_ron_gain, stats_richi_gain_impl, PlayerCondBeChasedRichi, PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_be_tsumo_gain, stats_richi_gain_impl, PlayerCondBeChasedRichi, PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_draw_gain, stats_richi_gain_impl, PlayerCondBeChasedRichi, PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_ryuukyoku_gain, stats_richi_gain_impl, PlayerCondBeChasedRichi, PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_ron_rate, stats_richi_rate_impl, PlayerCondBeChasedRichi, PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_tsumo_rate, stats_richi_rate_impl, PlayerCondBeChasedRichi, PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_be_ron_rate, stats_richi_rate_impl, PlayerCondBeChasedRichi, PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_be_tsumo_rate, stats_richi_rate_impl, PlayerCondBeChasedRichi, PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_draw_rate, stats_richi_rate_impl, PlayerCondBeChasedRichi, PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_be_chased_richi_n_ryuukyoku_rate, stats_richi_rate_impl, PlayerCondBeChasedRichi, PlayerCondRyuukyoku)


}   // namespace Hasaki
