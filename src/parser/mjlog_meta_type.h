/**
 * @file        mjlog_meta_type.h
 * @brief       定义了parser相关的类型常量，.hskmjlog的type也都是按照这个类型标记的
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     MetaType的设计是为了让Action能够极致压缩编码到2Byte内，从而能够大幅节省时间和空间开销
 *              因为一局中Action重复次数很多，每个Action都只占2字节的话，空间占用会很低，缓存命中率提高
 *              主要难点在于处理副露，因为副露状态码m是uint16_t，则一个副露行为就要占4字节（结构体对齐）
 *              由于Action需要用vector<Action>存储，相当于空间开销翻倍了，这是无法接受的
 *              MetaType通过一些位运算操作，将副露type+m压缩到了2字节内，从而节省了一半的Action空间开销
 *              具体而言，Action除了DORA以外都有who，对应玩家编号，所以所有Action都是以4的倍数开始，最后2bit代表who
 *              各个副露类型的转换规则写到注释里了，通过tests代码遍历扫描，get_m总是能返回原本的真实副露类型
 */
#ifndef HASAKI_MJLOG_PARSER_PARSER_META_TYPE_H_
#define HASAKI_MJLOG_PARSER_PARSER_META_TYPE_H_


namespace Hasaki {

enum MetaType: uint8_t {
    TUVW = 4,               // 摸(4 - 7)
    DEFG = 8,               // 切(8 - 11)
    CHI = 12,               // 吃(12 - 75)
    // type = CHI + who + m[12:15] * 4
    // value = m[3:8] m[10:11]
    PON = 76,               // 碰(76 - 99)
    // type = PON + who + (m[0:1] - 1) * 4 + m[15] * 12
    // value = m[5:6] m[9:14]
    KA_KAN = 100,           // 加杠(100 - 123)
    // type = KA_KAN + who + (m[0:1] - 1) * 4 + m[15] * 12
    // value = m[5:6] m[9:14]
    PEI = 124,              // 拔北(124 - 127)
    AN_KAN = 128,           // 暗杠(128 - 131)
    // type = AN_KAN + who
    // value = m[9:15]
    MIN_KAN = 132,          // 明杠(132 - 143)
    // type = MIN_KAN + who + (m[0:1] - 1) * 4
    // value = m[9:15]
    RICHI1 = 144,           // 立直1(144 - 147)
    RICHI2 = 148,           // 立直2(148 - 151)
    BYE = 152,              // 退出(152 - 155)
    RECONNECT = 156,        // 重连(156 - 159)
    DORA = 160,             // 翻宝牌(160)
    AGARI = 164,            // 胡牌(164 - 179): 164 + who + fromWho * 4
    AGARI_YAKUMAN = 180,    // 役满胡牌(180 - 195): 180 + who + fromWho * 4
    RYUUKYOKU = 196,        // 荒牌流局
    RYUUKYOKU_NM = 197,     // 流局满贯
    RYUUKYOKU_YAO9 = 198,   // 九种九牌
    RYUUKYOKU_KAZE4 = 199,  // 四风连打
    RYUUKYOKU_REACH4 = 200, // 四家立直
    RYUUKYOKU_RON3 = 201,   // 三家和了
    RYUUKYOKU_KAN4 = 202,   // 四杠散了
    ROUND = 203,            // 一轮的开始标识符
    GAME = 204,             // 一局的开始标识符
};

// 默认牌为NO_HAI，这是可能存在的，比如流局的时候不会记录玩家副露了哪些牌，就会少牌
// MELD_TAG的意思是，该字节后续2字节对应了副露行为的m，本字节本身无含义
enum HaiType: uint8_t {
    NO_HAI = 253,
    MELD_TAG = 254
};

// 支持的输出类型，目前只支持xml和byte类型
enum StringType: uint8_t {
    XML = 0,
    BYTE = 1
};

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_PARSER_META_TYPE_H_
