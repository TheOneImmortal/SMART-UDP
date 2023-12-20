/**
 * ****************protocol.hh****************
 * @brief  [ID-长度-内容]封装协议
 * @author https://github.com/TheOneImmortal
 * @date   2023-10-27
 * ********************************
 */

#ifndef __XN_SU__PROTOCOL_HH__
#define __XN_SU__PROTOCOL_HH__

/*----------------include----------------*/
#include <algorithm>
#include <array>
#include <bits/utility.h>
#include <compare>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <tuple>
#include <type_traits>

#include "../../base-type/include_me.hh"
#include "../../byte-buffer/include_me.hh"
#include "../../easy-template/include_me.hh"

/*----------------body----------------*/
namespace xn {

namespace idl_protocol {

// 因作者能力限制，ID类型请使用unsigned系列，并且为保证您的程序正常运行在个平台，请勿使用long。
using ID = u16;

/**
 * @brief 协议的每一项
 *
 * @tparam Ts 不应当含有 const、volatile、&、*
 */
template <class... Ts>
  requires((is_no_cvrp<Ts> /*&& std::is_gt(sizeof(Ts) <=> 0)*/) && ...)
struct Item {
  static constexpr idl_protocol::ID count =
      std::tuple_size<std::tuple<Ts...>>::value;

  using tuple = std::tuple<Ts...>;
  template <u64 i> using type = std::tuple_element_t<i, tuple>;
  static constexpr u64 size = (sizeof(Ts) + ...);
};

namespace helper {

template <ID ID, class... Ts> struct Protocol;

template <ID ID, class T> struct Protocol<ID, T> {
  struct GetMaxId {
    static inline constexpr idl_protocol::ID value = ID - 1;
  };

  struct GetTuple : std::type_identity<std::tuple<>> {};
};

template <ID ID, class T, class... Is>
struct Protocol<ID, T, Is...> : public Protocol<ID, Is...> {
  using GetMaxId = Protocol<ID, Is...>::GetMaxId;

  template <idl_protocol::ID id>
  using GetItem = Protocol<ID, Is...>::template GetItem<id>;

  using GetTuple = Protocol<ID, Is...>::GetTuple;
};

template <ID ID, class... Ts> struct Protocol<ID, Item<Ts...>> {
  struct GetMaxId {
    static inline constexpr idl_protocol::ID value = ID;
  };

  template <idl_protocol::ID id>
    requires(ID == id)
  struct GetItem : std::type_identity<Item<Ts...>> {};

  struct GetTuple : std::type_identity<std::tuple<Item<Ts...>>> {};

  // template <class P> struct GetHandlerList {
  //   static inline const auto value = []() {
  //     std::array<std::function<void(BufferOnHad)>, GetMaxId::value - ID + 1>
  //         ret;
  //     ret[0] = [](BufferOnHad buffer) {
  //       if (buffer.size < (sizeof(Ts) + ...))
  //         return;

  //       ProtocolMessageHandler<P, Ts...>::received([&]() {
  //         Ts v;

  //         std::make_integer_sequence<int, 5> memcpy(
  //             &v,
  //             buffer.From(tuple_front_i_size<tuple_match_i<Ts, Ts...>::value,
  //                                            Ts...>::value),
  //             sizeof(Ts));
  //         return v;
  //       }()...);
  //     };
  //     return ret;
  //   }();
  // };
};

template <ID ID, class... Ts, class... Is>
struct Protocol<ID, Item<Ts...>, Is...> : public Protocol<ID + 1, Is...> {
  struct GetMaxId : Protocol<ID + 1, Is...>::GetMaxId {};

  template <idl_protocol::ID id>
  struct GetItem : Protocol<ID + 1, Is...>::template GetItem<id> {};
  template <idl_protocol::ID id>
    requires(ID == id)
  struct GetItem<id> : std::type_identity<Item<Ts...>> {};

  struct GetTuple : std::type_identity<combine_tuples_t<
                        std::tuple<Item<Ts...>>,
                        typename Protocol<ID + 1, Is...>::GetTuple::type>> {};

  // template <class P> struct GetHandlerList {
  //   static inline const auto value = []() {
  //     std::array<std::function<void(BufferOnHad)>, GetMaxId::value - ID + 1>
  //         ret;
  //     auto last = Protocol<ID + 1, Is...>::template GetHandlerList<P>::value;
  //     for (u64 i = 0; i < last.size(); i++)
  //       ret[i] = last[i];
  //     ret[last.size()] = [](BufferOnHad buffer) {
  //       if (buffer.size < (sizeof(Ts) + ...))
  //         return;

  //       ProtocolMessageHandler<P, Ts...>::received([&]() {
  //         Ts v;

  //         std::make_integer_sequence<int, 5> memcpy(
  //             &v,
  //             buffer.From(tuple_front_i_size<tuple_match_i<Ts, Ts...>::value,
  //                                            Ts...>::value),
  //             sizeof(Ts));
  //         return v;
  //       }()...);
  //     };
  //     return ret;
  //   }();
  // };
};

} // namespace helper

/**
 * @brief 协议
 *
 * @tparam Ts
 * 应当由Item包装，无包装的类型会被忽略（不会报错，而是会跳过，想试试的人可以试试，很厉害，不会影响其他部分编译）
 */
template <class... Ts> struct Protocol {
  static constexpr idl_protocol::ID count =
      helper::Protocol<0, Ts...>::GetMaxId::value + 1;

  using tuple = helper::Protocol<0, Ts...>::GetTuple::type;

  template <idl_protocol::ID id>
    requires(id <= count)
  using item = helper::Protocol<0, Ts...>::template GetItem<id>::type;

  template <idl_protocol::ID id>
    requires(id <= count)
  static inline constexpr u64 size = sizeof(item<id>);
};

} // namespace idl_protocol

} // namespace xn

#endif