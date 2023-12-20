cmake_minimum_required(VERSION 3.5)

project(XNLIB)

file(GLOB src */src/*.cc)

set(xn_lib "${src}" CACHE INTERNAL "xn lib cc文件")
