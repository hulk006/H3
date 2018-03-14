//
// Created by openwrt on 18-1-23.
//

#ifndef SERIALPROJECT_DATA_H
#define SERIALPROJECT_DATA_H

#include <stdint.h>
#include <string.h>
#include "protocol.h"

struct DeviceInfo
{
    char name[12+1];// Model number
    char serial_no[12+1];// Serial number
    char mac_id[6+1];//
} ;
struct UserBindInfo
{

    char user_id[12+1];// If bond, the bonding ID, all zero means no bond
    char user_name[16+1];// user name
    char bind;// 0 or 1  ,is bounded?
};

struct NetConfig
{
    char SSID[32+1];//多了一个
    char PWD[32+1];
};
//Status结构体，用来存储串口通信的状态信息
struct  Status
{
    char is_ready;// Are you ready ?

    struct NetConfig net_config;
    struct DeviceInfo device_info;
    struct UserBindInfo user_bind_info;

    char update_time[4];// start update time
    /*
     * 心电盒子的同步状态
     * 0同步失败，需要重来；
     * 1同步中未完成，继续请求；
     * 2同步已经完成，关闭与盒子到连接
    */
    ////命令13
    char time_sync_state;
    char sync_time[8];// 同步完的时间
    ////命令14
    char once_data_sync_state;//同步一次block的的状态 初始=‘0’成功为‘1’
    int ndata_blocks;// 当前已经同步了多少个blocks
    ////命令15
    char state;//命令15的state
    int remain_blocks;//剩余多少block没同步完成，初始化时应该尾1000
} ;
struct Status status={
        .remain_blocks = 999,
        .state = '0',
        .time_sync_state = '0',
        .once_data_sync_state = '0',
        .ndata_blocks = 0,
        .update_time[0] = '0',
        .update_time[1] = '0',
        .update_time[2] = '0',
        .update_time[3] = '0',
        .is_ready = '1',
        .ndata_blocks = 0,
        .user_bind_info = {
                .user_name = "user",
                .user_id = "123456",
                .bind = '1',
        },
        .device_info = {
                .mac_id = "macID",
                .serial_no = "serial_no",
                .name = "device name",
        },
        .net_config = {
                .SSID = {'0'},
                .PWD = {'0'},
        },
};
//TODO
struct DataBlock
{
    int n_data_block;
    char rec_buf[12000];//预留足够大的空间，来存储收到的data block
    int upload_to_cloud;
};

struct DataBlock data_block={
        .n_data_block = 1,
        .rec_buf = {'\0'},
        .upload_to_cloud=0,//上传云端是否成功
};
void InitStatus()
{
    status.remain_blocks = 999;//
    status.state = '0';//
    status.time_sync_state = '0';//
    status.once_data_sync_state = '0';
    status.ndata_blocks = 0;
    status.update_time[0] = '0';status.update_time[1] = '0';status.update_time[2] = '0';status.update_time[3] = '0';
    status.is_ready = '1';//盒子的状态0 ready ，1 busy
    status.ndata_blocks = 0;
    return;
}
void InitDataBlock()
{
    data_block.n_data_block = 1;
    data_block.upload_to_cloud = 0;
    bzero(data_block.rec_buf, sizeof(data_block.rec_buf));
    return;
}
#endif //SERIALPROJECT_DATA_H
