/**
 * @file        function_traits.hpp
 * @brief       从std::function提取出模板参数
 * @copyright   Copyright (c) 2026, Hasaki. All rights reserved.
 * @license     MIT License (see LICENSE file for details)
 * @author      间宫羽咲sama (Hasaki)
 * @date        2026-05-05
 * @version     1.0.0
 * @details     stats/mjlog_filter.h用，根据std::function的输入输出参数类型调用对应的assign模板
 */
#ifndef HASAKI_MJLOG_PARSER_TEMPLATE_FUNCTION_TRAITS_HPP_
#define HASAKI_MJLOG_PARSER_TEMPLATE_FUNCTION_TRAITS_HPP_

#include <tuple>
#include <type_traits>
#include <functional>

namespace Hasaki {

// 基础模板
template<typename T>
struct function_traits;

// 针对 std::function<R(Args...)> 的特化
template<typename R, typename... Args>
struct function_traits<std::function<R(Args...)>>
{
    using return_type = R;
    static constexpr size_t arity = sizeof...(Args);

    template<size_t I>
    using arg_type = std::tuple_element_t<I, std::tuple<Args...>>;
};

}   // namespace Hasaki

#endif //HASAKI_MJLOG_PARSER_TEMPLATE_FUNCTION_TRAITS_HPP_
