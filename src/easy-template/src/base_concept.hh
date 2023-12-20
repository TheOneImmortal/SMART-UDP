/**
 * ****************base-concept.hh****************
 * @brief  基础约束
 * @author https://github.com/TheOneImmortal
 * @date   2023-12-10
 * ********************************
 */

#ifndef __XN_ET__BASE_CONCEPT_HH__
#define __XN_ET__BASE_CONCEPT_HH__

/*----------------include----------------*/
#include <algorithm>
#include <bits/utility.h>

/*----------------body----------------*/
namespace xn {

/**
 * @brief 是否 类型不是 常量、非优化量、引用、指针
 *
 * @tparam T 被判断类型
 */
template <typename T>
concept is_no_cvrp = requires() {
  requires std::is_same_v<T, std::remove_pointer_t<std::remove_cvref_t<T>>>;
};

/**
 * @brief 是否 类型可 实例化
 *
 * @tparam T 被判断类型
 */
template <typename T>
concept is_can_ins = requires() { T{}; };

} // namespace xn

#endif
