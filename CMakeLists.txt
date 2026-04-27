cmake_minimum_required(VERSION 3.10)
project(libtrm C)

set(CMAKE_C_STANDARD 11)

add_executable(libtrm_tutorial main.c libtrm.h)

add_executable(libtrm_errors errors_test.c libtrm.h)