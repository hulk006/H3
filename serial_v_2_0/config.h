//
// Created by yh on 3/12/18.
//

#ifndef SERIALREADANDSAVE_CONFIG_H
#define SERIALREADANDSAVE_CONFIG_H

#define WAIT_TIME_RESTART 10
#define FRAME_HEAD_LENGTH 2
#define FRAME_TAIL_LENGTH 2
#define SSID_LENGTH 32
#define PWD_LENGTH 32
#define DEVICE_NAME_LENGTH 14
#define DEVICE_SN_LENGTH 12
#define DEVICE_MACID_LENGTH 6
#define USER_ID_LENGTH 12
#define USER_NAME_LENGTH 16
//// \r\n 不同的系统下不一样需要修改
#define CHAR_STOP_BITS 0
#define HEAD_DATA_BLOCK_SIZE 65540
#define DATA_BLOCK_SIZE 262148
#define NUM 1//一次发送几个block
#define NUM_ONCE_BLOCKS 0x01
#define ANSWER_15_LENGTH 8

char WORKING_DIR[100] = {'\0'};
char FRAME_HEAD[] = {'H','3'};
char FRAME_TAIL[] = {'5','A'};
//第一个命令

const char REUQEST_01_ISREADY[8]     = {'H','3',0x01,0x00,0x00,0x00,'5','A'};//第一个命令：询问是否可以同步数据的指令
const char REUQEST_10_NET_CONFIG[8]  = {'H','3',0x10,0x00,0x00,0x00,'5','A'};//第二个命令：请求读取盒子的网络配置的指令
const char REUQEST_11_DEVICE_INFO[8]  = {'H','3',0x11,0x00,0x00,0x00,'5','A'};//第三个命令：请求读取盒子的设备信息的指令
const char REUQEST_12_USER_INFO[8]    = {'H','3',0x12,0x00,0x00,0x00,'5','A'};//第四个命令：请求读取与盒子绑定用户信息的指令
//to change
char REUQEST_13_SYNC_TIME[10]  = {'H','3',0x13,0x00,0x00,0x00,0x00,0x00,'5','A'};//第五个命令：请求同步时间


const char REUQEST_16_SERVER_IP[8]    = {'H','3',0x16,0x00,0x00,0x00,'5','A'};//第四个命令：请求读取与盒子绑定用户信息的指令
const char REUQEST_17_KEY[8]    = {'H','3',0x17,0x00,0x00,0x00,'5','A'};//第四个命令：请求读取与盒子绑定用户信息的指令

#endif //SERIALREADANDSAVE_CONFIG_H
