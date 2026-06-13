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
#include "parser/mjlog_tenpai.h"


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

using PlayerCond = bool(const PlayerStatus &player);

#define PlayerAnd(cond1, cond2) \
    [](const PlayerStatus& player) -> bool { return cond1(player) && cond2(player); }

#define PlayerOr(cond1, cond2) \
    [](const PlayerStatus& player) -> bool { return cond1(player) || cond2(player); }

constexpr auto PlayerCondTrue = [](const PlayerStatus &player) { return true; };
constexpr auto PlayerCondOya = [](const PlayerStatus &player) { return player.oya; };
constexpr auto PlayerCondNotOya = [](const PlayerStatus &player) { return !player.oya; };
constexpr auto PlayerCondRichi = [](const PlayerStatus &player) { return player.richi; };
constexpr auto PlayerCondRichiOk = [](const PlayerStatus &player) { return player.richi_ok; };
constexpr auto PlayerCondFirstRichi = [](const PlayerStatus &player) { return player.first_richi; };
constexpr auto PlayerCondFirstRichiOk = [](const PlayerStatus &player) { return player.first_richi && player.richi_ok; };
constexpr auto PlayerCondChasingRichi = [](const PlayerStatus &player) { return player.chasing_richi; };
constexpr auto PlayerCondChasingRichiOk = [](const PlayerStatus &player) { return player.chasing_richi && player.richi_ok; };
constexpr auto PlayerCondBeChasedRichi = [](const PlayerStatus &player) { return player.be_chased_richi; };
constexpr auto PlayerCondBeChasedRichiOk = [](const PlayerStatus &player) { return player.be_chased_richi_ok; };

constexpr auto PlayerCondRon = [](const PlayerStatus &player) { return player.agari && player.agari_type != StatusAgariType::TSUMO; };
constexpr auto PlayerCondTsumo = [](const PlayerStatus &player) { return player.agari && player.agari_type == StatusAgariType::TSUMO; };
constexpr auto PlayerCondBeRon = [](const PlayerStatus &player) { return player.be_ron; };
constexpr auto PlayerCondBeTsumo = [](const PlayerStatus &player) { return player.be_tsumo; };
constexpr auto PlayerCondDraw = [](const PlayerStatus &player) { return player.draw && !player.agari; };
constexpr auto PlayerCondRyuukyoku = [](const PlayerStatus &player) { return player.ryuukyoku_type != 0; };


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

map<int32_t, int32_t> stats_game_round(const MjlogGameContainer &container)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        ans[static_cast<int32_t>(game.m_rounds.size())] += 1;
    }
    return ans;
}

map<int32_t, int32_t> stats_round_continue_oya(const MjlogGameContainer &container)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        for (uint32_t i = 0; i < game.m_rounds.size(); ++ i) {
            if (i + 1 == game.m_rounds.size()) {
                continue;
            }
            if (game.m_rounds[i].m_init.m_oya == game.m_rounds[i + 1].m_init.m_oya) {
                ans[1] += 1;
            } else {
                ans[0] += 1;
            }
        }
    }
    return ans;
}

map<int32_t, int32_t> stats_round_end_type(const MjlogGameContainer &container)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            if (round_.m_ends[0]->is_ryuukyoku()) {
                ans[0] += 1;    // 0: 流局
                continue;
            }
            for (const auto &end: round_.m_ends) {
                if (!end) {
                    break;
                }
                const EndAgari *agari = end->to_agari();
                bool is_tsumo = agari->is_tsumo();
                if (agari->is_richi()) {
                    ans[1 + is_tsumo] += 1;     // 立直荣和/自摸
                } else {
                    if (!agari->is_meld()) {
                        ans[3 + is_tsumo] += 1; // 门清默听荣和
                    } else {
                        ans[5 + is_tsumo] += 1; // 副露荣和
                    }
                }
            }
        }
    }
    return ans;
}


map<int32_t, int32_t> stats_agari_dora_num_impl(const MjlogGameContainer &container, PlayerCond cond)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            if (round_.m_ends[0]->is_ryuukyoku()) {
                continue;
            }
            for (const auto &end: round_.m_ends) {
                if (!end) {
                    break;
                }
                const EndAgari *agari = end->to_agari();
                if (cond(round_.m_players[agari->get_who()])) {
                    auto hai_flatten = agari->get_hai_flatten();
                    int32_t dora_cnt = agari->get_dora_cnt(hai_flatten) + agari->get_aka_dora_cnt(hai_flatten);
                    ++ ans[dora_cnt];
                }
            }
        }
    }
    return ans;
}

DEFINE_FUNC_NUM_OCCUR_TIME(stats_agari_richi_ok_dora_num, stats_agari_dora_num_impl, PlayerCondRichiOk)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_agari_first_richi_ok_dora_num, stats_agari_dora_num_impl, PlayerCondFirstRichiOk)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_agari_chasing_richi_ok_dora_num, stats_agari_dora_num_impl, PlayerCondChasingRichiOk)
DEFINE_FUNC_NUM_OCCUR_TIME(stats_agari_be_chased_richi_ok_dora_num, stats_agari_dora_num_impl, PlayerCondBeChasedRichiOk)


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


map<int32_t, map<int32_t, int32_t> >
stats_richi_gain_impl(const MjlogGameContainer &container, PlayerCond cond1, PlayerCond cond2)
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


map<int32_t, map<int32_t, int32_t> >
stats_richi_rate_impl(const MjlogGameContainer &container, PlayerCond cond1, PlayerCond cond2)
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

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondTrue)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_ron_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_tsumo_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_be_ron_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_be_tsumo_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_draw_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_ryuukyoku_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondTrue)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_ron_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_tsumo_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_be_ron_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_be_tsumo_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_draw_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_ryuukyoku_gain, stats_richi_gain_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ron_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_tsumo_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_ron_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_be_tsumo_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_draw_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_richi_n_ryuukyoku_rate, stats_richi_rate_impl, PlayerCondRichi, PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondTrue)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_ron_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_tsumo_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_be_ron_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_be_tsumo_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_draw_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_oya_richi_n_ryuukyoku_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondOya, PlayerCondRichi), PlayerCondRyuukyoku)

DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondTrue)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_ron_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_tsumo_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_be_ron_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondBeRon)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_be_tsumo_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondBeTsumo)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_draw_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondDraw)
DEFINE_FUNC_NUM_TO_MEAN_STD_SAMPLE(stats_not_oya_richi_n_ryuukyoku_rate, stats_richi_rate_impl, PlayerAnd(PlayerCondNotOya, PlayerCondRichi), PlayerCondRyuukyoku)

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



map<int32_t, map<uint64_t, int32_t> > stats_richi_tenpai_content(const MjlogGameContainer &container)
{
    TenPaiCheck::preprocess();

    map<int32_t, map<uint64_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            RoundTracer tracer(round_);
            while (tracer.do_action()) {
                if (tracer.m_it->is_richi2()) {
                    auto who = tracer.m_it->get_who();
                    auto ten_pai = TenPaiCheck::get_all_ten_pai(tracer.m_hai_private[who]);
                    uint8_t i = 0;
                    bitset<34> key;
                    for (i = 0; i < 16; ++ i) {
                        if (ten_pai[i] == NO_TEN_PAI) {
                            break;
                        }
                        key.set(ten_pai[i]);
                    }
                    if (i == 0) {
                        throw runtime_error("richi but no ten pai? wtf??");
                    }
                    ++ ans[tracer.m_sub_round[who]][key.to_ullong()];
                }
            }
        }
    }
    return ans;
}


}   // namespace Hasaki
