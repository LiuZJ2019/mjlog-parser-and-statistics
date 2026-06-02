/**
 * @file        mjlog_game.cpp
 * @brief       RoundData是一小局，MjlogGame是一整场半庄
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     EndAgari会记录包括副露编码的最终牌型，其中副露编码以MELD_TYPE开头，后面接着小端序的m
 *              EndRyuuKyoKu会记录包括各家的牌，但不包含副露，所以可能存在NO_HAI
 *              MjlogGame包含若干个RoundData和type，实际上xml包含更多元信息，但我只处理了type
 *              目前我是通过.hskmjlog的首字节确定的版本，当前版本为1。如果未来需要新版本处理更多元信息，需要更改版本号
 */
#include "mjlog_game.h"
#include <algorithm>


namespace Hasaki {

InitData InitData::from_bytes(Buffer &q)
{
    uint8_t round_ = q.front();
    q.pop_front();
    uint8_t honba = q.front();
    q.pop_front();
    uint8_t kyotaku = q.front();
    q.pop_front();
    uint8_t dora = q.front();
    q.pop_front();
    uint8_t oya = q.front();
    q.pop_front();
    array<int16_t, 4> ten{};
    for (uint8_t i = 0; i < 4; ++i) {
        ten[i] = readFromBuf<int16_t>(q);
    }
    array<HaiCompress, 4> hai{};
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 13; ++j) {
            hai[i][j] = q.front();
            q.pop_front();
        }
    }
    return {
            .m_round=round_,
            .m_honba=honba,
            .m_kyotaku=kyotaku,
            .m_dora=dora,
            .m_ten=ten,
            .m_oya=oya,
            .m_hai=hai
    };
}

void InitData::to_bytes(Buffer &q) const
{
    q.push_back(m_round);
    q.push_back(m_honba);
    q.push_back(m_kyotaku);
    q.push_back(m_dora);
    q.push_back(m_oya);
    for (uint8_t i = 0; i < 4; ++i) {
        writeToBuf<int16_t>(q, m_ten[i]);
    }
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 13; ++j) {
            q.push_back(static_cast<uint8_t>(m_hai[i][j]));
        }
    }
}

InitData InitData::from_xml(const string &name, const XmlMap &xmlMap)
{
    if (name != "INIT") {
        throw runtime_error("Invalid type in InitData::from_xml: " + name);
    }
    auto seed = splitToVecInt(xmlMap, "seed");
    auto ten = splitToVecInt(xmlMap, "ten");
    auto oya = get_u2_from_attrs(xmlMap, "oya");
    vector<vector<int32_t> > hai_{
            splitToVecInt(xmlMap, "hai0"),
            splitToVecInt(xmlMap, "hai1"),
            splitToVecInt(xmlMap, "hai2"),
            splitToVecInt(xmlMap, "hai3"),
    };
    array<HaiCompress, 4> hai{};
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 13; ++j) {
            hai[i][j] = static_cast<uint8_t>(hai_[i][j]);
        }
    }
    return {
            .m_round=static_cast<uint8_t>(seed[0]),
            .m_honba=static_cast<uint8_t>(seed[1]),
            .m_kyotaku=static_cast<uint8_t>(seed[2]),
            .m_dora=static_cast<uint8_t>(seed[5]),
            .m_ten={static_cast<int16_t>(ten[0]), static_cast<int16_t>(ten[1]),
                    static_cast<int16_t>(ten[2]), static_cast<int16_t>(ten[3])},
            .m_oya=oya,
            .m_hai=hai
    };
}

void InitData::to_xml(string &str) const
{
    str += "<INIT";
    vector<uint8_t> seed{m_round, m_honba, m_kyotaku, 0, 0, m_dora};
    str += R"( seed=")" + to_string(seed) + R"(")";
    str += R"( ten=")" + to_string(m_ten) + R"(")";
    str += R"( oya=")" + to_string(m_oya) + R"(")";
    str += R"( hai0=")" + to_string(m_hai[0]) + R"(")";
    str += R"( hai1=")" + to_string(m_hai[1]) + R"(")";
    str += R"( hai2=")" + to_string(m_hai[2]) + R"(")";
    str += R"( hai3=")" + to_string(m_hai[3]) + R"(")";
    str += "/>";
}

string InitData::str(StringType type) const
{
    string ans;
    switch (type) {
        case StringType::XML: {
            to_xml(ans);
            break;
        }
        case StringType::BYTE: {
            Buffer q;
            to_bytes(q);
            ans = to_string(q);
            break;
        }
        default:
            break;
    }
    return ans;
}

bool InitData::operator==(const InitData &data) const noexcept
{
    return (m_round == data.m_round &&
            m_honba == data.m_honba &&
            m_kyotaku == data.m_kyotaku &&
            m_dora == data.m_dora &&
            m_ten == data.m_ten &&
            m_oya == data.m_oya &&
            m_hai == data.m_hai);
}

bool InitData::operator!=(const InitData &data) const noexcept
{
    return !(*this == data);
}


unique_ptr<EndData> EndData::from_bytes(Buffer &q)
{
    uint8_t type = q.front();
    if (MetaType::AGARI <= type && type < MetaType::RYUUKYOKU) {
        return EndAgari::from_bytes(q);
    } else if (MetaType::RYUUKYOKU <= type && type < MetaType::ROUND) {
        return EndRyuuKyoKu::from_bytes(q);
    } else {
        throw runtime_error("unknown type in EndData::from_bytes: " + to_string(type));
    }
}

void EndData::to_bytes(Buffer &q) const
{
    if (MetaType::AGARI <= m_type && m_type < MetaType::RYUUKYOKU) {
        static_cast<const EndAgari *>(this)->to_bytes(q);
    } else if (MetaType::RYUUKYOKU <= m_type && m_type < MetaType::ROUND) {
        static_cast<const EndRyuuKyoKu *>(this)->to_bytes(q);
    } else {
        throw runtime_error("unknown type in EndData::to_bytes: " + to_string(m_type));
    }
}

unique_ptr <EndData> EndData::from_xml(const string &name, const XmlMap &xmlMap)
{
    if (name == "AGARI") {
        return EndAgari::from_xml(name, xmlMap);
    } else if (name == "RYUUKYOKU") {
        return EndRyuuKyoKu::from_xml(name, xmlMap);
    } else {
        throw runtime_error("unknown type in EndData::from_bytes: " + name);
    }
}

void EndData::to_xml(string &str) const
{
    if (is_agari()) {
        static_cast<const EndAgari *>(this)->to_xml(str);
    } else if (is_ryuukyoku()) {
        static_cast<const EndRyuuKyoKu *>(this)->to_xml(str);
    } else {
        throw runtime_error("unknown type in EndData::to_bytes: " + to_string(m_type));
    }
}

bool EndData::is_agari() const noexcept
{
    return MetaType::AGARI <= m_type && m_type < MetaType::RYUUKYOKU;
}

bool EndData::is_ryuukyoku() const noexcept
{
    return MetaType::RYUUKYOKU <= m_type && m_type < MetaType::ROUND;
}

const EndAgari *EndData::to_agari() const
{
    if (!this->is_agari()) {
        throw runtime_error("Invalid EndData::to_agari.");
    }
    return static_cast<const EndAgari *>(this);
}

const EndRyuuKyoKu *EndData::to_ryuukyoku() const
{
    if (!this->is_ryuukyoku()) {
        throw runtime_error("Invalid EndData::to_agari.");
    }
    return static_cast<const EndRyuuKyoKu *>(this);
}

MetaType EndData::get_meta_type() const
{
    if (MetaType::AGARI <= m_type && m_type < MetaType::AGARI_YAKUMAN) {
        return MetaType::AGARI;
    } else if (MetaType::AGARI_YAKUMAN <= m_type && m_type < MetaType::RYUUKYOKU) {
        return MetaType::AGARI_YAKUMAN;
    } else if (MetaType::RYUUKYOKU <= m_type && m_type < MetaType::RYUUKYOKU_NM) {
        return MetaType::RYUUKYOKU;
    } else if (MetaType::RYUUKYOKU_NM <= m_type && m_type < MetaType::RYUUKYOKU_YAO9) {
        return MetaType::RYUUKYOKU_NM;
    } else if (MetaType::RYUUKYOKU_YAO9 <= m_type && m_type < MetaType::RYUUKYOKU_KAZE4) {
        return MetaType::RYUUKYOKU_YAO9;
    } else if (MetaType::RYUUKYOKU_KAZE4 <= m_type && m_type < MetaType::RYUUKYOKU_REACH4) {
        return MetaType::RYUUKYOKU_KAZE4;
    } else if (MetaType::RYUUKYOKU_REACH4 <= m_type && m_type < MetaType::RYUUKYOKU_RON3) {
        return MetaType::RYUUKYOKU_REACH4;
    } else if (MetaType::RYUUKYOKU_RON3 <= m_type && m_type < MetaType::RYUUKYOKU_KAN4) {
        return MetaType::RYUUKYOKU_RON3;
    } else if (MetaType::RYUUKYOKU_KAN4 <= m_type && m_type < MetaType::ROUND) {
        return MetaType::RYUUKYOKU_KAN4;
    } else {
        throw runtime_error("EndData::get_meta_type Unknown type");
    }
}

void EndData::init_from_bytes(Buffer &q)
{
    m_type = q.front();
    q.pop_front();
    m_honba = q.front();
    q.pop_front();
    m_kyotaku = q.front();
    q.pop_front();
    for (uint8_t i = 0; i < 8; ++i) {
        m_sc[i] = readFromBuf<int16_t>(q);
    }
}

void EndData::write_to_bytes(Buffer &q) const
{
    q.push_back(m_type);
    q.push_back(m_honba);
    q.push_back(m_kyotaku);
    for (uint8_t i = 0; i < 8; ++i) {
        writeToBuf<int16_t>(q, m_sc[i]);
    }
}

void EndData::init_from_xml(const string &name, const XmlMap &xmlMap)
{
    auto ba = splitToVecInt(xmlMap, "ba");
    auto sc_ = splitToVecInt(xmlMap, "sc");
    array<int16_t, 8> sc{};
    for (uint8_t i = 0; i < 8; ++i) {
        sc[i] = static_cast<int16_t>(sc_[i]);
    }

    uint8_t type;
    if (name == "AGARI") {
        bool isYaku = xmlMap.find("yaku") != xmlMap.end();
        bool isYakuMan = xmlMap.find("yakuman") != xmlMap.end();
        if (isYaku + isYakuMan != 1) {
            throw runtime_error("Invalid yaku/yakuman tag");
        }

        auto who = get_u2_from_attrs(xmlMap, "who");
        auto fromWho = get_u2_from_attrs(xmlMap, "fromWho");
        type = (isYaku ? MetaType::AGARI : MetaType::AGARI_YAKUMAN) + who + fromWho * 4;
    } else if (name == "RYUUKYOKU") {
        auto it = xmlMap.find("type");
        if (it == xmlMap.end()) {
            type = MetaType::RYUUKYOKU;
        } else if (it->second == "nm") {
            type = MetaType::RYUUKYOKU_NM;
        } else if (it->second == "yao9") {
            type = MetaType::RYUUKYOKU_YAO9;
        } else if (it->second == "kaze4") {
            type = MetaType::RYUUKYOKU_KAZE4;
        } else if (it->second == "reach4") {
            type = MetaType::RYUUKYOKU_REACH4;
        } else if (it->second == "ron3") {
            type = MetaType::RYUUKYOKU_RON3;
        } else if (it->second == "kan4") {
            type = MetaType::RYUUKYOKU_KAN4;
        } else {
            throw runtime_error("Invalid RYUUKYOKU type in init_from_xml: " + it->second);
        }
    } else {
        throw runtime_error("Invalid EndData type: " + name);
    }
    m_type = type;
    m_honba = static_cast<uint8_t>(ba[0]);
    m_kyotaku = static_cast<uint8_t>(ba[1]);
    m_sc = sc;
}

string EndData::str(StringType type) const
{
    string ans;
    switch (type) {
        case StringType::XML: {
            to_xml(ans);
            break;
        }
        case StringType::BYTE: {
            Buffer q;
            to_bytes(q);
            ans = to_string(q);
            break;
        }
        default:
            break;
    }
    return ans;
}

bool EndData::operator==(const EndData &data) const noexcept
{
    if (m_type != data.m_type ||
        m_honba != data.m_honba ||
        m_kyotaku != data.m_kyotaku ||
        m_sc != data.m_sc) {
        return false;
    }
    if (MetaType::AGARI <= m_type && m_type < MetaType::RYUUKYOKU) {
        return *static_cast<const EndAgari *>(this) == static_cast<const EndAgari &>(data);
    } else if (MetaType::RYUUKYOKU <= m_type && m_type < MetaType::ROUND) {
        return *static_cast<const EndRyuuKyoKu *>(this) == static_cast<const EndRyuuKyoKu &>(data);
    } else {
        return false;
    }
}

bool EndData::operator!=(const EndData &data) const noexcept
{
    return !(*this == data);
}


DoraMap get_dora_map(const DoraIndicator &indicator)
{
    DoraMap ans{};
    for (auto hai: indicator) {
        if (hai != HaiType::NO_HAI && hai >= 136) {
            throw runtime_error("Unknown hai: " + to_string(hai));
        }
        if (hai == HaiType::NO_HAI) {
            break;
        }
        uint8_t hai_idx = hai >> 2;
        switch (hai_idx) {
            case 8:     // 9m
            case 17:    // 9p
            case 26:    // 9s
                ++ ans[hai_idx - 8];
                break;
            case 30:    // 4z
                ++ ans[27];
                break;
            case 33:    // 7z
                ++ ans[31];
                break;
            default:
                ++ ans[hai_idx + 1];
                break;
        }
    }
    return ans;
}

bool is_aka_dora(uint8_t hai)
{
    return hai == 16 || hai == 52 || hai == 88;
}

array<uint8_t, 4> flatten_meld(uint16_t meld)
{
    if (meld & (1 << 2)) {                  // MetaType::CHI
        uint8_t t = meld >> 10;
        uint8_t hai_type = t / 3;           // 1-7m 1-7p 1-7s
        // uint8_t first_second_third = t % 3; // CHI first/second/third
        uint8_t color = hai_type / 7;       // m/p/s
        uint8_t num = hai_type % 7;         // 1-7 color
        uint8_t first = (color * 9 + num) * 4 + 4 * 0 + ((meld & 0b0000'0000'0001'1000) >> 3);
        uint8_t second = (color * 9 + num) * 4 + 4 * 1 + ((meld & 0b0000'0000'0110'0000) >> 5);
        uint8_t third = (color * 9 + num) * 4 + 4 * 2 + ((meld & 0b0000'0001'1000'0000) >> 7);
        return {first, second, third, HaiType::NO_HAI};
    } else if (meld & (1 << 3)) {           // MetaType::PON
        uint8_t t = meld >> 9;
        uint8_t base = (t / 3) * 4;
        array<uint8_t, 4> ans = {
            base,
            static_cast<uint8_t>(base + 1),
            static_cast<uint8_t>(base + 2),
            static_cast<uint8_t>(base + 3)
        };
        for (uint8_t i = ((meld & 0b0110'0000) >> 5); i < 3; ++i) {
            ans[i] = ans[i + 1];
        }
        ans[3] = HaiType::NO_HAI;
        return ans;
    } else if (meld & (1 << 4)) {           // MetaType::KA_KAN
        uint8_t t = meld >> 9;
        uint8_t base = (t / 3) * 4;
        return {
            base,
            static_cast<uint8_t>(base + 1),
            static_cast<uint8_t>(base + 2),
            static_cast<uint8_t>(base + 3)
        };
    } else if (meld & (1 << 5)) {           // MetaType::PEI
        throw runtime_error("not support three-player now");
    } else {                                // MetaType::AN_KAN or MetaType::MIN_KAN
        uint8_t base = (meld >> 8) & 0b1111'1100;
        return {
            base,
            static_cast<uint8_t>(base + 1),
            static_cast<uint8_t>(base + 2),
            static_cast<uint8_t>(base + 3)
        };
    }
}

HaiFlatten flatten_hai(const HaiCompress &hai_compress, uint8_t machi)
{
    HaiFlatten ans;
    ans.fill(HaiType::NO_HAI);
    uint8_t ans_idx = 0;

    for (uint8_t i = 0; i < 13; ++i) {
        auto hai = hai_compress[i];
        if (hai == HaiType::NO_HAI) {
            break;
        }
        if (hai == HaiType::MELD_TAG) {
            array<uint8_t, 4> meld_hai = flatten_meld(hai_compress[i + 1] | (hai_compress[i + 2] << 8));
            for (const auto hai2: meld_hai) {
                if (hai2 != HaiType::NO_HAI) {
                    ans[ans_idx++] = hai2;
                }
            }
            i += 2;
        } else {
            ans[ans_idx++] = hai;
        }
    }
    return ans;
}

uint8_t get_dora_count_of_hai(const DoraIndicator &indicator, HaiFlatten hai_flatten)
{
    uint8_t ans = 0;
    auto dora_map = get_dora_map(indicator);
    for (const auto hai: hai_flatten) {
        if (hai == HaiType::NO_HAI) {
            break;
        }
        ans += dora_map[hai / 4];
    }
    return ans;
}


unique_ptr <EndData> EndAgari::from_bytes(Buffer &q)
{
    EndAgari ans;
    ans.init_from_bytes(q);
    for (uint8_t i = 0; i < 13; ++i) {
        ans.m_hai[i] = q.front();
        q.pop_front();
    }
    ans.m_machi = q.front();
    q.pop_front();
    ans.m_ten0 = q.front();
    q.pop_front();
    ans.m_ten2 = q.front();
    q.pop_front();
    ans.m_ten1 = readFromBuf<uint16_t>(q);
    ans.m_yaku = bitset<55>(readFromBuf<uint64_t>(q));
    for (uint8_t i = 0; i < 5; ++i) {
        ans.m_doraHai[i] = q.front();
        q.pop_front();
    }
    for (uint8_t i = 0; i < 5; ++i) {
        ans.m_doraHaiUra[i] = q.front();
        q.pop_front();
    }
    return make_unique<EndAgari>(ans);
}

void EndAgari::to_bytes(Buffer &q) const
{
    EndData::write_to_bytes(q);
    for (uint8_t i = 0; i < 13; ++i) {
        q.push_back(m_hai[i]);
    }
    q.push_back(m_machi);
    q.push_back(m_ten0);
    q.push_back(m_ten2);
    writeToBuf<uint16_t>(q, m_ten1);
    writeToBuf<uint64_t>(q, m_yaku.to_ullong());
    for (uint8_t i = 0; i < 5; ++i) {
        q.push_back(m_doraHai[i]);
    }
    for (uint8_t i = 0; i < 5; ++i) {
        q.push_back(m_doraHaiUra[i]);
    }
}

unique_ptr<EndData> EndAgari::from_xml(const string &name, const XmlMap &xmlMap)
{
    HaiCompress hai{};
    hai.fill(HaiType::NO_HAI);
    uint8_t machi = get_u8_from_attrs(xmlMap, "machi");
    auto hai_ = splitToVecInt(xmlMap, "hai");
    uint8_t idx = 0;
    for (auto h: hai_) {
        if (h != machi) {
            hai[idx] = h;
            ++idx;
        }
    }
    auto m = splitToVecInt(xmlMap, "m");
    for (auto meld: m) {
        if (idx + 2 >= 13) {
            throw runtime_error("EndAgari::from_xml: hai index overflow");
        }
        hai[idx] = HaiType::MELD_TAG;
        hai[idx + 1] = meld & 0xff;
        hai[idx + 2] = meld >> 8;
        idx += 3;
    }
    auto ten = splitToVecInt(xmlMap, "ten");
    auto yaku_ = splitToVecInt(xmlMap, "yaku");
    auto yakuman_ = splitToVecInt(xmlMap, "yakuman");
    bitset<55> yaku;
    for (uint32_t i = 0; i < yaku_.size(); i += 2) {
        yaku.set(yaku_[i]);
    }
    for (uint32_t i = 0; i < yakuman_.size(); i += 1) {
        yaku.set(yakuman_[i]);
    }
    DoraIndicator doraHai{};
    doraHai.fill(HaiType::NO_HAI);
    DoraIndicator doraHaiUra{};
    doraHaiUra.fill(HaiType::NO_HAI);
    auto doraHai_ = splitToVecInt(xmlMap, "doraHai");
    auto doraHaiUra_ = splitToVecInt(xmlMap, "doraHaiUra");
    for (uint32_t i = 0; i < doraHai_.size(); ++i) {
        doraHai[i] = doraHai_[i];
    }
    for (uint32_t i = 0; i < doraHaiUra_.size(); ++i) {
        doraHaiUra[i] = doraHaiUra_[i];
    }

    EndAgari ans{
            .m_hai=hai,
            .m_machi=machi,
            .m_ten0=static_cast<uint8_t>(ten[0]),
            .m_ten2=static_cast<uint8_t>(ten[2]),
            .m_ten1=static_cast<uint16_t>(ten[1] / 100),
            .m_yaku=yaku,
            .m_doraHai=doraHai,
            .m_doraHaiUra=doraHaiUra,
    };
    ans.init_from_xml(name, xmlMap);
    return make_unique<EndAgari>(ans);
}

void EndAgari::to_xml(string &str) const
{
    str += "<AGARI";
    bool isYaku = MetaType::AGARI <= m_type && m_type < MetaType::AGARI_YAKUMAN;
    auto base_type = isYaku ? MetaType::AGARI : MetaType::AGARI_YAKUMAN;
    vector<uint8_t> ba{m_honba, m_kyotaku};
    vector<uint32_t> ten{m_ten0, static_cast<uint32_t>(m_ten1 * 100), m_ten2};
    vector<uint8_t> hai{m_machi};
    vector<uint16_t> m;
    for (uint8_t i = 0; i < 13; ++i) {
        if (m_hai[i] != HaiType::MELD_TAG) {
            hai.push_back(m_hai[i]);
        } else {
            m.push_back(m_hai[i + 1] | (m_hai[i + 2] << 8));
            i += 2;
        }
    }
    sort(hai.begin(), hai.end());
    vector<uint8_t> yaku;
    for (uint8_t i = 0; i < 55; ++i) {
        if (m_yaku.test(i)) {
            yaku.push_back(i);
            if (isYaku) {
                yaku.push_back(0);
            }
        }
    }
    str += R"( ba=")" + to_string(ba) + R"(")";
    str += R"( hai=")" + to_string(hai) + R"(")";
    str += R"( m=")" + to_string(m) + R"(")";
    str += R"( machi=")" + to_string(m_machi) + R"(")";
    str += R"( ten=")" + to_string(ten) + R"(")";
    if (isYaku) {
        str += R"( yaku=")" + to_string(yaku) + R"(")";     // 没有计算对应的番数，失去插入顺序
    } else {
        str += R"( yakuman=")" + to_string(yaku) + R"(")";
    }
    str += R"( doraHai=")" + to_string(m_doraHai) + R"(")";
    str += R"( doraHaiUra=")" + to_string(m_doraHaiUra) + R"(")";
    str += R"( who=")" + to_string((m_type - base_type) % 4) + R"(")";
    str += R"( fromWho=")" + to_string((m_type - base_type) / 4) + R"(")";
    str += R"( sc=")" + to_string(m_sc) + R"(")";
    str += " />";
}

string EndAgari::str(StringType type) const
{
    string ans;
    switch (type) {
        case StringType::XML: {
            to_xml(ans);
            break;
        }
        case StringType::BYTE: {
            Buffer q;
            to_bytes(q);
            ans = to_string(q);
            break;
        }
        default:
            break;
    }
    return ans;
}

uint8_t EndAgari::get_who() const noexcept
{
    return m_type % 4;
}

uint8_t EndAgari::get_from_who() const noexcept
{
    return ((m_type - MetaType::AGARI) / 4) % 4;
}

bool EndAgari::is_tsumo() const noexcept
{
    return get_who() == get_from_who();
}

bool EndAgari::is_meld() const noexcept
{
    return std::any_of(m_hai.begin(), m_hai.end(), [](const auto& hai) { return hai == HaiType::MELD_TAG; });
}

bool EndAgari::is_yakuman() const noexcept
{
    return MetaType::AGARI_YAKUMAN <= m_type && m_type < MetaType::RYUUKYOKU;
}

bool EndAgari::is_richi() const noexcept
{
    return m_yaku.test(YakuType::Riichi);
}

int32_t EndAgari::get_score() const noexcept
{
    return m_ten1 * 100;
}

HaiFlatten EndAgari::get_hai_flatten() const
{
    return flatten_hai(m_hai, m_machi);
}

uint8_t EndAgari::get_dora_cnt(const HaiFlatten &hai_flatten) const
{
    return get_dora_count_of_hai(m_doraHai, hai_flatten);
}

uint8_t EndAgari::get_dora_cnt() const
{
    return get_dora_cnt(get_hai_flatten());
}

uint8_t EndAgari::get_ura_dora_cnt(const HaiFlatten &hai_flatten) const
{
    return get_dora_count_of_hai(m_doraHaiUra, hai_flatten);
}

uint8_t EndAgari::get_ura_dora_cnt() const
{
    return get_ura_dora_cnt(get_hai_flatten());
}

uint8_t EndAgari::get_aka_dora_cnt(const HaiFlatten &hai_flatten) const
{
    uint8_t ans = 0;
    for (const auto hai: hai_flatten) {
        if (hai == HaiType::NO_HAI) {
            break;
        }
        ans += is_aka_dora(hai);
    }
    return ans;
}

uint8_t EndAgari::get_aka_dora_cnt() const
{
    return get_aka_dora_cnt(get_hai_flatten());
}

bool EndAgari::operator==(const EndAgari &data) const noexcept
{
    return (m_hai == data.m_hai &&
            m_machi == data.m_machi &&
            m_ten0 == data.m_ten0 &&
            m_ten2 == data.m_ten2 &&
            m_ten1 == data.m_ten1 &&
            m_yaku == data.m_yaku &&
            m_doraHai == data.m_doraHai &&
            m_doraHaiUra == data.m_doraHaiUra);
}

bool EndAgari::operator!=(const EndAgari &data) const noexcept
{
    return !(*this == data);
}


unique_ptr <EndData> EndRyuuKyoKu::from_bytes(Buffer &q)
{
    EndRyuuKyoKu ans{};
    ans.init_from_bytes(q);
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 13; ++j) {
            ans.m_hai[i][j] = q.front();
            q.pop_front();
        }
    }
    return make_unique<EndRyuuKyoKu>(ans);
}

void EndRyuuKyoKu::to_bytes(Buffer &q) const
{
    EndData::write_to_bytes(q);
    for (uint8_t i = 0; i < 4; ++i) {
        for (uint8_t j = 0; j < 13; ++j) {
            q.push_back(m_hai[i][j]);
        }
    }
}

unique_ptr <EndData> EndRyuuKyoKu::from_xml(const string &name, const XmlMap &xmlMap)
{
    EndRyuuKyoKu ans{};
    ans.init_from_xml(name, xmlMap);
    vector<vector<int32_t> > hai{
            splitToVecInt(xmlMap, "hai0"),
            splitToVecInt(xmlMap, "hai1"),
            splitToVecInt(xmlMap, "hai2"),
            splitToVecInt(xmlMap, "hai3"),
    };
    for (uint32_t i = 0; i < 4; ++i) {
        ans.m_hai[i].fill(HaiType::NO_HAI);
        for (uint32_t j = 0; j < hai[i].size(); ++j) {
            ans.m_hai[i][j] = hai[i][j];
        }
    }
    return make_unique<EndRyuuKyoKu>(ans);
}

void EndRyuuKyoKu::to_xml(string &str) const
{
    vector<uint8_t> ba{m_honba, m_kyotaku};
    str += "<RYUUKYOKU";
    switch (m_type) {
        case MetaType::RYUUKYOKU:
            break;
        case MetaType::RYUUKYOKU_NM:
            str += R"( type="nm")";
            break;
        case MetaType::RYUUKYOKU_YAO9:
            str += R"( type="yao9")";
            break;
        case MetaType::RYUUKYOKU_KAZE4:
            str += R"( type="kaze4")";
            break;
        case MetaType::RYUUKYOKU_REACH4:
            str += R"( type="reach4")";
            break;
        case MetaType::RYUUKYOKU_RON3:
            str += R"( type="ron3")";
            break;
        case MetaType::RYUUKYOKU_KAN4:
            str += R"( type="kan4")";
            break;
        default:
            throw runtime_error("unknown RYUUKYOKU type in to_xml: " + to_string(m_type));
    }
    str += R"( ba=")" + to_string(ba) + R"(")";
    str += R"( sc=")" + to_string(m_sc) + R"(")";
    for (uint32_t i = 0; i < 4; ++i) {
        if (m_hai[i][0] != HaiType::NO_HAI) {
            vector<uint8_t> hai;
            for (uint32_t j = 0; j < 13; ++j) {
                if (m_hai[i][j] == HaiType::NO_HAI) {
                    break;
                }
                hai.push_back(m_hai[i][j]);
            }
            str += R"( hai)" + to_string(i) + R"(=")" + to_string(hai) + R"(")";
        }
    }
    str += " />";
}

string EndRyuuKyoKu::str(StringType type) const
{
    string ans;
    switch (type) {
        case StringType::XML: {
            to_xml(ans);
            break;
        }
        case StringType::BYTE: {
            Buffer q;
            to_bytes(q);
            ans = to_string(q);
            break;
        }
        default:
            break;
    }
    return ans;
}

bool EndRyuuKyoKu::operator==(const EndRyuuKyoKu &data) const noexcept
{
    return m_hai == data.m_hai;
}

bool EndRyuuKyoKu::operator!=(const EndRyuuKyoKu &data) const noexcept
{
    return !(*this == data);
}


PlayerStatus::PlayerStatus() noexcept
        : richi(false)
        , richi_ok(false)
        , first_richi(false)
        , chasing_richi(false)
        , be_chased_richi(false)
        , be_chased_richi_ok(false)
        , agari(false)
        , draw(false)
        , be_ron(false)
        , be_tsumo(false)
        , agari_type(0)
        , ryuukyoku_type(0)
        , richi_num(UINT8_MAX)
        , end_num(UINT8_MAX)
{
}

RoundData RoundData::from_bytes(Buffer &q)
{
    uint8_t type = q.front();
    q.pop_front();
    if (type != MetaType::ROUND) {
        throw runtime_error("Invalid round type in RoundData::from_bytes: " + to_string(type));
    }

    auto len = readFromBuf<uint16_t>(q);
    uint32_t size0 = q.size();
    RoundData ans{};
    ans.m_init = InitData::from_bytes(q);
    auto num_action = readFromBuf<uint16_t>(q);
    ans.m_actions.reserve(num_action);
    for (uint16_t i = 0; i < num_action; ++i) {
        ans.m_actions.emplace_back(Action::from_bytes(q));
    }
    auto num_end = readFromBuf<uint8_t>(q);
    for (uint16_t i = 0; i < num_end; ++i) {
        ans.m_ends[i] = EndData::from_bytes(q);
    }
    if (size0 != len + q.size()) {
        throw runtime_error("Invalid RoundData::from_bytes len" + to_string(len) +
                            "size0 = " + to_string(size0) + "size1 = " + to_string(q.size()));
    }
    ans.preprocess_player_status();
    return ans;
}

void RoundData::to_bytes(Buffer &q) const
{
    writeToBuf<uint8_t>(q, MetaType::ROUND);

    uint32_t len_pos = q.size();
    writeToBuf<uint16_t>(q, 0);

    m_init.to_bytes(q);
    writeToBuf<uint16_t>(q, static_cast<uint16_t>(m_actions.size()));
    for (const auto &action: m_actions) {
        action.to_bytes(q);
    }

    uint32_t num_end_pos = q.size();
    writeToBuf<uint8_t>(q, 0);

    uint8_t num_end = 0;
    for (const auto &item: m_ends) {
        if (item == nullptr) {
            break;
        }
        item->to_bytes(q);
        ++num_end;
    }

    overwriteToBuf<uint16_t>(q, static_cast<uint16_t>(q.size() - len_pos - sizeof(uint16_t)), len_pos);
    overwriteToBuf<uint8_t>(q, num_end, num_end_pos);
}

RoundData RoundData::from_xml(XmlParser &parser)
{
    RoundData data{};
    auto [name, attrs] = parser.getOneNode();
    data.m_init = InitData::from_xml(name, attrs);
    parser.nextNode();
    while (true) {
        std::tie(name, attrs) = parser.getOneNode();
        auto action = Action::from_xml(name, attrs);
        if (action.bytes[0] == 0xff && action.bytes[1] == 0xff) {
            break;
        }
        parser.nextNode();
        data.m_actions.emplace_back(action);
    }
    uint32_t i = 0;
    while (true) {
        std::tie(name, attrs) = parser.getOneNode();
        if (name == "" || name == "INIT") {
            break;
        }
        parser.nextNode();
        // 正常来说这里应该都是AGARI/RYUUKYOKU，其后就是下一把的INIT或者结束
        // 但有一个例外case，就是AGARI/RYUUKYOKU后有人掉线/重连了
        // 这种异常case，处理方法时将掉线重连视为AGARI/RYUUKYOKU前
        if (name == "BYE" || name == "UN") {
            auto action = Action::from_xml(name, attrs);
            data.m_actions.emplace_back(action);
            continue;
        }
        data.m_ends[i++] = EndData::from_xml(name, attrs);
    }
    if (i == 0 || i > 4) {
        throw runtime_error("No/More ROUND end tag in RoundData::from_xml: " + to_string(i) + parser.m_str);
    }
    if (name == "" || name == "INIT") {
        data.preprocess_player_status();
        return data;
    }
    throw runtime_error("Invalid ROUND end tag in RoundData::from_xml: " + name);
}

void RoundData::to_xml(string &str) const
{
    m_init.to_xml(str);
    for (const auto &action: m_actions) {
        action.to_xml(str);
    }
    for (uint32_t i = 0; i < 4; ++i) {
        if (m_ends[i] == nullptr) {
            break;
        }
        m_ends[i]->to_xml(str);
    }
}

void RoundData::preprocess_player_status()
{
    uint8_t richi_num = 0;
    uint8_t prev_richi[4] = {UINT8_MAX, UINT8_MAX, UINT8_MAX, UINT8_MAX};
    array<uint8_t, 4> sub_round_num{};  // 总巡目数
    for (const auto &action: m_actions) {
        uint8_t t = action.bytes[0];
        uint8_t who = t & 0x03;
        if (MetaType::TUVW <= t && t < MetaType::DEFG) {
            ++sub_round_num[who];
        } else if (MetaType::RICHI1 <= t && t < MetaType::RICHI2) {
            m_players[who].richi = true;
            m_players[who].richi_num = sub_round_num[who];
            if (prev_richi[0] == UINT8_MAX) {   // 如果无人曾立直，则这是先制立直
                m_players[who].first_richi = true;
            } else {    // 有人曾立直，则这是追立直
                m_players[who].chasing_richi = true;
                for (uint32_t i = 0; i < richi_num; ++i) {  // 曾立直的人均被追立直
                    m_players[prev_richi[i]].be_chased_richi = true;
                }
            }
        } else if (MetaType::RICHI2 <= t && t < MetaType::BYE) {
            m_players[who].richi_ok = true;
            for (uint32_t i = 0; i < richi_num; ++i) {
                m_players[prev_richi[i]].be_chased_richi_ok = true;
            }
            prev_richi[richi_num++] = who;  // 立直成功后，记录本家，后续人立直时会记录追立直
        }
    }
    for (uint32_t i = 0; i < 4; ++i) {
        m_players[i].end_num = sub_round_num[i];
    }

    for (const auto &end_item: m_ends) {
        if (end_item == nullptr) {
            break;
        }
        if (end_item->is_agari()) {
            const auto agari = end_item->to_agari();
            uint8_t who = agari->get_who();
            uint8_t fromWho = agari->get_from_who();
            bool is_ron = who != fromWho;
            if (is_ron) {
                m_players[who].agari = true;
                if (m_players[fromWho].richi_ok) {
                    m_players[who].agari_type = StatusAgariType::RON_REACH2;
                } else if (m_players[fromWho].richi) {
                    m_players[who].agari_type = StatusAgariType::RON_REACH1;
                } else {
                    m_players[who].agari_type = StatusAgariType::RON_NO_REACH;
                }
                m_players[fromWho].be_ron = true;
                for (uint32_t i = 0; i < 4; ++i) {
                    if (i != who && i != fromWho) {
                        m_players[i].draw = true;
                    }
                }
            } else {
                m_players[who].agari = true;
                m_players[who].agari_type = StatusAgariType::TSUMO;
                for (uint32_t i = 0; i < 4; ++i) {
                    if (i != who) {
                        m_players[i].be_tsumo = true;
                    }
                }
            }
        } else {
            for (uint32_t i = 0; i < 4; ++i) {
                m_players[i].ryuukyoku_type = end_item->m_type - MetaType::RYUUKYOKU + 1;
            }
        }
    }
}

string RoundData::str(StringType type) const
{
    string ans;
    switch (type) {
        case StringType::XML: {
            to_xml(ans);
            break;
        }
        case StringType::BYTE: {
            Buffer q;
            to_bytes(q);
            ans = to_string(q);
            break;
        }
        default:
            break;
    }
    return ans;
}

int32_t RoundData::get_last_end_idx() const
{
    int32_t idx;
    for (idx = 3; idx >= 0; --idx) {
        if (m_ends[idx]) {
            return idx;
        }
    }
    throw runtime_error("No end data");
}

bool RoundData::operator==(const RoundData &data) const noexcept
{
    if (m_init != data.m_init ||
        m_actions != data.m_actions) {
        return false;
    }
    for (uint32_t i = 0; i < 4; ++i) {
        if ((m_ends[i] == nullptr) + (data.m_ends[i] == nullptr) == 2) {
            break;
        } else if ((m_ends[i] == nullptr) + (data.m_ends[i] == nullptr) == 1) {
            return false;
        }
        if (*m_ends[i] != *data.m_ends[i]) {
            return false;
        }
    }
    return true;
}

bool RoundData::operator!=(const RoundData &data) const noexcept
{
    return !(*this == data);
}

MjlogGame MjlogGame::from_bytes(Buffer &q)
{
    MjlogGame result{};
    uint8_t type = q.front();
    q.pop_front();
    if (type != MetaType::GAME) {
        throw runtime_error("Invalid round type in MjlogGame::from_bytes: " + to_string(type));
    }

    auto len = readFromBuf<uint32_t>(q);
    uint32_t size0 = q.size();
    auto type_game = readFromBuf<uint16_t>(q);
    result.m_type = type_game;
    while (size0 < len + q.size()) {
        result.m_rounds.emplace_back(RoundData::from_bytes(q));
    }
    if (size0 != len + q.size()) {
        throw runtime_error("Invalid length in MjlogGame::from_bytes: size0 = " + to_string(size0) +
                            ", len = " + to_string(len) + ", q.size() = " + to_string(q.size()));
    }
    return result;
}

void MjlogGame::to_bytes(Buffer &q) const
{
    q.push_back(MetaType::GAME);

    uint32_t len_pos = q.size();
    writeToBuf<uint32_t>(q, 0);

    writeToBuf<uint16_t>(q, m_type);

    for (const auto &r: m_rounds) {
        r.to_bytes(q);
    }

    overwriteToBuf<uint32_t>(q, static_cast<uint32_t>(q.size() - len_pos - sizeof(uint32_t)), len_pos);
}

MjlogGame MjlogGame::from_xml(const string &str)
{
    XmlParser parser(str);
    MjlogGame result{};
    {
        auto [name, attrs] = parser.getOneNode();
        parser.nextNode();
        if (name != "SHUFFLE") {
            throw runtime_error("Invalid tag: expect SHUFFLE but " + name);
        }
    }
    {
        auto [name, attrs] = parser.getOneNode();
        parser.nextNode();
        if (name != "GO") {
            throw runtime_error("Invalid tag: expect GO but " + name);
        }
        result.m_type = get_u16_from_attrs(attrs, "type");
    }
    {
        auto [name, attrs] = parser.getOneNode();
        parser.nextNode();
        if (name != "UN") {
            throw runtime_error("Invalid tag: expect UN but " + name);
        }
    }
    {
        auto [name, attrs] = parser.getOneNode();
        parser.nextNode();
        if (name != "TAIKYOKU") {
            throw runtime_error("Invalid tag: expect TAIKYOKU but " + name);
        }
    }
    // 正常来说就该INIT了，但有一种异常case，就是INIT前有人直接掉线了，这种case直接忽略掉BYE和RECONNECT，简单处理
    // @todo 这种情况需要记录吗？其实我个人都不太想处理这个掉线/重连的，懒得搞
    {
        while (parser.getOneNode().first != "INIT") {
            if (parser.getOneNode().first == "BYE" || parser.getOneNode().first == "UN") {
                parser.nextNode();
                continue;
            }
            throw runtime_error("Invalid tag after TAIKYOKU: " + parser.getOneNode().first);
        }
    }
    while (parser.isValid()) {
        result.m_rounds.emplace_back(RoundData::from_xml(parser));
    }
    return result;
}

void MjlogGame::to_xml(string &str) const
{
    str += R"(<mjloggm ver="2.3">)";
    str += R"(<SHUFFLE />)";
    str += R"(<GO type=")" + to_string(m_type) + R"("/>)";
    str += R"(<UN />)";
    str += R"(<TAIKYOKU />)";
    for (const auto &r: m_rounds) {
        r.to_xml(str);
    }
    str += R"(</mjloggm>)";
}

string MjlogGame::str(StringType type) const
{
    string ans;
    switch (type) {
        case StringType::XML: {
            to_xml(ans);
            break;
        }
        case StringType::BYTE: {
            Buffer q;
            to_bytes(q);
            ans = to_string(q);
            break;
        }
        default:
            break;
    }
    return ans;
}

bool MjlogGame::operator==(const MjlogGame &data) const noexcept
{
    return (m_type == data.m_type &&
            m_rounds == data.m_rounds);
}

bool MjlogGame::operator!=(const MjlogGame &data) const noexcept
{
    return !(*this == data);
}

}   // namespace Hasaki
