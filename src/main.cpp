#include <iostream>
#include <chrono>
#include <fstream>
#include <filesystem>   // 新增：用于路径校验与拼接
#include "parser/mjlog_stats_algorithm.h"
#include "util/json_util.h"

using namespace std;
using namespace chrono;
using namespace Hasaki;
namespace fs = std::filesystem;  // 别名，方便使用


MjlogGameContainer load_main_data(const string &path, const string &suffix)
{
    auto load_start = steady_clock::now();

    // 修正：使用传入的 path 和 suffix，不再硬编码路径
    auto name2path = Hasaki::getAllPath(path, suffix);
    auto container = MjlogGameContainer();
    for (auto &[filename, full_path] : name2path) {
        deque<uint8_t> q_ = Hasaki::load_hskmjlog(full_path);
        container.read_bytes(q_);
    }

    cout << "Data loaded success. Games: " << container.m_games.size() << endl;
    auto load_end = steady_clock::now();
    auto load_us = duration_cast<microseconds>(load_end - load_start).count();
    cout << "Data load time: " << (load_us / 1'000'000.0) << "s" << endl;
    return container;
}


bool convert_xml_to_hskmjlog(const string &input_dir, const string &output_dir, bool is_check)
{
    auto convert_start = steady_clock::now();

    size_t game_cnt = 0;
    auto subdir_vec = Hasaki::get_subdirectories(input_dir);
    for (const auto &[subdir_path, dir_name]: subdir_vec) {
        auto name2path = Hasaki::getAllPath(subdir_path, ".xml");
        Hasaki::MjlogGameContainer container;
        for (auto &[filename, full_path]: name2path) {
            auto u8str = Hasaki::readAsUtf8String(full_path);
            container.read_one_xml(u8str);
        }
        // 这个if是用来测试输出的.hskmjlog和内存里的内容是否一致的（也可以用test_bytes_output，这个是测试bytes输入输出的一致性）
        // 这个test很费性能，debug用就行，如果需要性能，这段代码可以删去
        if (is_check) {
            if (!container.test_bytes_output() || !container.test_xml_output()) {
                cout << "error!" << endl;
                return false;
            }
        }
        Buffer q;
        container.write_bytes(q);
        Hasaki::save_hskmjlog(output_dir + dir_name + ".hskmjlog", q);
        game_cnt += container.m_games.size();
    }

    cout << "convert_xml_to_hskmjlog success. Game: " << game_cnt << endl;
    auto convert_end = steady_clock::now();
    auto convert_us = duration_cast<microseconds>(convert_end - convert_start).count();
    cout << "convert_xml_to_hskmjlog time: " << (convert_us / 1'000'000.0) << "s" << endl;

    return true;
}


void do_stats_algorithm(const MjlogGameContainer &container, const string &path)
{
    auto stats_start = steady_clock::now();
    JsonContainer jsonContainer;
    {
        auto temp_data = stats_richi_ok_num(container);
        jsonContainer.add_data("stats_richi_ok_num", JsonOutputDataType::NUM_OCCUR_TIME, temp_data);
    }
    {
        auto temp_data = stats_first_richi_ok_num(container);
        jsonContainer.add_data("stats_first_richi_ok_num", JsonOutputDataType::NUM_OCCUR_TIME, temp_data);
    }
    {
        auto temp_data = stats_chasing_richi_ok_num(container);
        jsonContainer.add_data("stats_chasing_richi_ok_num", JsonOutputDataType::NUM_OCCUR_TIME, temp_data);
    }
    {
        auto temp_data = stats_be_chased_richi_ok_num(container);
        jsonContainer.add_data("stats_be_chased_richi_ok_num", JsonOutputDataType::NUM_OCCUR_TIME, temp_data);
    }
    {
        auto temp_data = stats_richi_n_gain(container);
        jsonContainer.add_data("stats_richi_n_gain", JsonOutputDataType::NUM_TO_MEAN_STD_SAMPLE, temp_data);
    }
    {
        auto temp_data = stats_richi_n_ron_rate(container);
        jsonContainer.add_data("stats_richi_n_ron_rate", JsonOutputDataType::NUM_TO_MEAN_STD_SAMPLE, temp_data);
    }
    {
        auto temp_data = stats_richi_n_tsumo_rate(container);
        jsonContainer.add_data("stats_richi_n_tsumo_rate", JsonOutputDataType::NUM_TO_MEAN_STD_SAMPLE, temp_data);
    }
    {
        auto temp_data = stats_richi_n_be_ron_rate(container);
        jsonContainer.add_data("stats_richi_n_be_ron_rate", JsonOutputDataType::NUM_TO_MEAN_STD_SAMPLE, temp_data);
    }
    {
        auto temp_data = stats_richi_n_be_tsumo_rate(container);
        jsonContainer.add_data("stats_richi_n_be_tsumo_rate", JsonOutputDataType::NUM_TO_MEAN_STD_SAMPLE, temp_data);
    }
    {
        auto temp_data = stats_richi_n_draw_rate(container);
        jsonContainer.add_data("stats_richi_n_draw_rate", JsonOutputDataType::NUM_TO_MEAN_STD_SAMPLE, temp_data);
    }
    {
        auto temp_data = stats_richi_n_ryuukyoku_rate(container);
        jsonContainer.add_data("stats_richi_n_ryuukyoku_rate", JsonOutputDataType::NUM_TO_MEAN_STD_SAMPLE, temp_data);
    }

    cout << "do_stats_algorithm success. Algorithm: " << jsonContainer.all_data.size() << endl;
    auto stats_end = steady_clock::now();
    auto stats_us = duration_cast<microseconds>(stats_end - stats_start).count();
    cout << "do_stats_algorithm time: " << (stats_us / 1'000'000.0) << "s" << endl;

    ofstream ofs(path);
    if (ofs.is_open()) {
        ofs << jsonContainer.to_json_string() << endl;
        ofs.close();
    }
}


int main(int argc, char* argv[])
{
    // ---------- 命令行参数解析 ----------
    if (argc < 2) {
        cerr << "Usage:\n"
             << "  " << argv[0] << " preprocess <input_dir> <output_dir> [check=true|false]\n"
             << "  " << argv[0] << " stats <input_dir> <output_json> [suffix=hskmjlog|xml]\n";
        return -1;
    }

    string mode = argv[1];

    if (mode == "preprocess") {
        if (argc < 4) {
            cerr << "Error: preprocess mode requires input_dir and output_dir.\n";
            return -1;
        }
        string input_dir = argv[2];
        string output_dir = argv[3];
        bool is_check = false;   // 默认值
        if (argc >= 5) {
            string check_str = argv[4];
            if (check_str == "true" || check_str == "1") {
                is_check = true;
            } else if (check_str == "false" || check_str == "0") {
                is_check = false;
            } else {
                cerr << "Error: check parameter must be true or false.\n";
                return -1;
            }
        }

        // 校验输入目录合法性
        fs::path input_path(input_dir);
        if (!fs::exists(input_path) || !fs::is_directory(input_path)) {
            cerr << "Error: input directory does not exist or is not a directory: " << input_dir << endl;
            return -1;
        }

        if (!convert_xml_to_hskmjlog(input_dir, output_dir, is_check)) {
            cerr << "Error occurs in convert_xml_to_hskmjlog." << endl;
            return -1;
        }
    }
    else if (mode == "stats") {
        if (argc < 4) {
            cerr << "Error: stats mode requires input_dir and output_json.\n";
            return -1;
        }
        string input_dir = argv[2];
        string output_json = argv[3];
        string suffix = "hskmjlog";  // 默认值
        if (argc >= 5) {
            suffix = argv[4];
        }

        if (!(suffix == "hskmjlog" || suffix == "xml")) {
            cerr << "Error suffix: " << suffix << " in mode stats" << endl;
            return -1;
        }

        // 校验输入目录合法性
        fs::path input_path(input_dir);
        if (!fs::exists(input_path) || !fs::is_directory(input_path)) {
            cerr << "Error: input directory does not exist or is not a directory: " << input_dir << endl;
            return -1;
        }

        auto container = load_main_data(input_dir, "." + suffix);
        do_stats_algorithm(container, output_json);
    }
    else {
        cerr << "Unknown mode: " << mode << endl;
        return -1;
    }

    return 0;
}