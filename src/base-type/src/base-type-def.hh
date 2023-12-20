#ifndef __XN_BT__BASETYPEDEF_HH__
#define __XN_BT__BASETYPEDEF_HH__

#include <format>
#include <string>

using wchar = wchar_t;

using byte = unsigned char;
using word = unsigned short;
using uint = unsigned int;
using ulong = unsigned long long;

using i8 = signed char;
using i16 = short;
using i32 = int;
using i64 = long long;

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using r32 = float;
using r64 = double;

using str = std::string;

struct v2i8 {
  i8 x, y;
};
struct v2i16 {
  i16 x, y;
};
struct v2i32 {
  i32 x, y;
};
struct v2i64 {
  i64 x, y;
};

struct v2r32 {
  r32 x, y;
};
struct v2r64 {
  r64 x, y;
};

#endif