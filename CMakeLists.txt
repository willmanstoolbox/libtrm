cmake_minimum_required(VERSION 3.10)
project(libtrm C)

set(CMAKE_C_STANDARD 11)

add_executable(libtrm_tutorial examples/tutorial.c libtrm.h)

add_executable(libtrm_errors examples/error_handling.c libtrm.h)