//
// Created by openwrt on 18-1-23.
//

#ifndef SERIALPROJECT_DATA_H
#define SERIALPROJECT_DATA_H

#include <stdint.h>
#include <string.h>
#include "protocol.h"
#include "config.h"

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
    int bind;// 0 or 1  ,is bounded?
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
    int time_sync_state;
    char sync_time[8];// 同步完的时间
    ////命令14
    char once_data_sync_state;//同步一次block的的状态 初始=‘0’成功为‘1’
    ////命令15
    int state;//命令15的state
    ////当前已经同步了多少个blocks
    int ndata_blocks;
    unsigned char n_blocks[3];
    ////剩余多少block没同步完成，初始化时应该尾1000
    int remain_blocks;
    int total_blocks_num;
    unsigned char n_remain_blocks[3];
} ;
struct Status status={
        .remain_blocks = 1000,
        .state = 0,
        .time_sync_state = 1,
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
                .user_id = "unbinduser",
                .bind = 0,
        },
        .device_info = {
                .mac_id = "macID",
                .serial_no = "serial_no",
                .name = "device name",
        },
        .net_config = {
                .SSID = {'l','e','n','o','v','o'},
                .PWD = {'1','2','3','4','5','6','7','8','9'},
        },
        .n_blocks={0x00,0x00,0x00},
        .n_remain_blocks={0xff,0xff,0xff},
        .total_blocks_num=1000,
};
//TODO
struct DataBlock
{
    int n_data_block;
    unsigned char rec_buf[256 * 1024 + 5];//预留足够大的空间，来存储收到的data block
    int upload_to_cloud;
};

struct DataBlock data_block={
        .n_data_block = 1,
        .rec_buf = {'\0'},
        .upload_to_cloud=0,//上传云端是否成功
};
void InitStatus()
{

    status.state = 0;//
    status.time_sync_state = 0;//
    status.once_data_sync_state = 0;
    status.ndata_blocks = 0;
    status.update_time[0] = '0';status.update_time[1] = '0';status.update_time[2] = '0';status.update_time[3] = '0';
    status.is_ready = '1';//盒子的状态0 ready ，1 busy

    status.ndata_blocks = 0;
    status.n_blocks[0] = 0x00;status.n_blocks[1] = 0x00;status.n_blocks[2] = 0x00;

    status.remain_blocks = 1000;//假设剩余比较大的data block 数量
    status.n_remain_blocks[0] = 0xff; status.n_remain_blocks[1] = 0xff; status.n_remain_blocks[2] = 0xff;
    status.total_blocks_num=1000;
    return;
}
void InitDataBlock()
{
    data_block.n_data_block = NUM;
    data_block.upload_to_cloud = 0;
    bzero(data_block.rec_buf, sizeof(data_block.rec_buf));
    return;
}
#endif //SERIALPROJECT_DATA_H
