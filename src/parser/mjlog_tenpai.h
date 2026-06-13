/**
 * @file        mjlog_tenpai.h
 * @brief       计算向听数/是否听牌/听哪些牌
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-06-04
 * @version     1.0.0
 */
#ifndef HASAKI_MJLOG_PARSER_PARSER_MJLOG_TENPAI_H_
#define HASAKI_MJLOG_PARSER_PARSER_MJLOG_TENPAI_H_

#include <array>
#include <string>
#include <vector>
#include <cstdint>
#include <stdexcept>
#include <functional>


namespace Hasaki {

using std::array;
using std::function;
using std::to_string;
using std::runtime_error;

using NumHai = array<uint8_t, 9>;
// 16个一定够用，一个花色听9张只有可能是九莲宝灯，所以多花色场景每个花色最多听8张，两个花色最多16张（实际上肯定没有16张）
using AllTenHai = array<uint8_t, 16>;

const uint8_t NO_TEN_PAI = 0xff;
const uint8_t TEN_PAI_YAO13 = 0xfe;

// 5^N, 编译期计算
constexpr long long power5(unsigned int N)
{
    return (N == 0) ? 1 : 5 * power5(N - 1);
}

// a: add; s: sub; 3n_a2 == 3n add 2 == 3*n + 2; 3n_s1 == 3n sub 1 == 3*n - 1
struct TenPaiState {
    uint16_t m_ten_3n_s1;     // bit0-8: 3n-1听牌(且听1-9); bit9: 3n-1听牌; bit10: 3n和了
    uint16_t m_ten_3n_a1;     // bit0-8: 3n+1听牌(且听1-9); bit9: 3n+1听牌; bit10: 3n+2和了
    // 3n和了
    bool get_agari_3n_a0() const noexcept {
        return m_ten_3n_s1 & (1 << 10);
    }
    void set_agari_3n_a0() noexcept {
        m_ten_3n_s1 |= (1 << 10);
    }
    // 3n+2和了
    bool get_agari_3n_a2() const noexcept {
        return m_ten_3n_a1 & (1 << 10);
    }
    void set_agari_3n_a2() noexcept {
        m_ten_3n_a1 |= (1 << 10);
    }
    // 3n-1听牌
    bool get_ten_3n_s1() const noexcept {
        return m_ten_3n_s1 & (1 << 9);
    }
    void set_ten_3n_s1() noexcept {
        m_ten_3n_s1 |= (1 << 9);
    }
    // 3n+1听牌
    bool get_ten_3n_a1() const noexcept {
        return m_ten_3n_a1 & (1 << 9);
    }
    void set_ten_3n_a1() noexcept {
        m_ten_3n_a1 |= (1 << 9);
    }
    // 3n-1听牌
    bool get_ten_3n_s1(uint8_t idx) const noexcept {
        return m_ten_3n_s1 & (1 << idx);
    }
    void set_ten_3n_s1(uint8_t idx) noexcept {
        m_ten_3n_s1 |= (1 << idx);
    }
    // 3n+1听牌
    bool get_ten_3n_a1(uint8_t idx) const noexcept {
        return m_ten_3n_a1 & (1 << idx);
    }
    void set_ten_3n_a1(uint8_t idx) noexcept {
        m_ten_3n_a1 |= (1 << idx);
    }
};


struct HaiMsg {
    uint8_t all_hai[34];
    uint8_t all_cnt[4];     // man_zu/pin_zu/sou_zu/ji_hai
    uint8_t meld_cnt;

    HaiMsg()
        : all_hai{}
        , all_cnt{}
        , meld_cnt{}
    {
    };
    HaiMsg(const HaiMsg &)=default;
    void add_hai(uint8_t hai) {
        uint8_t hai_real = hai / 4;
        uint8_t color_real = hai_real / 9;
        if (hai_real >= 34) {
            throw runtime_error("HaiMsg::add_hai: unknown hai: " + to_string(hai_real));
        }
        ++ all_hai[hai_real];
        ++ all_cnt[color_real];
        if (all_hai[hai_real] > 4) {
            throw runtime_error("HaiMsg::add_hai: Invalid increase");
        }
    }
    void del_hai(uint8_t hai) {
        uint8_t hai_real = hai / 4;
        uint8_t color_real = hai_real / 9;
        if (hai_real >= 34) {
            throw runtime_error("HaiMsg::del_hai: unknown hai: " + to_string(hai_real));
        }
        -- all_hai[hai_real];
        -- all_cnt[color_real];
        if (all_hai[hai_real] == 0xff || all_cnt[color_real] == 0xff) {
            throw runtime_error("HaiMsg::del_hai: Invalid decrease");
        }
    }
    void meld_hai(uint8_t hai[4], uint8_t no_ten_pai_tag) {
        for (uint8_t i = 0; i < 4; ++ i) {
            if (hai[i] == no_ten_pai_tag) {
                break;
            }
            del_hai(hai[i]);
        }
        ++ meld_cnt;
    }
    const uint8_t *get_man_zu() const noexcept {
        return (const uint8_t *)all_hai;
    }
    const uint8_t *get_pin_zu() const noexcept {
        return (const uint8_t *)all_hai + 9;
    }
    const uint8_t *get_sou_zu() const noexcept {
        return (const uint8_t *)all_hai + 18;
    }
};


struct TenPaiCheck {
    static inline std::vector<TenPaiState> state;

    static void preprocess();
    // Attention: must execute TenPaiCheck::preprocess before!
    static AllTenHai get_all_ten_pai(const HaiMsg &hai);

    static bool check_agari_3n_a0(const uint8_t hai[9]);
    static bool check_agari_3n_a2(const uint8_t hai[9]);
    static bool check_ten_3n_s1(const uint8_t hai[9]);
    static bool check_ten_3n_a1(const uint8_t hai[9]);
    static array<uint8_t, 9> get_all_ten_3n_s1(const uint8_t hai[9], uint8_t base);
    static array<uint8_t, 9> get_all_ten_3n_a1(const uint8_t hai[9], uint8_t base);
private:
    static void preprocess_agari_3n_a0();
    static void preprocess_agari_3n_a2();
    static void preprocess_ten_3n_s1();
    static void preprocess_ten_3n_a1();

    // 判断是否听国士无双，不听返回NO_TEN_PAI，听牌返回听的那张牌，听13面返回TEN_PAI_YAO13
    static uint8_t get_ten_pai_yao13(const HaiMsg &hai);
    // 判断是否听七对，不听返回NO_TEN_PAI，听牌返回听的那张牌
    static uint8_t get_ten_pai_pair7(const HaiMsg &hai);
    // 判断是否听一般型
    static AllTenHai get_ten_pai_normal(const HaiMsg &hai);
};


uint32_t get_ten_pai_size(array<uint8_t, 9> hai);


// 对于所有满足0 <= a[i] <= 4，sum(a[i]) == k的a，执行f(a)
template<typename Callback>
void for_each_array_with_sum(int k, Callback&& f) {
    NumHai a{};

    function<void(int, int)> dfs = [&](int pos, int rem) {
        if (pos == 9) {
            if (rem == 0) {
                f(a);
            }
            return;
        }

        int min_v = std::max(0, rem - (8 - pos) * 4);
        int max_v = std::min(4, rem);

        for (int v = min_v; v <= max_v; ++v) {
            a[pos] = static_cast<uint8_t>(v);
            dfs(pos + 1, rem - v);
        }
    };

    dfs(0, k);
}

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_PARSER_MJLOG_TENPAI_H_
