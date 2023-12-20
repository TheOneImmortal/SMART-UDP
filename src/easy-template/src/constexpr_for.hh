/**
 * ****************constexpr_for.hh****************
 * @brief  编译器for展开
 * @author https://github.com/TheOneImmortal
 * @date   2023-12-10
 * ********************************
 */

#ifndef __XN_ET__CONSTEXPR_FOR_HH__
#define __XN_ET__CONSTEXPR_FOR_HH__

/*----------------include----------------*/
#include <algorithm>
#include <bits/utility.h>
#include <type_traits>

#include "../../base-type/include_me.hh"

/*----------------body----------------*/
namespace xn {

template <u64... Is> constexpr void loop(std::index_sequence<Is...>, auto &&f) {
  (f.template operator()<Is>(), ...);
}

template <u64 N> constexpr void loop(auto &&f) {
  loop(std::make_index_sequence<N>{}, std::forward<decltype(f)>(f));
}

template <typename T, typename F, u64... Is>
constexpr void loop(T, std::index_sequence<Is...>, F &&f) {
  // if constexpr (requires {
  //                 F().template operator()<u64(0), std::tuple_element_t<0,
  //                 T>>();
  //               })
  (f.template operator()<Is, std::tuple_element_t<Is, T>>(), ...);
  // else
  //   (f.template operator()<std::tuple_element_t<Is, T>>(), ...);
}

template <typename T>
  requires(std::tuple_size<T>::value > 0)
constexpr void loop(auto &&f) {
  loop(T(), std::make_index_sequence<std::tuple_size_v<T>>{},
       std::forward<decltype(f)>(f));
}

template <typename T, typename... F, u64... Is>
constexpr void loop(T, std::index_sequence<Is...>, F &&...f) {
  (f.template operator()<Is, std::tuple_element_t<Is, T>>(), ...);
}

template <typename T>
  requires(std::tuple_size<T>::value > 0)
constexpr void loop(auto &&...f) {
  loop(T(), std::make_index_sequence<std::tuple_size_v<T>>{},
       std::forward<decltype(f)>(f)...);
}

} // namespace xn

#endif