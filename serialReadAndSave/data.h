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
    unsigned char name[14+1];// Model number??
    unsigned char mac_id[6+1];//
} ;
struct UserBindInfo
{
    unsigned char user_id[12+1];// If bond, the bonding ID, all zero means no bond
    int bind;// 0 or 1  ,is bounded?
};

struct NetConfig
{
    unsigned char SSID[32+1];//多了一个
    unsigned char PWD[32+1];
};
//Status结构体，用来存储串口通信的状态信息
struct  Status
{
    char is_ready;// Are you ready ?

    struct NetConfig net_config;
    struct DeviceInfo device_info;
    struct UserBindInfo user_bind_info;

    unsigned char update_time[4];// start update time
    /*
     * 心电盒子的同步状态
     * 0同步失败，需要重来；
     * 1同步中未完成，继续请求；
     * 2同步已经完成，关闭与盒子到连接
    */
    ////命令13
    int time_sync_state;
    unsigned char sync_time[8];// 同步完的时间
    ////命令14
    unsigned char once_data_sync_state;//同步一次block的的状态 初始=‘0’成功为‘1’
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
                .user_id = "unbinduser",
                .bind = 0,
        },
        .device_info = {
                .mac_id = "macID",
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
    unsigned char rec_buf[256 * 1024 + 8];//预留足够大的空间，来存储收到的data block
    int upload_to_cloud;
};

struct DataBlock data_block={
        .n_data_block = 1,
        .rec_buf = {'\0'},
        .upload_to_cloud=0,//上传云端是否成功
};

#pragma pack(4)
typedef struct static_data_block
{
    uint16_t static_data_length; 		// static data block 的有效数据长度，以 byte 做单位不包括本字段的长度
    uint32_t dynamic_data_header_addr; 	// Start address of dynamic data header
    uint8_t data_format_ver; 			// flash 数据存储格式版本，从 0 开始，每更新一版加 1
    uint8_t hw_ver[10]; // Hardware version
    uint8_t ble_mac[6]; // BLE MAC address
    uint8_t model_no[8]; // Model number
    uint8_t serial_no[12]; // Serial number
    uint8_t sync_time[8]; // 同步完的时间
    // ECG config data
    uint8_t ecg_sample_rate; // 00b-125Hz 01b-250Hz 10b-500Hz 11b-1000Hz
    uint8_t ecg_channel_no; // ECG 导联数
    // Breadthing config data
    uint8_t breathing_sample_rate; // 00b-125Hz 01b-250Hz 10b-500Hz 11b-1000Hz
    // G sensor config data
    uint8_t g_sensor_sample_rate; // 采样率多少该数值就是多少，最大 255Hz

    union Flag
    {
        uint8_t flag;
        struct Bond
        {
            uint8_t bond:1; // 0-No bond 1-Bond
            uint8_t reserved0:7;
        };
        struct Bond bond;
    };

    union Flag bondFlag;
    uint8_t user_id[12]; // If bond, the bonding ID, all zero means no bond

    uint16_t nand_block_num;	// block 个数
    uint32_t nand_block_size;	// block 大小
    uint16_t nand_page_size;	// 页大小

    uint16_t ecg_save_block;
    uint16_t sensor_save_block;
    uint16_t resp_save_block;
    uint16_t alarm_save_block;
}STATIC_DATA_BLOCK;

// Dyanmic data
typedef struct dynamic_data_header
{
    uint8_t type; // 1-ECG data, 2-Breath data, 3-G sensor data, 4-Status data
    union Flag1   //????
    {
        uint8_t flag;
        struct Effect
        {
            uint8_t effect:1; // 该 data block 是否有效 0-not effective 1-effective
            uint8_t reserved:7;
        };
        struct Effect effect;
    };

    uint8_t start_time[8];
    uint8_t reserved1[2];
    uint32_t start_addr; 	// Start address(定义为Block的编号从1开始，依次累加)
    uint32_t length; 		// Data length，不包含 header
    uint32_t checksum; 		// 所有 data 相加的和
}DYNAMIC_DATA_HEADER;

// ECG data block
typedef struct ecg_data
{
    uint32_t ecg_data1;
    uint32_t ecg_data2;
}ECG_DATA;

// Breath data block
typedef struct ecg_breath_data
{
    uint32_t ecg_data1;
    uint32_t breath_data;
}ECG_BREATH_DATA;
#pragma pack()

DYNAMIC_DATA_HEADER dynamic_data_header[1000]={'\0'};
int DATA_NUMBER = 0;


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


/**
 * @brief  合并字符串
 * @return des为p0+p1
*/
char * MergeString2(char *des,char const *p0,char const *p1)
{
    strcat(des,p0);
    strcat(des,p1);
    return des;
}
char * MergeString3(char *des,char const  *p0,char const  *p1,char const *p2)
{
    strcat(des,p0);
    strcat(des,p1);
    strcat(des,p2);
    return des;
}
char * MergeString4(char *des,char const *p0, char const *p1,char const *p2,char const *p3)
{
    strcat(des,p0);
    strcat(des,p1);
    strcat(des,p2);
    strcat(des,p3);
    return des;
}
#endif //SERIALPROJECT_DATA_H
