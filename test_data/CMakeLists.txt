cmake_minimum_required(VERSION 2.8.4)
project(serialReadAndSave C)

#set extern libraries
set(LIBRARIES libm.so)
#set std 99
set(CMAKE_C_STANDARD 99)
# add exe
add_executable(serialReadAndSave  serial.h protocol.h data.h main.c dir.h unused.h config.h analyze_head.h)
#add link lib
TARGET_LINK_LIBRARIES(serialReadAndSave libm.so)