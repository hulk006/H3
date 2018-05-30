cmake_minimum_required(VERSION 2.8.4)
project(test_data C)

#set extern libraries
set(LIBRARIES libm.so)
#set std 99
set(CMAKE_C_STANDARD 99)
# add exe
add_executable(test_data  serial.h protocol.h data.h main.c dir.h unused.h config.h analyze_head.h)
#add link lib
TARGET_LINK_LIBRARIES(test_data libm.so)