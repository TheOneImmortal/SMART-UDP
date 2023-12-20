/**
 * ****************ip.hh****************
 * @brief  将常用IP提前计算为二进制
 * @author https://github.com/TheOneImmortal
 * @date   2023-10-27
 * ********************************
 */

#ifndef __XN_BN__IP_HH__
#define __XN_BN__IP_HH__

/*----------------include----------------*/
#pragma region include

#include <algorithm>
#include <winsock2.h>

#include "../../base-type/include_me.hh"

#pragma endregion

/*----------------body----------------*/
namespace xn {

constexpr u64 ip_any = 0x00'00'00'00;
constexpr u64 ip_local = 0x7f'00'00'01;

const u64 ip_any_n = htonl(ip_any);
const u64 ip_local_n = htonl(ip_local);

} // namespace xn

#endif