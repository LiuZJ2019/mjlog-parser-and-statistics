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

map<int32_t, int32_t> stats_richi_ok_num(const MjlogGameContainer &container)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.richi_ok) {
                    ans[player.richi_num] += 1;
                }
            }
        }
    }
    return ans;
}

map<int32_t, int32_t> stats_first_richi_ok_num(const MjlogGameContainer &container)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.first_richi && player.richi_ok) {
                    ans[player.richi_num] += 1;
                }
            }
        }
    }
    return ans;
}

map<int32_t, int32_t> stats_chasing_richi_ok_num(const MjlogGameContainer &container)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.chasing_richi && player.richi_ok) {
                    ans[player.richi_num] += 1;
                }
            }
        }
    }
    return ans;
}

map<int32_t, int32_t> stats_be_chased_richi_ok_num(const MjlogGameContainer &container)
{
    map<int32_t, int32_t> ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.be_chased_richi_ok) {
                    ans[player.richi_num] += 1;
                }
            }
        }
    }
    return ans;
}

map<int32_t, map<int32_t, int32_t> > stats_richi_n_gain(const MjlogGameContainer &container)
{
    map<int32_t, map<int32_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (uint32_t i = 0; i < 4; ++i) {
                const auto &player = round_.m_players[i];
                if (player.richi) {
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

map<int32_t, map<int32_t, int32_t> > stats_richi_n_ron_rate(const MjlogGameContainer &container)
{
    map<int32_t, map<int32_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.richi) {
                    if (player.agari && player.agari_type != StatusAgariType::TSUMO) {
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

map<int32_t, map<int32_t, int32_t> > stats_richi_n_tsumo_rate(const MjlogGameContainer &container)
{
    map<int32_t, map<int32_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.richi) {
                    if (player.agari && player.agari_type == StatusAgariType::TSUMO) {
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

map<int32_t, map<int32_t, int32_t> > stats_richi_n_be_ron_rate(const MjlogGameContainer &container)
{
    map<int32_t, map<int32_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.richi) {
                    if (player.be_ron) {
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

map<int32_t, map<int32_t, int32_t> > stats_richi_n_be_tsumo_rate(const MjlogGameContainer &container)
{
    map<int32_t, map<int32_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.richi) {
                    if (player.be_tsumo) {
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

map<int32_t, map<int32_t, int32_t> > stats_richi_n_draw_rate(const MjlogGameContainer &container)
{
    map<int32_t, map<int32_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.richi) {
                    if (player.draw && !player.agari) {
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

map<int32_t, map<int32_t, int32_t> > stats_richi_n_ryuukyoku_rate(const MjlogGameContainer &container)
{
    map<int32_t, map<int32_t, int32_t> > ans;
    for (const auto &game: container.m_games) {
        for (const auto &round_: game.m_rounds) {
            for (const auto &player: round_.m_players) {
                if (player.richi) {
                    if (player.ryuukyoku_type != 0) {
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

}   // namespace Hasaki
