/**
 * @file        json_util.cpp
 * @brief       将输出转换为json格式，用于外部程序分析
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     实现了结果到json转换的注册方法
 */
#include "json_util.h"
#include <cmath>
#include <stdexcept>


namespace Hasaki {

using std::to_string;
using std::runtime_error;

string StringJoin(const vector<string> &vec_string, const string &delim)
{
    string ans;
    if (!vec_string.empty()) {
        ans += vec_string[0];
    }
    for (size_t i = 1; i < vec_string.size(); ++ i) {
        ans += delim + vec_string[i];
    }
    return ans;
}

tuple<double, double, int32_t> CalcMeanStdTotal(const map<int32_t, int32_t> &occur_time)
{
    int32_t total = 0;
    double mean = 0;
    double std_sample = 0;
    for (const auto &[k, v]: occur_time) {
        total += v;
        mean += k * v;
    }
    if (total <= 1) {
        return {mean, std_sample, total};
    }
    mean /= total;
    for (const auto &[k, v]: occur_time) {
        std_sample += (k - mean) * (k - mean) * v;
    }
    std_sample /= total - 1;
    std_sample = sqrt(std_sample);
    return {mean, std_sample, total};
}

void JsonContainer::add_data(const string &name, JsonOutputDataType type, JsonOutputData data)
{
    all_data.emplace_back(name, type, data);
}

template<>
string JsonContainer::to_json_string<JsonOutputDataType::NUM_OCCUR_TIME>(const JsonOutputData &data) const
{
    string ans;
    ans += "{ ";
    vector<string> vec_string;
    for (const auto &[k, v]: std::get<map<int32_t, int32_t> >(data)) {
        string s = R"(")" + to_string(k) + R"(": )" + to_string(v);
        vec_string.emplace_back(std::move(s));
    }
    ans += StringJoin(vec_string, ", ");
    ans += " }";
    return ans;
}

template<>
string JsonContainer::to_json_string<JsonOutputDataType::NUM_TO_MEAN_STD_SAMPLE>(const JsonOutputData &data) const
{
    string ans;
    ans += "{\n";
    vector<string> vec_string;
    for (const auto &[k, sub_data]: std::get<map<int32_t, map<int32_t, int32_t> > >(data)) {
        string s;
        auto [mean, std_sample, total] = CalcMeanStdTotal(sub_data);
        s += R"(      ")" + to_string(k) + R"(": { "mean": )" + to_string(mean) + R"(, "std": )" + to_string(std_sample) +
             R"(, "total": )" + to_string(total) + " }";
        vec_string.emplace_back(std::move(s));
    }
    ans += StringJoin(vec_string, ",\n");
    ans += "\n";
    ans += "    }";
    return ans;
}

string JsonContainer::to_json_string() const
{
    map<string, int32_t> cnt;
    for (const auto &[name, type, value]: all_data) {
        cnt[name] += 1;
        if (cnt[name] > 1) {
            throw runtime_error("JsonContainer::to_json_string exist name: " + name);
        }
    }
    string ans;
    ans += "[\n";
    vector<string> vec_string;
    for (const auto &[name, type, value]: all_data) {
        string now_block;
        now_block += "  {\n";
        now_block += R"(    "name": ")" + name + R"(")" + ",\n";
        now_block += R"(    "type": ")";
        switch (type) {
#define X(type_) case JsonOutputDataType::type_: now_block += #type_; break;
            JSON_TYPE_LIST
#undef X
        }
        now_block += R"(")" + string(",\n");

        now_block += R"(    "value": )";
        switch (type) {
#define X(type_) case JsonOutputDataType::type_: now_block += to_json_string<JsonOutputDataType::type_>(value); break;
            JSON_TYPE_LIST
#undef X
        }

        now_block += "\n";
        now_block += "  }";
        vec_string.emplace_back(std::move(now_block));
    }
    ans += StringJoin(vec_string, ",\n");
    ans += "\n]\n";
    return ans;
}

}   // namespace Hasaki
