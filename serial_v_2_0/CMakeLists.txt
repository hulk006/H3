cmake_minimum_required(VERSION 2.8.4)
project(serial_v_2_0 C)

#set extern libraries
set(LIBRARIES libm.so)
#set std 99
set(CMAKE_C_STANDARD 99)
# add exe
add_executable(serial_v_2_0  serial.h protocol.h data.h main.c dir.h daemon.h config.h analyze_head.h utils.h init.h flash_head_data.h)
#add link lib
TARGET_LINK_LIBRARIES(serial_v_2_0 libm.so)