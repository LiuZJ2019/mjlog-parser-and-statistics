#include <gtest/gtest.h>
#include <chrono>
#include "parser/mjlog_tenpai.h"

using namespace std;
using namespace chrono;
using namespace Hasaki;


NumHai generate_num_hai(string s)
{
    NumHai ans{};
    for (auto c: s) {
        uint32_t idx = (c - '1');
        if (idx > 8) {
            throw runtime_error("generate_num_hai error1");
        }
        ++ ans[idx];
    }
    return ans;
}


NumHai generate_num_hai(array<uint8_t, 9> s)
{
    NumHai ans{};
    for (auto idx: s) {
        if (idx == 255) {
            break;
        }
        if (idx > 8) {
            throw runtime_error("generate_num_hai error2");
        }
        ++ ans[idx];
    }
    return ans;
}


void assert_3n_a1_hai(const string &hai_str, const string &ten_pai_str)
{
    auto arr = generate_num_hai(hai_str);
    auto ten_pai_result = TenPaiCheck::get_all_ten_3n_a1(arr.data(), 0);
    ASSERT_EQ(generate_num_hai(ten_pai_result), generate_num_hai(ten_pai_str));
}


TEST(TenPaiTest, MULTIPLE_AGARI_YAKUMAN_case) {
    auto preprocess_start = steady_clock::now();

    TenPaiCheck::preprocess();
    assert_3n_a1_hai("3334567888999", "23456789");
    assert_3n_a1_hai("3334556677888", "23456789");
    assert_3n_a1_hai("3334455667888", "23456789");
    assert_3n_a1_hai("2344445678999", "12356789");
    assert_3n_a1_hai("2333344567888", "12456789");
    assert_3n_a1_hai("2223456777999", "12345678");
    assert_3n_a1_hai("2223456777888", "12345678");
    assert_3n_a1_hai("2223456777789", "12345689");
    assert_3n_a1_hai("2223456677778", "12345689");
    assert_3n_a1_hai("2223445566777", "12345678");
    assert_3n_a1_hai("2223344556777", "12345678");
    assert_3n_a1_hai("2223334567888", "23456789");
    assert_3n_a1_hai("1233334567888", "12456789");
    assert_3n_a1_hai("1113334567888", "23456789");
    assert_3n_a1_hai("1112345666678", "12345789");
    assert_3n_a1_hai("1112223456777", "12345678");
    assert_3n_a1_hai("1112345678999", "123456789");

    auto preprocess_end = steady_clock::now();
    auto preprocess_us = duration_cast<microseconds>(preprocess_end - preprocess_start).count();
    cout << "Preprocess time: " << (preprocess_us / 1'000'000.0) << "s" << endl;
}
