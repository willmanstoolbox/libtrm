cmake_minimum_required(VERSION 4.0)
project(libtrm C)

set(CMAKE_C_STANDARD 11)

add_executable(libtrm main.c
        libtrm.h)
