cmake_minimum_required(VERSION 3.0)

project(s2arrows)

add_executable(s2arrows s2arrows.c)
target_compile_options(s2arrows PRIVATE -Wall -Wextra)

install(TARGETS s2arrows RUNTIME DESTINATION bin)
