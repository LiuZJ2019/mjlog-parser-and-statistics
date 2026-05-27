/**
 * @file        json_util.h
 * @brief       将输出转换为json格式，用于外部程序分析
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     将输出转换为json格式，用于外部程序分析，仅main.cpp使用
 */
#ifndef HASAKI_MJLOG_PARSER_UTIL_JSON_UTIL_H_
#define HASAKI_MJLOG_PARSER_UTIL_JSON_UTIL_H_


#include <map>
#include <tuple>
#include <vector>
#include <string>
#include <cstdint>
#include <variant>

namespace Hasaki {

using std::map;
using std::pair;
using std::tuple;
using std::vector;
using std::string;
using std::variant;

#define JSON_TYPE_LIST          \
    X(NUM_OCCUR_TIME)           \
    X(NUM_TO_MEAN_STD_SAMPLE)

enum class JsonOutputDataType {
#define X(name) name,
    JSON_TYPE_LIST
#undef X
};

using JsonOutputData = variant<
    map<int32_t, int32_t>,
    map<int32_t, map<int32_t, int32_t> >
>;

struct JsonContainer {
    vector<tuple<string, JsonOutputDataType, JsonOutputData> > all_data;

    JsonContainer()=default;

    void add_data(const string &name, JsonOutputDataType type, JsonOutputData data);
    string to_json_string() const;

    template<JsonOutputDataType T>
    string to_json_string(const JsonOutputData &data) const;
};

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_UTIL_JSON_UTIL_H_
