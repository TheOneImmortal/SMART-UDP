/**
 * ****************adapt_format.hh****************
 * @brief  适配std::format
 * @author https://github.com/TheOneImmortal
 * @date   2023-12-20
 * ********************************
 */

#ifndef __XN_BB__ADAPT_FORMAT_HH__
#define __XN_BB__ADAPT_FORMAT_HH__

/*----------------include----------------*/
#include <algorithm>
#include <format>

#include "buffer.hh"

/*----------------body----------------*/
template <> struct std::formatter<xn::BufferOnHad, char> {
  FORMATTER_NO_PARSE

  auto format(const xn::BufferOnHad &v,
              std::format_context &fc) const noexcept {
    std::format_to(fc.out(), "[");
    for (int i = 0; i < v.size - 1; i++) {
      std::format_to(fc.out(), "{:02x},", v.data[i]);
    }
    return std::format_to(fc.out(), "{:02x}]", v.data[v.size - 1]);
  }
};

#endif