/**
 * ****************tuple_helper.hh****************
 * @brief  元组功能扩展
 * @author https://github.com/TheOneImmortal
 * @date   2023-12-14
 * ********************************
 */

#ifndef __XN_ET__TUPLE_HELPER_HH__
#define __XN_ET__TUPLE_HELPER_HH__

/*----------------include----------------*/
#include <algorithm>
#include <tuple>
#include <type_traits>

#include "../../base-type/include_me.hh"

/*----------------body----------------*/
namespace xn {

template <class... Ts>
struct combine_tuples
    : std::type_identity<typename std::__combine_tuples<Ts...>::__type> {};
template <class... Ts> using combine_tuples_t = combine_tuples<Ts...>::type;

template <u64 I, class... Ts> class tuple_front_i_size;
template <u64 I, class... Ts>
  requires(I == 0)
class tuple_front_i_size<I, Ts...> : public std::integral_constant<u64, 0> {};
template <u64 I, class T, class... Ts>
  requires(I == 1)
class tuple_front_i_size<I, T, Ts...>
    : public std::integral_constant<u64, sizeof(T)> {};
template <u64 I, class T, class... Ts>
  requires(I > 1)
class tuple_front_i_size<I, T, Ts...>
    : public std::integral_constant<
          u64, sizeof(T) + tuple_front_i_size<I - 1, Ts...>::value> {};
template <u64 I, class... Ts>
class tuple_front_i_size<I, std::tuple<Ts...>>
    : public tuple_front_i_size<I, Ts...> {};

template <u64 ID, class T, class T2, class... Ts>
class tuple_match_i_helper : public tuple_match_i_helper<ID + 1, T, Ts...> {};
template <u64 ID, class T, class T2, class... Ts>
  requires(std::is_same_v<T, T2>)
class tuple_match_i_helper<ID, T, T2, Ts...>
    : public std::integral_constant<u64, ID> {};
template <class T, class... Ts>
class tuple_match_i : public tuple_match_i_helper<0, T, Ts...> {};
template <class T, class... Ts>
class tuple_match_i<T, std::tuple<Ts...>>
    : public tuple_match_i_helper<0, T, Ts...> {};

} // namespace xn

#endif