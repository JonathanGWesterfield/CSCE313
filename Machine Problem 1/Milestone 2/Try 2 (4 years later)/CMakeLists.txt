cmake_minimum_required(VERSION 3.17)
project(Malloc_Pt_2 C)

set(CMAKE_C_STANDARD 99)

add_executable(Test test.c)
add_executable(Malloc_Pt_2 my_allocator.c my_allocator.h memtest.c free_list.h free_list.c )