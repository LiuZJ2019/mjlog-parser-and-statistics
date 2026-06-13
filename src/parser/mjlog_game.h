/**
 * @file        mjlog_game.h
 * @brief       RoundData是一小局，MjlogGame是一整场半庄
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     RoundData分为InitData+vector<Action>+array<unique_ptr<EndData>, 4>
 *              InitData是当前Round的初始信息，包括局数/本场数/供托数/宝牌指示牌/初始点数/谁是庄家/初始手牌
 *              vector<Action>是对局行为，见parser/mjlog_action.h
 *              array<unique_ptr<EndData>, 4>的EndData是终局条件，分为EndAgari和EndRyuuKyoKu
 *              其中存在一炮双响/多家流局满贯的可能，所以EndData最多可能有4个（虽然我都没见过3家流局满贯的case）
 *              天凤牌谱中，一炮三响不会有多个EndAgari，而是一个类型为ron3的EndRyuuKyoKu
 *              EndData包含公共信息，即type/本场/供托数/分数及分数变化
 */
#ifndef HASAKI_MJLOG_PARSER_PARSER_MJLOG_GAME_H_
#define HASAKI_MJLOG_PARSER_PARSER_MJLOG_GAME_H_

#include <array>
#include <bitset>
#include "parser/mjlog_action.h"
#include "parser/mjlog_xml_parser.h"
#include "parser/mjlog_tenpai.h"


namespace Hasaki {

using std::bitset;

using DoraMap = array<uint8_t, 34>;
using DoraIndicator = array<uint8_t, 5>;
using HaiCompress = array<uint8_t, 13>;     // 压缩编码的手牌，每个副露压缩成 MELD+2B
using HaiFlatten = array<uint8_t, 24>;      // 完全展开的手牌，副露也展开

struct InitData {
    uint8_t m_round;
    uint8_t m_honba;
    uint8_t m_kyotaku;
    uint8_t m_dora;
    array<int16_t, 4> m_ten;
    uint8_t m_oya;
    array<HaiCompress, 4> m_hai;

    static InitData from_bytes(Buffer &q);
    void to_bytes(Buffer &q) const;
    static InitData from_xml(const string &name, const XmlMap &xmlMap);
    void to_xml(string &str) const;

    string str(StringType type) const;
    bool operator==(const InitData &data) const noexcept;
    bool operator!=(const InitData &data) const noexcept;
};

struct EndAgari;
struct EndRyuuKyoKu;

// 结束标志符，分为AGARI(可能多个，对应一炮多响)和RYUUKYOKU(普通流局/流局满贯/九种九牌/四风连打/四家立直/三家和了/四杠散了)，流局满贯也可能多个
// 不记录owari，因为点数可以算出来，而我不关心段位分变化
struct EndData {
    uint8_t m_type;
    uint8_t m_honba;
    uint8_t m_kyotaku;
    array<int16_t, 8> m_sc;

    static unique_ptr<EndData> from_bytes(Buffer &q);
    void to_bytes(Buffer &q) const;
    static unique_ptr<EndData> from_xml(const string &name, const XmlMap &xmlMap);
    void to_xml(string &str) const;

    bool is_agari() const noexcept;
    bool is_ryuukyoku() const noexcept;
    const EndAgari *to_agari() const;
    const EndRyuuKyoKu *to_ryuukyoku() const;
    MetaType get_meta_type() const;

    string str(StringType type) const;
    bool operator==(const EndData &data) const noexcept;
    bool operator!=(const EndData &data) const noexcept;

protected:
    void init_from_bytes(Buffer &q);
    void write_to_bytes(Buffer &q) const;
    void init_from_xml(const string &name, const XmlMap &xmlMap);
};

/// @see https://docs.rs/mjlog/latest/mjlog/model/enum.Yaku.html
enum YakuType: uint8_t {
    MenzenTsumo = 0,
    Riichi = 1,
    Ippatsu = 2,
    Chankan = 3,
    Rinshankaihou = 4,
    HaiteiTsumo = 5,
    HouteiRon = 6,
    Pinfu = 7,
    Tanyao = 8,
    Iipeikou = 9,
    PlayerWindTon = 10,
    PlayerWindNan = 11,
    PlayerWindSha = 12,
    PlayerWindPei = 13,
    FieldWindTon = 14,
    FieldWindNan = 15,
    FieldWindSha = 16,
    FieldWindPei = 17,
    YakuhaiHaku = 18,
    YakuhaiHatsu = 19,
    YakuhaiChun = 20,
    DoubleRiichi = 21,
    Chiitoitsu = 22,
    Chanta = 23,
    Ikkitsuukan = 24,
    SansyokuDoujun = 25,
    SanshokuDoukou = 26,
    Sankantsu = 27,
    Toitoi = 28,
    Sanannkou = 29,
    Shousangen = 30,
    Honroutou = 31,
    Ryanpeikou = 32,
    Junchan = 33,
    Honiisou = 34,
    Chiniisou = 35,
    Renhou = 36,
    Tenhou = 37,
    Chiihou = 38,
    Daisangen = 39,
    Suuankou = 40,
    SuuankouTanki = 41,
    Tsuuiisou = 42,
    Ryuuiisou = 43,
    Chinroutou = 44,
    Tyuurenpoutou = 45,
    Tyuurenpoutou9 = 46,
    Kokushimusou = 47,
    Kokushimusou13 = 48,
    Daisuushii = 49,
    Syousuushii = 50,
    Suukantsu = 51,
    Dora = 52,
    UraDora = 53,
    AkaDora = 54,
};

// 从宝牌指示牌计算哪些牌是宝牌
// 0-35: 1-9m
// 36-71: 1-9p
// 72-107: 1-9s
// 108-123: 1-4z (东南西北)
// 124-135: 5-7z (白发中)
DoraMap get_dora_map(const DoraIndicator &indicator);

// 16/52/88: 0m/0p/0s
bool is_aka_dora(uint8_t hai);

// 将2Byte的副露编码解析为原始牌
array<uint8_t, 4> flatten_meld(uint16_t meld);

// 将压缩副露编码展开为牌序列 (hai_compress + machi)，为计算宝牌做准备
HaiFlatten flatten_hai(const HaiCompress &hai_compress, uint8_t machi=HaiType::NO_HAI);

// 计算手牌有多少宝牌
uint8_t get_dora_count_of_hai(const DoraIndicator &indicator, HaiFlatten hai_flatten);


struct EndAgari : public EndData {
    HaiCompress m_hai;                  // 此时是hai+m-machi, m编码为3位uint8_t，以MELD_TYPE开头
    uint8_t m_machi;                    // 此时是hai+m-machi
    uint8_t m_ten0;                     // 符
    uint8_t m_ten2;                     // 类型
    uint16_t m_ten1;                    // 打点/100（顺序换过来，节省字节）
    bitset<55> m_yaku;                  // 役种，一共55种，用bitset标识节省空间
    DoraIndicator m_doraHai;            // 宝牌指示牌
    DoraIndicator m_doraHaiUra;         // 里宝牌指示牌

    static unique_ptr<EndData> from_bytes(Buffer &q);
    void to_bytes(Buffer &q) const;
    static unique_ptr<EndData> from_xml(const string &name, const XmlMap &xmlMap);
    void to_xml(string &str) const;

    string str(StringType type) const;
    uint8_t get_who() const noexcept;
    uint8_t get_from_who() const noexcept;
    bool is_tsumo() const noexcept;
    bool is_meld() const noexcept;
    bool is_yakuman() const noexcept;
    bool is_richi() const noexcept;
    int32_t get_score() const noexcept;
    HaiFlatten get_hai_flatten() const;
    uint8_t get_dora_cnt(const HaiFlatten &hai_flatten) const;
    uint8_t get_dora_cnt() const;
    uint8_t get_ura_dora_cnt(const HaiFlatten &hai_flatten) const;
    uint8_t get_ura_dora_cnt() const;
    uint8_t get_aka_dora_cnt(const HaiFlatten &hai_flatten) const;
    uint8_t get_aka_dora_cnt() const;
    bool operator==(const EndAgari &data) const noexcept;
    bool operator!=(const EndAgari &data) const noexcept;
};

struct EndRyuuKyoKu : public EndData {
    // 可能包含NO_HAI
    array<HaiCompress, 4> m_hai;

    static unique_ptr<EndData> from_bytes(Buffer &q);
    void to_bytes(Buffer &q) const;
    static unique_ptr<EndData> from_xml(const string &name, const XmlMap &xmlMap);
    void to_xml(string &str) const;

    string str(StringType type) const;
    bool operator==(const EndRyuuKyoKu &data) const noexcept;
    bool operator!=(const EndRyuuKyoKu &data) const noexcept;
};

enum StatusAgariType: uint8_t {
    TSUMO = 0,
    RON_NO_REACH = 1,
    RON_REACH1 = 2,
    RON_REACH2 = 3
};

struct PlayerStatus {
    bool oya: 1;                        // 是否是庄家
    bool richi: 1;                      // 是否立直，无论是否成功
    bool richi_ok: 1;                   // 是否立直成功
    bool first_richi: 1;                // 是否先制立直，无论是否成功（前提: richi == true）
    bool chasing_richi: 1;              // 是否为追立直，无论是否成功（前提: richi == true）
    bool be_chased_richi: 1;            // 是否被追立直，无论是否成功（前提: richi == true）
    bool be_chased_richi_ok: 1;         // 追立直时立直宣言牌未铳（前提: be_chased_richi == true）
    bool agari: 1;                      // 自家和牌(一炮多响可能会agari+draw)
    bool draw: 1;                       // 他家和非自家牌(方便起见，不对役满包牌做特殊处理)
    bool be_ron: 1;                     // 他家和自家牌(方便起见，不对役满包牌做特殊处理)
    bool be_tsumo: 1;                   // 他家自摸(方便起见，不对役满包牌做特殊处理)
    uint8_t agari_type: 2;              // 0: 自摸, 1: 荣和非立直家的牌, 2: 荣和立直宣言牌, 3: 荣和其他立直家的牌(非宣言牌)
    uint8_t ryuukyoku_type: 3;          // 0: 非流局, 1-7: 荒牌流局/流局满贯/九种九牌/四风连打/四家立直/三家和了/四杠散了
    uint8_t richi_num;                  // 立直巡目（前提: richi == true，未立直为UINT8_MAX）
    uint8_t end_num;                    // 终局巡目

    PlayerStatus() noexcept;
};

struct RoundData {
    InitData m_init;
    vector<Action> m_actions;
    array<unique_ptr<EndData>, 4> m_ends;
    array<PlayerStatus, 4> m_players;

    static RoundData from_bytes(Buffer &q);
    void to_bytes(Buffer &q) const;
    static RoundData from_xml(XmlParser &parser);
    void to_xml(string &str) const;

    void preprocess_player_status();

    string str(StringType type) const;
    int32_t get_last_end_idx() const;
    bool operator==(const RoundData &data) const noexcept;
    bool operator!=(const RoundData &data) const noexcept;
};

struct MjlogGame {
    uint16_t m_type;
    vector<RoundData> m_rounds;

    static MjlogGame from_bytes(Buffer &q);
    void to_bytes(Buffer &q) const;
    static MjlogGame from_xml(const string &str);
    void to_xml(string &str) const;

    string str(StringType type) const;
    bool operator==(const MjlogGame &data) const noexcept;
    bool operator!=(const MjlogGame &data) const noexcept;
};


// 按照牌局先后顺序模拟每次行为后的状态，用于追踪复盘
struct RoundTracer {
    const RoundData &m_data;
    vector<Action>::const_iterator m_it;    // Action的迭代器
    array<uint8_t, 4> m_sub_round;          // 当前巡目数，每摸牌一次+1
    array<int16_t, 4> m_ten;                // 玩家的当前点棒数（除以100），立直成功后会变动
    HaiMsg m_hai_public;                    // 所有人可见的牌河，包括副露+切牌+宝牌指示牌
    array<HaiMsg, 4> m_hai_private;         // 玩家的手牌
    array<uint8_t, 5> m_dora;               // 宝牌
    array<bool, 4> m_real_meld;             // 记录玩家是否为非暗杠的副露

    RoundTracer(const RoundData &data);
    // 模拟m_it对应的Action，如果目前执行完了所有Action，返回false，否则返回true
    bool do_action();
};

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_PARSER_MJLOG_GAME_H_
