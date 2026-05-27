#include <iostream>
#include <chrono>
#include "parser/mjlog_container.h"
#include "stats/mjlog_filter.h"

using namespace std;
namespace xml = tinyxml2;
using namespace Hasaki;


int main()
{
    auto name2path = Hasaki::getAllPath("F:/Desktop/C++/46.mjlogParser/hskmjlog_data", ".hskmjlog");
    auto container = make_shared<Hasaki::MjlogGameContainer>();
    for (auto &[filename, full_path]: name2path) {
        cout << filename << endl;
        deque<uint8_t> q_ = Hasaki::load_hskmjlog(full_path);
        container->read_bytes(q_);
    }
    cout << container->m_games.size() << endl;
//    if (!container->test_bytes_output()) {
//        cout << "error!" << endl;
//        return -1;
//    }


    Hasaki::GameContainerStatus status(container);
    Hasaki::MjlogFuncFactory filterFactory;

    auto start_time = std::chrono::steady_clock::now();
    cout << "all round: " << status.get_round_num() << endl;

    auto test_richi_num_if_richi_ok = filterFactory.statistics(Level::PLAYER, status, vector<StatsTuple>{
            {Level::PLAYER, DynFuncName::BOOL_PLAYER_IS_RICHI_OK, 3, SrcIdxList{}},
            {Level::PLAYER, DynFuncName::BOOL_GAME_NOT, 2, SrcIdxList{3u}},
            {Level::PLAYER, DynFuncName::I64_PLAYER_GET_RICHI_ROUND, 2, SrcIdxList{}},
    });
    cout << "test_richi_num_if_richi_ok: " << test_richi_num_if_richi_ok.size() << endl;
    cout << "test_richi_num_if_richi_ok: " << average(test_richi_num_if_richi_ok) << endl;

    auto test_total_gain_if_richi_ok = filterFactory.statistics(Level::PLAYER, status, vector<StatsTuple>{
            {Level::PLAYER, DynFuncName::BOOL_PLAYER_IS_RICHI_OK, 3, SrcIdxList{}},
            {Level::PLAYER, DynFuncName::BOOL_GAME_NOT, 2, SrcIdxList{3u}},
            {Level::PLAYER, DynFuncName::I64_PLAYER_GET_TOTAL_GAIN_POINT, 2, SrcIdxList{}},
    });
    cout << "test_total_gain_if_richi_ok: " << test_total_gain_if_richi_ok.size() << endl;
    cout << "test_total_gain_if_richi_ok: " << avg_std(test_total_gain_if_richi_ok) << endl;

    auto test_total_gain_if_richi_and_agari = filterFactory.statistics(Level::PLAYER, status, vector<StatsTuple>{
            {Level::PLAYER, DynFuncName::BOOL_PLAYER_IS_RICHI_OK, 3, SrcIdxList{}},
            {Level::PLAYER, DynFuncName::BOOL_GAME_NOT, 2, SrcIdxList{3u}},
            {Level::PLAYER, DynFuncName::BOOL_PLAYER_IS_TSUMO, 3, SrcIdxList{}},
            {Level::PLAYER, DynFuncName::BOOL_PLAYER_IS_RON, 4, SrcIdxList{}},
            {Level::PLAYER, DynFuncName::BOOL_GAME_NOR, 2, SrcIdxList{3u, 4u}},
            {Level::PLAYER, DynFuncName::I64_PLAYER_GET_TOTAL_GAIN_POINT, 2, SrcIdxList{}},
    });
    cout << "test_total_gain_if_richi_and_agari: " << test_total_gain_if_richi_and_agari.size() << endl;
    cout << "test_total_gain_if_richi_and_agari: " << avg_std(test_total_gain_if_richi_and_agari) << endl;

    auto test_total_gain_if_double_richi_ok = filterFactory.statistics(Level::PLAYER, status, vector<StatsTuple>{
            {Level::PLAYER, DynFuncName::BOOL_PLAYER_IS_DOUBLE_RICHI_OK, 3, SrcIdxList{}},
            {Level::PLAYER, DynFuncName::BOOL_GAME_NOT, 2, SrcIdxList{3u}},
            {Level::PLAYER, DynFuncName::I64_PLAYER_GET_TOTAL_GAIN_POINT, 2, SrcIdxList{}},
    });
    cout << "test_total_gain_if_double_richi_ok: " << test_total_gain_if_double_richi_ok.size() << endl;
    cout << "test_total_gain_if_double_richi_ok: " << avg_std(test_total_gain_if_double_richi_ok) << endl;

    auto test_total_gain_if_double_richi_ok_and_agari = filterFactory.statistics(Level::PLAYER, status, vector<StatsTuple>{
            {Level::PLAYER, DynFuncName::BOOL_PLAYER_IS_DOUBLE_RICHI_OK, 3, SrcIdxList{}},
            {Level::PLAYER, DynFuncName::BOOL_GAME_NOT, 2, SrcIdxList{3u}},
            {Level::PLAYER, DynFuncName::BOOL_PLAYER_IS_TSUMO, 3, SrcIdxList{}},
            {Level::PLAYER, DynFuncName::BOOL_PLAYER_IS_RON, 4, SrcIdxList{}},
            {Level::PLAYER, DynFuncName::BOOL_GAME_NOR, 2, SrcIdxList{3u, 4u}},
            {Level::PLAYER, DynFuncName::I64_PLAYER_GET_TOTAL_GAIN_POINT, 2, SrcIdxList{}},
    });
    cout << "test_total_gain_if_double_richi_ok_and_agari: " << test_total_gain_if_double_richi_ok_and_agari.size() << endl;
    cout << "test_total_gain_if_double_richi_ok_and_agari: " << avg_std(test_total_gain_if_double_richi_ok_and_agari) << endl;

//    try {
//        // ...
//    } catch (std::runtime_error &e) {
//        cout << "already catch runtime error: " << e.what() << endl;
//    } catch (std::exception &e) {
//        cout << "already catch unknown error: " << e.what() << endl;
//    }

    auto end_time = std::chrono::steady_clock::now();
    auto diff_time = end_time - start_time;

    std::cout << "Elapsed time: "
              << std::chrono::duration<double, std::milli>(diff_time).count()
              << " ms" << std::endl;

    return 0;
}
