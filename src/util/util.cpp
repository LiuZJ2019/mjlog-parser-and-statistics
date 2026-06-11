/**
 * @file        util.cpp
 * @brief       通用模块
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     通用模块的实现，部分是用AI(deepseek)写的，不过我都逐行review/重写过了
 */
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <charconv>
#include "util.h"


namespace Hasaki {

using std::ifstream;
using std::ofstream;
using std::ios;
using std::streamsize;

Buffer load_hskmjlog(const fs::path &file_path)
{
    ifstream file(file_path, ios::binary | ios::ate);
    if (!file.is_open()) {
        throw runtime_error("Cannot open hskmjlog file: " + file_path.string());
    }

    streamsize size = file.tellg();
    if (size <= 0) {
        throw runtime_error("hskmjlog file is empty: " + file_path.string());
    }

    file.seekg(0, ios::beg);

    vector<uint8_t> all(static_cast<size_t>(size));
    if (!file.read(reinterpret_cast<char*>(all.data()), size)) {
        throw runtime_error("读取文件失败: " + file_path.string());
    }

    if (all.empty() || all[0] != 1) {
        throw runtime_error("load hskmjlog file error: " + file_path.string() +
                            ": invalid version" + (all.empty() ? "?"s : std::to_string(all[0])));
    }
    return {all.begin() + 1, all.end()};
}

void save_hskmjlog(const fs::path &file_path, const Buffer &buffer)
{
    if (fs::exists(file_path)) {
        cout << "In save_hskmjlog, file exist and will be overwrite: " << file_path << endl;
    }

    vector<uint8_t> data;
    data.reserve(1 + buffer.size());
    data.push_back(1);
    data.insert(data.end(), buffer.begin(), buffer.end());

    ofstream file(file_path, ios::binary | ios::trunc);
    if (!file.is_open()) {
        throw runtime_error("In save_hskmjlog: cannot create file: " + file_path.string());
    }

    if (!file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()))) {
        throw runtime_error("In save_hskmjlog: error in write file" + file_path.string());
    }

    file.flush();
    if (!file.good()) {
        throw runtime_error("In save_hskmjlog: error in check write file" + file_path.string());
    }
}

vector<pair<string, string> > get_subdirectories(const string &dir_path)
{
    vector<pair<string, string> > subdirs;

    fs::path base_path(dir_path);
    if (!fs::exists(base_path) || !fs::is_directory(base_path)) {
        cerr << "Error in get_subdirectories: " << dir_path << endl;
        return subdirs;
    }

    for (const auto &entry : fs::directory_iterator(base_path)) {
        if (fs::is_directory(entry.status())) {
            string full_path = entry.path().generic_string();
            string folder_name = entry.path().filename().generic_string();
            subdirs.emplace_back(std::move(full_path), std::move(folder_name));
        }
    }

    return subdirs;
}

map<string, fs::path> getAllPath(const string &rootDir, const string &suffix)
{
    map<string, fs::path> result;
    if (!fs::exists(rootDir) || !fs::is_directory(rootDir)) {
        throw runtime_error("Invalid directory: " + rootDir);
    }

    for (const auto &entry : fs::recursive_directory_iterator(rootDir)) {
        if (entry.is_regular_file() && entry.path().extension() == suffix) {
            string fileName = entry.path().filename().string();
            fs::path filePath = entry.path().string();

            if (result.find(fileName) != result.end()) {
                cout << "Same filename: " << fileName << endl;
                continue;
            }
            result[fileName] = filePath;
        }
    }
    return result;
}

string readAsUtf8String(const fs::path &filePath)
{
    string content;
    ifstream ifs(filePath, ios::in | ios::binary);
    if (!ifs) {
        throw runtime_error("Invalid file_path: "s + filePath.string());
    }
    content.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    return content;
}

string to_string(const Buffer &arr)
{
    std::ostringstream oss;
    for (uint8_t b : arr) {
        oss << std::hex << std::setw(2) << std::setfill('0')
            << static_cast<int>(b) << ' ';
    }
    return oss.str();
}

string to_string(uint8_t num)
{
    return std::to_string(num);
}

string to_string(uint16_t num)
{
    return std::to_string(num);
}

string to_string(int16_t num)
{
    return std::to_string(num);
}

// from_chars和string_view性能更高，不校验空格
vector<int32_t> splitToVecInt(const XmlMap &attrs, const string &key)
{
    vector<int32_t> vec;
    auto it = attrs.find(key);
    if (it == attrs.end()) {
        return vec;
    }

    std::string_view target = it->second;
    size_t start = 0;
    while (start < target.size()) {
        size_t end = target.find(',', start);
        if (end == std::string_view::npos) {
            end = target.size();
        }

        std::string_view token = target.substr(start, end - start);
        if (!token.empty()) {
            int32_t value;
            // 1. 先尝试解析为整数
            auto [ptr, ec] = std::from_chars(token.data(), token.data() + token.size(), value);
            if (ec == std::errc()) {
                vec.push_back(value);
            } else {
                // 整数解析失败，用 strtod 解析浮点数（需要临时 null-terminated 字符串），因为owari可能有浮点数，这个没用
                string tmp(token);
                char *endPtr = nullptr;
                double val_double = strtod(tmp.c_str(), &endPtr);
                if (endPtr == tmp.c_str()) {
                    throw runtime_error("Invalid number format: " + string(token));
                }
                auto val_int = static_cast<int32_t>(round(val_double));
                vec.push_back(val_int);
            }
        }
        start = end + 1;
    }
    return vec;
}

XmlMap getXmlAttrs(const xml::XMLElement *element)
{
    XmlMap attrs;
    if (!element) {
        return attrs;
    }

    for (auto attr = element->FirstAttribute();  attr != nullptr; attr = attr->Next()) {
        if (attrs.find(attr->Name()) != attrs.end()) {
            throw runtime_error("Exist attribute: "s + attr->Name());
        }
        attrs[attr->Name()] = attr->Value();
    }
    return attrs;
}

uint8_t get_u2_from_attrs(const XmlMap &attrs, const char *name)
{
    auto it = attrs.find(name);
    if (it == attrs.end()) {
        throw runtime_error("In get_u2_from_attrs: Unknown name: "s + name);
    }
    uint32_t u2_value = stoul(it->second);
    if (u2_value > 3) {
        throw runtime_error("u2 value out_of_range: " + it->second);
    }
    return u2_value;
}

uint8_t get_u8_from_attrs(const XmlMap &attrs, const char *name)
{
    auto it = attrs.find(name);
    if (it == attrs.end()) {
        throw runtime_error("In get_u8_from_attrs: Unknown name: "s + name);
    }
    uint32_t u8_value = stoul(it->second);
    if (u8_value > UINT8_MAX) {
        throw runtime_error("u8 value out_of_range: " + it->second);
    }
    return u8_value;
}

uint16_t get_u16_from_attrs(const XmlMap &attrs, const char *name)
{
    auto it = attrs.find(name);
    if (it == attrs.end()) {
        throw runtime_error("In get_u16_from_attrs: Unknown name: "s + name);
    }
    uint32_t u16_value = stoul(it->second);
    if (u16_value > UINT16_MAX) {
        throw runtime_error("u16 value out_of_range: " + it->second);
    }
    return u16_value;
}

double average(const vector<int64_t> &vec)
{
    if (vec.size() == 0) {
        return 0.;
    }
    int64_t num = 0;
    for (const auto &item: vec) {
        num += item;
    }
    return static_cast<double>(num) / static_cast<double>(vec.size());
}

double standard(const vector<int64_t> &vec)
{
    if (vec.size() <= 1) {
        return 0.;
    }
    double avg = average(vec);
    double num = 0;
    for (const auto &item: vec) {
        num += (static_cast<double>(item) - avg) * (static_cast<double>(item) - avg);
    }
    return sqrt(static_cast<double>(num) / static_cast<double>(vec.size() - 1));
}

string avg_std(const vector<int64_t> &vec)
{
    return to_string(average(vec)) + "#" + to_string(standard(vec));
}

}   // namespace Hasaki
