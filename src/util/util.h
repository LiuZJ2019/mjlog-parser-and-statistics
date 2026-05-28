/**
 * @file        util.h
 * @brief       通用模块
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     定义了一些常用的using，所以Hasaki命名空间下不需要using std的东西了
 *              XmlMap是xml解析的数据类型定义，Buffer是bytes解析的数据类型定义
 *              Buffer用RingBuffer的话性能实现会更好，但我懒得改了，先跑起来再说
 *              hskmjlog是我自定义的天凤xml牌谱压缩存储类型，经过实测，hskmjlog的空间占用为xml版本的20%，读入效率是xml的50-100倍
 *              通过load_hskmjlog+MjlogGameContainer::read_bytes可以将.hskmjlog读入为MjlogGameContainer
 *              通过MjlogGameContainer::write_bytes+save_hskmjlog可以将MjlogGameContainer写入为.hskmjlog
 */
#ifndef HASAKI_MJLOG_PARSER_UTIL_UTIL_H_
#define HASAKI_MJLOG_PARSER_UTIL_UTIL_H_

#include <map>
#include <deque>
#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>
#include <functional>
#include <filesystem>
#include "thirdparty/tinyxml2.h"


namespace Hasaki {

// well-known std library or thirdparty library namespace
using std::map;
using std::pair;
using std::array;
using std::deque;
using std::tuple;
using std::vector;
using std::string;
using std::function;
using std::unique_ptr;
using std::shared_ptr;
using std::to_string;
using std::make_unique;
using std::cout;
using std::cerr;
using std::endl;
using std::runtime_error;
using namespace std::string_literals;
namespace fs = std::filesystem;
namespace xml = tinyxml2;

// XmlMap定义了xml解析对，Buffer定义了bytes解析对，理论上RingBuffer的性能会更高
using XmlMap = map<string, string>;
using Buffer = deque<uint8_t>;

// 定义了读入/写入.hskmjlog的方法，与MjlogGameContainer::read_bytes/write_bytes配合使用可以实现牌谱的压缩保存/读取
// MjlogGameContainer采用了静态多态分配的方式避免了虚表的开销，读入性能很高，并且没有额外空间开销
// 对局主体的Action用联合体+模板压缩存储为2字节（可以看parser/mjlog_meta_type.h和parser/mjlog_action.h），所以很省空间
// 经过实测，hskmjlog的空间占用为xml版本的20%，读入效率是xml的50-100倍
Buffer load_hskmjlog(const fs::path &file_path);
void save_hskmjlog(const fs::path &file_path, const Buffer &buffer);

// xml转.hskmjlog用的函数，下面是一个例子
/**
 * @brief   下面是一个例子，在`F:/Desktop/rust/tenhou_logs/`下有若干文件夹，例如20250101/, 20250102/, ...
 *          每个文件夹下有若干xml文件，下面这段代码将整个文件夹下的xml文件打包为20250101.hskmjlog
 *          我的.hskmjlog文件就是这么打包出来的，亲测无误
 * @code
 * bool convert_xml_to_hskmjlog()
 * {
 *     auto subdir_vec = Hasaki::get_subdirectories("F:/Desktop/rust/tenhou_logs/");
 *     for (const auto &[subdir_path, dir_name]: subdir_vec) {
 *         auto name2path = Hasaki::getAllPath(subdir_path, ".xml");
 *         Hasaki::MjlogGameContainer container;
 *         for (auto &[filename, full_path]: name2path) {
 *             auto u8str = Hasaki::readAsUtf8String(full_path);
 *             container.read_one_xml(u8str);
 *         }
 *         // 这个if是用来测试输出的.hskmjlog和内存里的内容是否一致的（也可以用test_bytes_output，这个是测试bytes输入输出的一致性）
 *         // 这个test很费性能，debug用就行，如果需要性能，这段代码可以删去
 *         if (!container.test_xml_output()) {
 *             cout << "error!" << endl;
 *             return false;
 *         }
 *         Buffer q;
 *         container.write_bytes(q);
 *         Hasaki::save_hskmjlog("F:/Desktop/C++/46.mjlogParser/hskmjlog_data/" + dir_name + ".hskmjlog", q);
 *     }
 *     return true;
 * }
 * @endcode
 */
vector<pair<string, string> > get_subdirectories(const string &dir_path);

// 获取目录下所有后缀为suffix的文件，用于读入文件
/**
 * @brief   下面是一个例子，在`F:/Desktop/C++/46.mjlogParser/hskmjlog_data`下有若干.hskmjlog文件，我要读入为MjlogGameContainer
 * @code
 * auto name2path = Hasaki::getAllPath("F:/Desktop/C++/46.mjlogParser/hskmjlog_data", ".hskmjlog");
 * auto container = make_shared<Hasaki::MjlogGameContainer>();
 * for (auto &[filename, full_path]: name2path) {
 *     cout << filename << endl;
 *     deque<uint8_t> q_ = Hasaki::load_hskmjlog(full_path);
 *     container->read_bytes(q_);
 * }
 * @endcode
 */
map<string, fs::path> getAllPath(const string &rootDir, const string &suffix);

// 读入xml文件的代码，实际上跟utf-8没什么关系，懒得改了
string readAsUtf8String(const fs::path &filePath);

string to_string(const Buffer &arr);

template<typename T>
string to_string(const vector<T> &vec)
{
    if (vec.empty()) {
        return "";
    }
    string result = to_string(vec[0]);
    for (size_t i = 1; i < vec.size(); ++i) {
        result += "," + to_string(vec[i]);
    }
    return result;
}

template<typename T, size_t U>
string to_string(const array<T, U> &arr)
{
    string result = to_string(arr[0]);
    for (size_t i = 1; i < U; ++i) {
        result += "," + to_string(arr[i]);
    }
    return result;
}

// xml解析用的代码
vector<int32_t> splitToVecInt(const XmlMap &attrs, const string &key);
XmlMap getXmlAttrs(const tinyxml2::XMLElement *element);
uint8_t get_u2_from_attrs(const XmlMap &attrs, const char *name);
uint8_t get_u8_from_attrs(const XmlMap &attrs, const char *name);
uint16_t get_u16_from_attrs(const XmlMap &attrs, const char *name);

// 结果统计用的函数，用于测试命令行里输出结果
double average(const vector<int64_t> &vec);
double standard(const vector<int64_t> &vec);
string avg_std(const vector<int64_t> &vec);

// Buffer的I/O接口，不处理大小端，总是假设是小端设备
template<typename T>
T readFromBuf(Buffer &buf)
{
    union {
        T num;
        uint8_t bytes[sizeof(T)];
    } num;

    for (uint32_t i = 0; i < sizeof(T); ++i) {
        num.bytes[i] = buf.front();
        buf.pop_front();
    }
    return num.num;
}

template<typename T>
void writeToBuf(Buffer &buf, T num_in)
{
    union {
        T num;
        uint8_t bytes[sizeof(T)];
    } num{.num=num_in};

    for (uint32_t i = 0; i < sizeof(T); ++i) {
        buf.push_back(num.bytes[i]);
    }
}

template<typename T>
void overwriteToBuf(Buffer &buf, T num_in, uint32_t pos)
{
    union {
        T num;
        uint8_t bytes[sizeof(T)];
    } num{.num=num_in};

    for (uint32_t i = 0; i < sizeof(T); ++i) {
        buf[pos + i] = num.bytes[i];
    }
}

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_UTIL_UTIL_H_
