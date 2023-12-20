#include "message_handler.hh"

using namespace xn;

namespace xn::idl_protocol::helper {

std::optional<breaked> get_break(BufferOnHad data) {
  if ((sizeof(ID) + sizeof(u16)) > data.size)
    return std::nullopt;

  auto id = from_bytes<ID>(data.From(0, sizeof(ID)));
  auto length = from_bytes<u16>(data.From(sizeof(ID), sizeof(u16)));
  auto content = data.From(sizeof(ID) + sizeof(u16), length);
  if ((sizeof(ID) + sizeof(u16) + length) > data.size)
    return std::nullopt;
  auto next = data.From(sizeof(ID) + sizeof(u16) + length);

  return breaked{
      .id = id,
      .length = length,
      .content = content,
      .next = next,
  };
}

} // namespace xn::idl_protocol::helper