cmake_minimum_required(VERSION 2.8.4)
project(serialReadAndSave C)

set(CMAKE_C_STANDARD 99)

add_executable(serialReadAndSave  serial.h protocol.h data.h main.c dir.h)
