/**
 * ****************protocol_helper.hh****************
 * @brief  idl协议适配辅助
 * @author https://github.com/TheOneImmortal
 * @date   2023-12-19
 * ********************************
 */

#ifndef __XN_SU__PROTOCOL_HELPER_HH__
#define __XN_SU__PROTOCOL_HELPER_HH__

/*----------------include----------------*/
#include <algorithm>
#include <winsock2.h>

#include "../../byte-buffer/include_me.hh"
#include "../../easy-template/include_me.hh"
#include "../../id-length-protocol/include_me.hh"

/*----------------body----------------*/
namespace xn {

namespace idl_protocol {

#pragma region exports

template <class P> class Handler;

template <class Handler> void parser(BufferOnHad data);

#pragma endregion

#pragma region realization

namespace helper {
template <class H, class Tpl> class call_helper_with_addr;
template <class H, class... Ts>
class call_helper_with_addr<H, std::tuple<Ts...>> {
public:
  template <u64... Is>
  void operator()(sockaddr_in addr, BufferOnHad data,
                  std::index_sequence<Is...>) const noexcept {
    H::recived(addr, [&data]() -> Ts {
      return from_bytes<Ts>(
          data.From(tuple_front_i_size<Is, Ts...>::value, sizeof(Ts)));
    }()...);
  };
  void operator()(sockaddr_in addr, BufferOnHad data) const noexcept {
    operator()(addr, data, std::make_index_sequence<sizeof...(Ts)>());
  };
};

template <class H> class ParserWithSockaddr;
template <template <class P> class H, class P> class ParserWithSockaddr<H<P>> {
public:
  ParserWithSockaddr() noexcept {
    loop<typename P::tuple>([this]<u64 i, typename T>() {
      parsers[i] = [](sockaddr_in addr, BufferOnHad data) {
        if (data.size > P::template size<i>) {
          call_helper_with_addr<H<P>, typename P::template item<i>::tuple>()(
              addr, data);
        }
      };
    });
  }

  void operator()(sockaddr_in addr, BufferOnHad data) const {
    while (1) {
      auto broken_opt = get_break(data);
      if (!broken_opt.has_value())
        break;

      auto broken = broken_opt.value();

      if (broken.id < parsers.size())
        parsers[broken.id](addr, broken.content);
      else
        std::cout << "unknown id:" << broken.id << std::endl;

      data = std::move(broken.next);
    }
  };

private:
  std::array<std::function<void(sockaddr_in, BufferOnHad)>, P::count> parsers;
};

} // namespace helper

template <class Handler>
inline void parser(sockaddr_in addr, BufferOnHad data) {
  static helper::ParserWithSockaddr<Handler> parser;

  parser(addr, data);
}

} // namespace idl_protocol

} // namespace xn

#endif