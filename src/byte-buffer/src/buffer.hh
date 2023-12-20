/**
 * ****************buffer.hh****************
 * @brief  字节缓冲，小心使用，本系列为保证性能，没有任何泄露检查
 * @author https://github.com/TheOneImmortal
 * @date   2023-10-29
 * ********************************
 */

#ifndef __XN_BB__BUFFER_HH__
#define __XN_BB__BUFFER_HH__

/*----------------include----------------*/
#include <algorithm>
#include <initializer_list>
#include <type_traits>

#include "../../base-type/include_me.hh"

/*----------------body----------------*/
namespace xn {

using BufferSizeType = u16;

struct BufferOnHeap;
struct BufferOnHad;

template <BufferSizeType S> struct Buffer {
  static inline constexpr BufferSizeType size = S;

  byte data[S];

  constexpr BufferOnHad From(const BufferSizeType from) noexcept;
  constexpr BufferOnHad From(const BufferSizeType from,
                             const BufferSizeType size) noexcept;
};
template <> struct Buffer<0> {
  static inline constexpr BufferSizeType size = 0;

  static inline constexpr const byte *const data = nullptr;
};

struct BufferOnHeap {
  constexpr BufferOnHeap(const BufferSizeType size) noexcept
      : size(size), data(size ? new byte[size] : nullptr) {}
  constexpr BufferOnHeap(const std::initializer_list<byte> data) noexcept
      : size(data.size()), data(new byte[size]) {
    std::copy(data.begin(), data.end(), this->data);
  }
  constexpr BufferOnHeap(BufferOnHeap &&other) noexcept
      : data(other.data), size(other.size) {
    other.data = nullptr;
    other.size = 0;
  };
  constexpr ~BufferOnHeap() noexcept {
    if (data)
      delete[] data;
  }

  BufferSizeType size = 0;
  byte *data;

  constexpr BufferOnHad From(const BufferSizeType from) const noexcept;
  constexpr BufferOnHad From(const BufferSizeType from,
                             const BufferSizeType size) const noexcept;
};

class BufferOnHad {
public:
  constexpr BufferOnHad() noexcept {}
  constexpr BufferOnHad(const Buffer<0> &buffer) noexcept {}
  constexpr BufferOnHad(const BufferOnHad &buffer) noexcept
      : size(buffer.size), data(buffer.data) {}
  constexpr BufferOnHad(const BufferOnHeap &buffer) noexcept
      : size(buffer.size), data(buffer.data) {}
  template <BufferSizeType S>
  constexpr BufferOnHad(Buffer<S> &buffer) noexcept
      : size(buffer.size), data(buffer.data) {}
  constexpr BufferOnHad(const BufferOnHad &buffer,
                        const BufferSizeType from) noexcept
      : size(buffer.size - from), data(&buffer.data[from]) {}
  constexpr BufferOnHad(const BufferOnHeap &buffer,
                        const BufferSizeType from) noexcept
      : size(buffer.size - from), data(&buffer.data[from]) {}
  template <BufferSizeType S>
  constexpr BufferOnHad(Buffer<S> &buffer, const BufferSizeType from) noexcept
      : size(buffer.size - from), data(&buffer.data[from]) {}
  constexpr BufferOnHad(const BufferOnHad &buffer, const BufferSizeType from,
                        const BufferSizeType size) noexcept
      : size(size), data(&buffer.data[from]) {}
  constexpr BufferOnHad(const BufferOnHeap &buffer, const BufferSizeType from,
                        const BufferSizeType size) noexcept
      : size(size), data(&buffer.data[from]) {}
  template <BufferSizeType S>
  constexpr BufferOnHad(Buffer<S> &buffer, const BufferSizeType from,
                        const BufferSizeType size) noexcept
      : size(size), data(&buffer.data[from]) {}

  BufferSizeType size = 0;
  byte *data = nullptr;

  constexpr BufferOnHad From(const BufferSizeType from) const noexcept {
    return BufferOnHad(*this, from);
  }
  constexpr BufferOnHad From(const BufferSizeType from,
                             const BufferSizeType size) const noexcept {
    return BufferOnHad(*this, from, size);
  }
};
template <BufferSizeType S>
constexpr BufferOnHad Buffer<S>::From(const BufferSizeType from) noexcept {
  return BufferOnHad(*this, from);
}
template <BufferSizeType S>
constexpr BufferOnHad Buffer<S>::From(const BufferSizeType from,
                                      const BufferSizeType size) noexcept {
  return BufferOnHad(*this, from, size);
};
constexpr BufferOnHad
BufferOnHeap::From(const BufferSizeType from) const noexcept {
  return BufferOnHad(*this, from);
}
constexpr BufferOnHad
BufferOnHeap::From(const BufferSizeType from,
                   const BufferSizeType size) const noexcept {
  return BufferOnHad(*this, from, size);
};

template <class T>
  requires(sizeof(T) != 0)
static inline constexpr Buffer<sizeof(T)> to_bytes(const T &v) {
  Buffer<sizeof(T)> buffer;
  memcpy(as_pointer<char>(buffer.data[0]), as_pointer<char>(v), sizeof(T));
  return buffer;
}

/**
 * @brief 在传入的buffer上写
 *
 * @tparam B
 * @tparam T
 * @param buffer 写的位置
 * @param v 写值
 * @return constexpr BufferOnHad<B>
 * 如果成功，会返回剩下的Buffer区，如果失败，data置为nullptr
 */
template <class T, class... Ts>
static inline constexpr BufferOnHad to_bytes(const BufferOnHad &buffer,
                                             const T &v, const Ts &...vs) {

  if (buffer.size < sizeof(T))
    return BufferOnHad(0);
  memcpy(as_pointer<char>(buffer.data[0]), as_pointer<char>(v), sizeof(T));

  if constexpr (sizeof...(Ts) == 0)
    return buffer.From(sizeof(T));
  else
    return to_bytes(buffer.From(sizeof(T)), vs...);
}

template <class T>
  requires(sizeof(T) != 0)
static inline constexpr Buffer<sizeof(BufferSizeType) + sizeof(T)>
to_bytes_with_size(const T &v) {
  Buffer<sizeof(BufferSizeType) + sizeof(T)> buffer;
  *as_pointer<BufferSizeType>(buffer.data[0]) = sizeof(T);
  memcpy(as_pointer<char>(buffer.data[sizeof(BufferSizeType)]),
         as_pointer<char>(v), sizeof(T));
  return buffer;
}
/**
 * @brief 在传入的buffer上写
 *
 * @tparam B
 * @tparam T
 * @param buffer 写的位置
 * @param v 写值
 * @return constexpr BufferOnHad<B>
 * 如果成功，会返回剩下的Buffer区，如果失败，data置为nullptr
 */
template <class T>
  requires(sizeof(T) != 0)
static inline constexpr BufferOnHad
to_bytes_with_size(const BufferOnHad &buffer, const T &v) {
  if (buffer.size < (sizeof(BufferSizeType) + sizeof(T)))
    return BufferOnHad(0);
  *as_pointer<BufferSizeType>(buffer.data[0]) = sizeof(T);
  memcpy(as_pointer<char>(buffer.data[0]), as_pointer<char>(v), sizeof(T));
  return buffer.From(sizeof(BufferSizeType) + sizeof(T));
}

template <class T>
  requires(sizeof(T) != 0)
static inline constexpr T from_bytes(const BufferOnHad &buffer) {
  T v;
  memcpy(as_pointer<char>(v), as_pointer<char>(buffer.data[0]), sizeof(T));
  return v;
}

} // namespace xn

#endif