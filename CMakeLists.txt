cmake_minimum_required(VERSION 3.21)
project(SVHSS)

set(CMAKE_CXX_STANDARD 14)

add_executable(SVHSS main.c split.h split.c compute.h compute.c BV11.h BV11.c tool.h tool.c compute.h SVHSS.c SVHSS.h)

target_link_libraries(SVHSS libgmp.so /usr/local/lib/libflint.so libm.so)
