//
// Created by openwrt on 18-1-23.
//

#ifndef SERIALPROJECT_DATA_H
#define SERIALPROJECT_DATA_H

#include <stdint.h>
#include <string.h>
#include "protocol.h"
#include "config.h"


/**2048 flash head struct in H3SMALL box*/
typedef struct BlockInfo//可以等号直接赋值，没有要写一个等号的赋值函数
{
    int time_stamp;/***/
    uint32_t length;
    uint8_t block_id;/**block address in small box*/
    uint8_t type;/**1 represents ECG |2 |3 |4 */
    uint8_t have_read;/** wether the block is new for us, 1 a old block have been read|0 new*/
    uint8_t is_bad;/** bad block unread*/
    uint8_t effect;
} BLOCK_INFO;

typedef struct DeviceInfo
{
    unsigned char name[14+1];// Model number??
    unsigned char mac_id[6+1];//
    char device_num[12+1];
}DEVICE_INFO;
typedef struct UserBindInfo
{
    unsigned char user_id[12+1];// If bond, the bonding ID, all zero means no bond
    int bind;// 0 or 1  ,is bounded?
}USER_BIND_INFO;

typedef struct NetConfig
{
    unsigned char SSID[32+1];//多了一个
    unsigned char PWD[32+1];
}NET_CONFIG;
//Status结构体，用来存储串口通信的状态信息
typedef struct  Status
{
    char is_ready;// Are you ready ?
    NET_CONFIG net_config;
    DEVICE_INFO device_info;
    USER_BIND_INFO user_bind_info;

    unsigned char update_time[4];// start update time
    /*
     * 心电盒子的同步状态
     * 0同步失败，需要重来；
     * 1同步中未完成，继续请求；
     * 2同步已经完成，关闭与盒子到连接
    */
    ////命令13
    int time_sync_state;
    char secret_key[16+1];
    char server_ip[10];
    ////命令14
    unsigned char once_data_sync_state;//同步一次block的的状态 初始=‘0’成功为‘1’
    ////命令15
    int state;//命令15的state
}STATUS;


typedef struct DataBlock
{
    int n_data_block;
    unsigned char rec_buf[256 * 1024 + 8];//预留足够大的空间，来存储收到的data block
    int upload_to_cloud;
}DATA_BLOCK;


#endif //SERIALPROJECT_DATA_H
