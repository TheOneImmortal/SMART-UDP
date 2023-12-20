#ifndef __XN_BT__BASETYPEOP_HH__
#define __XN_BT__BASETYPEOP_HH__

#include <format>
#include <iostream>
#include <ranges>

#include "base-type-def.hh"

#pragma region formatter
#define FORMATTER_NO_PARSE                                                     \
  constexpr auto parse(const std::format_parse_context &pc) const noexcept {   \
    auto end = std::ranges::find(pc, '}');                                     \
    return end;                                                                \
  }

template <> struct std::formatter<v2i8, char> {
  FORMATTER_NO_PARSE

  auto format(const v2i8 &v, std::format_context &fc) const noexcept {
    return std::format_to(fc.out(), "{}:{}", v.x, v.y);
  }
};

template <> struct std::formatter<v2i16, char> {
  FORMATTER_NO_PARSE

  auto format(const v2i16 &v, std::format_context &fc) const noexcept {
    return std::format_to(fc.out(), "{}:{}", v.x, v.y);
  }
};

template <> struct std::formatter<v2i32, char> {
  FORMATTER_NO_PARSE

  auto format(const v2i32 &v, std::format_context &fc) const noexcept {
    return std::format_to(fc.out(), "{}:{}", v.x, v.y);
  }
};

template <> struct std::formatter<v2i64, char> {
  FORMATTER_NO_PARSE

  auto format(const v2i64 &v, std::format_context &fc) const noexcept {
    return std::format_to(fc.out(), "{}:{}", v.x, v.y);
  }
};

template <> struct std::formatter<v2r32, char> {
  FORMATTER_NO_PARSE

  auto format(const v2r32 &v, std::format_context &fc) const noexcept {
    return std::format_to(fc.out(), "{}:{}", v.x, v.y);
  }
};

template <> struct std::formatter<v2r64, char> {
  FORMATTER_NO_PARSE

  auto format(const v2r64 &v, std::format_context &fc) const noexcept {
    return std::format_to(fc.out(), "{}:{}", v.x, v.y);
  }
};
#pragma endregion

#pragma region data_combine
inline constexpr u16 u8_u8_to_u16(const std::integral auto high,
                                  const std::integral auto low) noexcept {
  return u16(((u16)(high & 0xff) << 8) | (u16)(low & 0xff));
}

inline constexpr u16 u8_u8_to_u16(const u8 high, const u8 low) noexcept {
  return u16(((u16)high << 8) | (u16)low);
}

inline constexpr u8 u8v_u8_in_u16(const std::integral auto u16) noexcept {
  return (u8)(u16 >> 8);
}

inline constexpr u8 u8_u8v_in_u16(const std::integral auto u16) noexcept {
  return (u8)u16;
}

inline constexpr u32 u16_u16_to_u32(const std::integral auto high,
                                    const std::integral auto low) noexcept {
  return u32(((u32)(high & 0xffff) << 16) | (u32)(low & 0xffff));
}

inline constexpr u32 u16_u16_to_u32(const u16 high, const u16 low) noexcept {
  return u32(((u32)high << 16) | (u32)low);
}

inline constexpr u16 u16v_u16_in_u32(const std::integral auto u32) noexcept {
  return (u16)(u32 >> 16);
}

inline constexpr u16 u16_u16v_in_u32(const std::integral auto u32) noexcept {
  return (u16)u32;
}

inline constexpr u64 u32_u32_to_u64(const std::integral auto high,
                                    const std::integral auto low) noexcept {
  return u64(((u64)(high & 0xffffffff) << 32) | (u64)(low & 0xffffffff));
}

inline constexpr u64 u32_u32_to_u64(const u32 high, const u32 low) noexcept {
  return u64(((u64)high << 32) | (u64)low);
}

inline constexpr u32 u32v_u32_in_u64(const std::integral auto u64) noexcept {
  return (u32)(u64 >> 32);
}

inline constexpr u32 u32_u32v_in_u64(const std::integral auto u64) noexcept {
  return (u32)u64;
}
#pragma endregion

inline constexpr void *as_void(const auto &v) noexcept { return (void *)&v; }
inline constexpr void *as_void(auto &&v) noexcept { return (void *)&v; }
inline constexpr void *as_void(auto *v) noexcept { return (void *)v; }
template <class T> inline constexpr T *as_pointer(const auto &v) noexcept {
  return (T *)as_void(v);
}
template <class T> inline constexpr T *as_pointer(auto &&v) noexcept {
  return (T *)as_void(v);
}
template <class T> inline constexpr T *as_pointer(auto *v) noexcept {
  return (T *)as_void(v);
}
#endif

template <u64 N> struct template_string {
  constexpr template_string(const char (&s)[N]) noexcept {
    std::ranges::copy(s, str);
  }
  char str[N]{};
};

template <template_string s> constexpr auto operator""_f() {
  return []<typename... Ts>(Ts... args) -> str {
    return std::format(s.str, args...);
  };
}
