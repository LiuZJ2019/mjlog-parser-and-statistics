/**
 * @file        mjlog_tenpai.cpp
 * @brief       计算向听数/是否听牌/听哪些牌
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-06-04
 * @version     1.0.0
 * @details     目前只允许单线程运行，可以改成多线程版本，但是没有必要
 */
#include "mjlog_tenpai.h"
#include <mutex>


namespace Hasaki {


uint32_t NumHaiHash(const NumHai &hai)
{
    uint32_t ans = 0;
    for (auto i : hai) {
        ans = 5 * ans + i;
    }
    return ans;
}


uint32_t NumHaiHash(const uint8_t hai[9])
{
    uint32_t ans = 0;
    for (uint32_t i = 0; i < 9; ++ i) {
        ans = 5 * ans + hai[i];
    }
    return ans;
}


// 因为 0 <= a && a <= 4，加速(a % 3)运算
uint8_t num_hai_mod3(uint8_t a)
{
    return a < 3 ? a : a - 3;
}

constexpr std::array<uint32_t, 9> make_power5_array() {
    std::array<uint32_t, 9> arr{};
    for (int i = 0; i < 9; ++i)
        arr[i] = power5(8 - i);   // 降序，[8]..[0]
    return arr;
}
constexpr auto num_hai_hash_component = make_power5_array();


void TenPaiCheck::preprocess_agari_3n_a0()
{
    auto check_func = [](NumHai arr) {
        auto state_idx = NumHaiHash(arr);
        if (state[state_idx].get_agari_3n_a0()) {
            throw runtime_error("cannot repeat TenPaiCheck::preprocess_agari_3n_a0");
        }

        uint8_t first = arr[0];
        uint8_t second = arr[1];
        for (uint32_t i = 0; i < 7; ++ i) {
            uint8_t remove = num_hai_mod3(first);
            uint8_t third = arr[i + 2];
            if (second < remove || third < remove) {
                return;     // false
            }
            first = second - remove;
            second = third - remove;
        }
        if (num_hai_mod3(first) == 0 && num_hai_mod3(second) == 0) {
            state[state_idx].set_agari_3n_a0();
        }
    };

    for_each_array_with_sum(0, check_func);
    for_each_array_with_sum(3, check_func);
    for_each_array_with_sum(6, check_func);
    for_each_array_with_sum(9, check_func);
    for_each_array_with_sum(12, check_func);
}

void TenPaiCheck::preprocess_agari_3n_a2()
{
    // 假设preprocess_agari_3n_a0已经执行了
    auto check_func = [](NumHai arr) {
        auto state_idx = NumHaiHash(arr);
        if (state[state_idx].get_agari_3n_a2()) {
            throw runtime_error("cannot repeat TenPaiCheck::preprocess_agari_3n_a2");
        }

        for (uint32_t i = 0; i < 9; ++ i) {
            if (arr[i] < 2) {
                continue;
            }
            // 尝试减去雀头对应的哈希分量
            auto new_state_idx = state_idx - 2 * num_hai_hash_component[i];
            if (new_state_idx > state_idx) {
                throw runtime_error("Integer subtraction reversal in TenPaiCheck::preprocess_agari_3n_a2");
            }
            if (state[new_state_idx].get_agari_3n_a0()) {
                state[state_idx].set_agari_3n_a2();
                break;
            }
        }
    };

    for_each_array_with_sum(2, check_func);
    for_each_array_with_sum(5, check_func);
    for_each_array_with_sum(8, check_func);
    for_each_array_with_sum(11, check_func);
    for_each_array_with_sum(14, check_func);
}

void TenPaiCheck::preprocess_ten_3n_s1()
{
    auto check_func = [](NumHai arr) {
        auto state_idx = NumHaiHash(arr);
        if (state[state_idx].get_ten_3n_s1()) {
            throw runtime_error("cannot repeat TenPaiCheck::preprocess_ten_3n_s1");
        }

        for (uint32_t i = 0; i < 9; ++ i) {
            if (arr[i] == 4) {  // 和牌条件：当手牌为4张时，这张牌不能为待牌
                continue;
            }
            // 尝试加上待牌对应的哈希分量
            auto new_state_idx = state_idx + num_hai_hash_component[i];
            if (state[new_state_idx].get_agari_3n_a0()) {
                state[state_idx].set_ten_3n_s1();
                state[state_idx].set_ten_3n_s1(i);
            }
        }
    };

    for_each_array_with_sum(2, check_func);
    for_each_array_with_sum(5, check_func);
    for_each_array_with_sum(8, check_func);
    for_each_array_with_sum(11, check_func);
}

void TenPaiCheck::preprocess_ten_3n_a1()
{
    auto check_func = [](NumHai arr) {
        auto state_idx = NumHaiHash(arr);
        if (state[state_idx].get_ten_3n_a1()) {
            throw runtime_error("cannot repeat TenPaiCheck::preprocess_ten_3n_a1");
        }

        for (uint32_t i = 0; i < 9; ++ i) {
            if (arr[i] == 4) {  // 和牌条件：当手牌为4张时，这张牌不能为待牌
                continue;
            }
            // 尝试加上待牌对应的哈希分量
            auto new_state_idx = state_idx + num_hai_hash_component[i];
            if (state[new_state_idx].get_agari_3n_a2()) {
                state[state_idx].set_ten_3n_a1();
                state[state_idx].set_ten_3n_a1(i);
            }
        }
    };

    for_each_array_with_sum(1, check_func);
    for_each_array_with_sum(4, check_func);
    for_each_array_with_sum(7, check_func);
    for_each_array_with_sum(10, check_func);
    for_each_array_with_sum(13, check_func);
}

void TenPaiCheck::preprocess()
{
    static std::once_flag initFlag;
    std::call_once(initFlag, []() {
        state.resize(power5(9));
        // 注意顺序，不能换顺序
        preprocess_agari_3n_a0();
        preprocess_agari_3n_a2();
        preprocess_ten_3n_s1();
        preprocess_ten_3n_a1();
    });
}

uint8_t TenPaiCheck::get_ten_pai_yao13(const Hasaki::HaiMsg &hai)
{
    uint8_t cnt[5] = {};    // type-0/type-1/type-2/type-3/type-4
    uint8_t type0_idx = TEN_PAI_YAO13;
    static constexpr array<uint8_t, 13> yao13_idx = {0, 8, 9, 17, 18, 26, 27, 28, 29, 30, 31, 32, 33};
    for (auto idx: yao13_idx) {
        ++ cnt[hai.all_hai[idx]];
        if (hai.all_hai[idx] == 0) {
            type0_idx = idx;
        }
    }
    if (cnt[0] == 0) {  // 13张牌，全是幺九，则听13面
        return TEN_PAI_YAO13;
    } else if (cnt[0] == 1 && cnt[1] == 11 && cnt[2] == 1) {
        return type0_idx;
    } else {
        return NO_TEN_PAI;
    }
}

uint8_t TenPaiCheck::get_ten_pai_pair7(const Hasaki::HaiMsg &hai)
{
    uint8_t type1_cnt = 0;
    uint8_t type1_idx = NO_TEN_PAI;
    for (uint8_t i = 0; i < 34; ++ i) {
        switch (hai.all_hai[i]) {
            case 3: case 4: return NO_TEN_PAI;
            case 1: {
                if (type1_idx != NO_TEN_PAI) {
                    return NO_TEN_PAI;
                }
                type1_idx = i;
                break;
            }
            default: break;
        }
    }
    if (type1_idx == NO_TEN_PAI) {
        throw runtime_error("unexpect case in TenPaiCheck::get_ten_pai_pair7");
    }
    return type1_idx;
}

void AppendNumHaiToAllTenHai(AllTenHai &all_hai, NumHai num_hai)
{
    uint32_t all_hai_size = 0;
    for (; all_hai_size < all_hai.size(); ++ all_hai_size) {
        if (all_hai[all_hai_size] == NO_TEN_PAI) {
            break;
        }
    }
    for (uint32_t idx = 0; idx < num_hai.size(); ++ idx) {
        if (num_hai[idx] == NO_TEN_PAI) {
            break;
        }
        if (all_hai_size + idx >= all_hai.size()) {
            throw runtime_error("AppendNumHaiToAllTenHai error");
        }
        all_hai[all_hai_size + idx] = num_hai[idx];
    }
}

AllTenHai TenPaiCheck::get_ten_pai_normal(const Hasaki::HaiMsg &hai)
{
    AllTenHai ans;
    ans.fill(NO_TEN_PAI);
    uint8_t type1_ji_hai = 0;
    uint8_t type2_ji_hai = 0;
    uint8_t type1_ji_hai_idx = 0;
    uint8_t type2_ji_hai_idx[2] = {NO_TEN_PAI, NO_TEN_PAI};
    for (uint8_t i = 27; i < 34; ++ i) {
        switch (hai.all_hai[i]) {
            case 1: ++ type1_ji_hai; type1_ji_hai_idx = i; break;
                // 位运算技巧提升性能
            case 2: ++ type2_ji_hai; type2_ji_hai_idx[type2_ji_hai_idx[0] != NO_TEN_PAI] = i; break;
                // case 3为刻子
            case 4: return ans;
            default: break;
        }
    }

    const uint8_t *color[3] = {hai.get_man_zu(), hai.get_pin_zu(), hai.get_sou_zu()};
    uint8_t mod_3n[3][3] = {{NO_TEN_PAI, NO_TEN_PAI, NO_TEN_PAI},
                            {NO_TEN_PAI, NO_TEN_PAI, NO_TEN_PAI},
                            {NO_TEN_PAI, NO_TEN_PAI, NO_TEN_PAI}};
    for (uint8_t i = 0; i < 3; ++ i) {
        uint8_t mod3_ret = hai.all_cnt[i] % 3;
        for (uint8_t j = 0; j < 3; ++ j) {
            if (mod_3n[mod3_ret][j] == NO_TEN_PAI) {
                mod_3n[mod3_ret][j] = i;
                break;
            }
        }
    }
    if (type1_ji_hai == 0 && type2_ji_hai == 0) {           // 3N+1听牌判定
        if (mod_3n[1][0] == NO_TEN_PAI) {   // 如果不存在模3余1的花色，取出两个模3余2的花色
            auto color0 = mod_3n[2][0];     // 模3余2
            auto color1 = mod_3n[2][1];     // 模3余2
            auto color2 = mod_3n[0][0];     // 模3余0
            if (color0 == NO_TEN_PAI || color1 == NO_TEN_PAI || color2 == NO_TEN_PAI) {
                return ans;
            }
            if (color0 > color1) {          // 使得ans从小到大排序
                std::swap(color0, color1);
            }
            if (check_agari_3n_a0(color[color2])) {
                if (check_agari_3n_a2(color[color1]) && check_ten_3n_s1(color[color0])) {
                    AppendNumHaiToAllTenHai(ans, get_all_ten_3n_s1(color[color0], 9 * color0));
                }
                if (check_agari_3n_a2(color[color0]) && check_ten_3n_s1(color[color1])) {
                    AppendNumHaiToAllTenHai(ans, get_all_ten_3n_s1(color[color1], 9 * color1));
                }
            }
        } else if (mod_3n[1][1] == NO_TEN_PAI) {    // 如果存在一个模3余1的花色，取出两个模3余0的花色
            auto color0 = mod_3n[0][0];     // 模3余0
            auto color1 = mod_3n[0][1];     // 模3余0
            auto color2 = mod_3n[1][0];     // 模3余1
            if (color0 == NO_TEN_PAI || color1 == NO_TEN_PAI || color2 == NO_TEN_PAI) {
                return ans;
            }
            if (check_agari_3n_a0(color[color0]) && check_agari_3n_a0(color[color1]) && check_ten_3n_a1(color[color2])) {
                AppendNumHaiToAllTenHai(ans, get_all_ten_3n_a1(color[color2], 9 * color2));
            }
        }   // else: 一定未听牌，直接返回NO_TEN_PAI
    } else if (type1_ji_hai == 1 && type2_ji_hai == 0) {    // 3N和了判定(雀头一定是type1_ji_hai_idx)
        auto color0 = mod_3n[0][0];     // 模3余0
        auto color1 = mod_3n[0][1];     // 模3余0
        auto color2 = mod_3n[0][2];     // 模3余0
        if (color0 == NO_TEN_PAI || color1 == NO_TEN_PAI || color2 == NO_TEN_PAI) {
            return ans;
        }
        if (check_agari_3n_a0(color[0]) && check_agari_3n_a0(color[1]) && check_agari_3n_a0(color[2])) {
            ans[0] = type1_ji_hai_idx;
        }
    } else if (type1_ji_hai == 0 && type2_ji_hai == 1) {    // 3N-1听牌判定(字牌为雀头) 和 3N+2和了判定(字牌为刻子)
        auto color0 = mod_3n[0][0];     // 模3余0
        auto color1 = mod_3n[0][1];     // 模3余0
        auto color2 = mod_3n[2][0];     // 模3余2
        if (color0 == NO_TEN_PAI || color1 == NO_TEN_PAI || color2 == NO_TEN_PAI ||
            !check_agari_3n_a0(color[color0]) || !check_agari_3n_a0(color[color1])) {
            return ans;
        }
        // 3N-1听牌判定(字牌为雀头)
        if (check_ten_3n_s1(color[color2])) {
            AppendNumHaiToAllTenHai(ans, get_all_ten_3n_s1(color[color2], 9 * color2));
        }
        // 3N+2和了判定(字牌为刻子)
        if (check_agari_3n_a2(color[color2])) {
            for (auto &h: ans) {
                if (h == NO_TEN_PAI) {
                    h = type2_ji_hai_idx[0];
                    break;
                }
            }
        }
    } else if (type1_ji_hai == 0 && type2_ji_hai == 2) {    // 3N和了判定(type2_ji_hai_idx的双碰)
        auto color0 = mod_3n[0][0];     // 模3余0
        auto color1 = mod_3n[0][1];     // 模3余0
        auto color2 = mod_3n[0][2];     // 模3余0
        if (color0 == NO_TEN_PAI || color1 == NO_TEN_PAI || color2 == NO_TEN_PAI) {
            return ans;
        }
        if (check_agari_3n_a0(color[0]) && check_agari_3n_a0(color[1]) && check_agari_3n_a0(color[2])) {
            ans[0] = type2_ji_hai_idx[0];
            ans[1] = type2_ji_hai_idx[1];
        }
    }   // else: 一定未听牌，直接返回NO_TEN_PAI
    return ans;
}

AllTenHai TenPaiCheck::get_all_ten_pai(const HaiMsg &hai)
{
    uint8_t hai_cnt = hai.all_cnt[0] + hai.all_cnt[1] + hai.all_cnt[2] + hai.all_cnt[3];
    if (hai_cnt + 3 * hai.meld_cnt != 13) {
        throw runtime_error("TenPaiCheck::check_ten_pai: hai count is not 13?");
    }

    uint8_t ret_pair7 = NO_TEN_PAI;
    if (hai.meld_cnt == 0) {
        auto ret_yao13 = get_ten_pai_yao13(hai);
        if (ret_yao13 != NO_TEN_PAI) {
            AllTenHai ans;
            ans.fill(NO_TEN_PAI);
            if (ret_yao13 == TEN_PAI_YAO13) {
                static constexpr array<uint8_t, 13> yao13_idx = {0, 8, 9, 17, 18, 26, 27, 28, 29, 30, 31, 32, 33};
                std::copy(std::begin(yao13_idx), std::end(yao13_idx), ans.begin());
            } else {
                ans[0] = ret_yao13;
            }
            return ans;
        }
        ret_pair7 = get_ten_pai_pair7(hai);
    }

    auto ret_normal = get_ten_pai_normal(hai);
    // 只要一般型听牌了，就以一般型的听牌结果为准（一般型听牌+七对子听牌情形，七对子一定能被解读为两杯口，从而被覆盖）
    if (ret_normal[0] != NO_TEN_PAI) {
        return ret_normal;
    }
    // 如果一般型没听牌，七对子听牌，返回七对子的结果（七对子没听牌的话，ret_pair7就是NO_TEN_PAI，不影响）
    ret_normal[0] = ret_pair7;
    return ret_normal;
}

bool TenPaiCheck::check_agari_3n_a0(const uint8_t hai[9])
{
    auto state_idx = NumHaiHash(hai);
    return state[state_idx].get_agari_3n_a0();
}

bool TenPaiCheck::check_agari_3n_a2(const uint8_t hai[9])
{
    auto state_idx = NumHaiHash(hai);
    return state[state_idx].get_agari_3n_a2();
}

bool TenPaiCheck::check_ten_3n_s1(const uint8_t hai[9])
{
    auto state_idx = NumHaiHash(hai);
    return state[state_idx].get_ten_3n_s1();
}

bool TenPaiCheck::check_ten_3n_a1(const uint8_t hai[9])
{
    auto state_idx = NumHaiHash(hai);
    return state[state_idx].get_ten_3n_a1();
}

array<uint8_t, 9> TenPaiCheck::get_all_ten_3n_s1(const uint8_t hai[9], uint8_t base)
{
    array<uint8_t, 9> ans;
    ans.fill(NO_TEN_PAI);
    auto state_idx = NumHaiHash(hai);
    uint8_t idx = 0;
    for (uint8_t hai_idx = 0; hai_idx < 9; ++ hai_idx) {
        if (state[state_idx].get_ten_3n_s1(hai_idx)) {
            ans[idx++] = base + hai_idx;
        }
    }
    return ans;
}

array<uint8_t, 9> TenPaiCheck::get_all_ten_3n_a1(const uint8_t hai[9], uint8_t base)
{
    array<uint8_t, 9> ans;
    ans.fill(NO_TEN_PAI);
    auto state_idx = NumHaiHash(hai);
    uint8_t idx = 0;
    for (uint8_t hai_idx = 0; hai_idx < 9; ++ hai_idx) {
        if (state[state_idx].get_ten_3n_a1(hai_idx)) {
            ans[idx++] = base + hai_idx;
        }
    }
    return ans;
}

uint32_t get_ten_pai_size(array<uint8_t, 9> hai)
{
    uint32_t ans = 0;
    for (auto i: hai) {
        if (i == NO_TEN_PAI) {
            break;
        }
        ++ ans;
    }
    return ans;
}

}   // namespace Hasaki
