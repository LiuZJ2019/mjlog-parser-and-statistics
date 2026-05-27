/**
 * @file        tuple_to_unique_variant.hpp
 * @brief       从std::function提取出模板参数
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     从tuple构造出去重类型的variant
 *              代码是AI(deepseek)写的，我没有review，看到功能没问题就直接用了
 */
#ifndef HASAKI_MJLOG_PARSER_TEMPLATE_TUPLE_TO_UNIQUE_VARIANT_HPP_
#define HASAKI_MJLOG_PARSER_TEMPLATE_TUPLE_TO_UNIQUE_VARIANT_HPP_

#include <tuple>
#include <variant>
#include <type_traits>

namespace Hasaki {

// 辅助模板：检查类型T是否在Tuple中
template <typename T, typename Tuple>
struct is_in_tuple;

template <typename T, typename... Ts>
struct is_in_tuple<T, std::tuple<Ts...>>
        : std::disjunction<std::is_same<T, Ts>...> {};

// 辅助模板：如果不存在，将类型T添加到Result
template <typename Result, typename T>
struct add_if_not_present;

template <typename... Unique, typename T>
struct add_if_not_present<std::tuple<Unique...>, T> {
    using type = std::conditional_t<
            is_in_tuple<T, std::tuple<Unique...>>::value,
            std::tuple<Unique...>,          // 已存在，不变
            std::tuple<Unique..., T>        // 不存在，追加
    >;
};

// 对 tuple 去重
template <typename Tuple>
struct unique_tuple;

template <typename... Ts>
struct unique_tuple<std::tuple<Ts...>> {
private:
    // 递归
    template <typename Acc, typename... Rest>
    struct impl;

    template <typename... Acc>
    struct impl<std::tuple<Acc...>> {
        using type = std::tuple<Acc...>;
    };

    template <typename... Acc, typename T, typename... Rest>
    struct impl<std::tuple<Acc...>, T, Rest...> {
        using current = typename add_if_not_present<std::tuple<Acc...>, T>::type;
        using type = typename impl<current, Rest...>::type;
    };

public:
    using type = typename impl<std::tuple<>, Ts...>::type;
};

// 从tuple生成variant的原始转换器
template <typename Tuple>
struct TupleToVariant_;

template <typename... Ts>
struct TupleToVariant_<std::tuple<Ts...>> {
    using type = std::variant<Ts...>;
};

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_TEMPLATE_TUPLE_TO_UNIQUE_VARIANT_HPP_
