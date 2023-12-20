cmake_minimum_required(VERSION 3.5)

project(TEST)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(./src)
message("xn_lib: ${xn_lib}")

set(src ./test/test.cc)

add_executable(runme ${src} ${xn_lib})
target_link_libraries(runme ws2_32)
