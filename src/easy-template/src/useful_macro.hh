/**
 * ****************useful_macro.hh****************
 * @brief  使用的宏
 * @author https://github.com/TheOneImmortal
 * @date   2023-10-27
 * ********************************
 */

#ifndef __XN_ET__USEFUL_MACRO_HH__
#define __XN_ET__USEFUL_MACRO_HH__

/*----------------include----------------*/
#pragma region include

#include <algorithm>
#include <utility>

#pragma endregion

/*----------------macro----------------*/
#pragma region macro

#define DELAY_TEMPLATE_RELOAD(function_name)                                   \
  [](auto &&...arg) { return function_name(std::forward(arg)...); }

#define DELAY_TEMPLATE_RUN(call) []() { call; }

#pragma endregion

#endif