//
// Created by yh on 6/4/18.
//

#ifndef SERIAL_V_2_0_DATA_HEAD_H
#define SERIAL_V_2_0_DATA_HEAD_H


/**
 * 下面的数据结构体是心电盒子发送不过给底座的原始结果结构体
 * 需要将原始的数据头转化为自己需要的结构体
 */
#pragma pack(4)		// 数据的开始地址必须四字节对其，这是FDS要求的

#include <elf.h>

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

    union Flag1
    {
        uint8_t flag;
        struct Bond
        {
            uint8_t bond:1; // 0-No bond 1-Bond
            uint8_t reserved0:7;
        };
        struct Bond bond;
    };

    union Flag1 bondFlag;
    uint8_t user_id[12]; // If bond, the bonding ID, all zero means no bond

    uint16_t nand_block_num;	// block 个数
    uint32_t nand_block_size;	// block 大小
    uint16_t nand_page_size;	// 页大小

    int16_t ecg_save_block;	// 当前心电数据正在写入的Blcok
    int16_t sensor_save_block;
    int16_t resp_save_block;
    int16_t alarm_save_block;
}STATIC_DATA_BLOCK;

// Dyanmic data
typedef struct dynamic_data_header
{
    uint8_t type; // 1-ECG data, 2-Breath data, 3-G sensor data, 4-Status data
    union Flag
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
    uint32_t start_addr; 	// 当前Block number
    uint32_t length; 		// 当前Block中储存有效数据长度
    uint32_t checksum; 		//
}DYNAMIC_DATA_HEADER;

// ECG data block
typedef struct ecg_data
{
    uint16_t ecg_data1;
    uint16_t ecg_data2;
}ECG_DATA;

// Breath data block
typedef struct ecg_breath_data
{
    uint32_t ecg_data1;
    uint32_t breath_data;
}ECG_BREATH_DATA;

// G sensor data block
typedef struct g_sensor_data
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
}G_SENSOR_DATA;

#pragma pack()





#endif //SERIAL_V_2_0_DATA_HEAD_H
