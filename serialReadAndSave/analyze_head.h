//
// Created by yh on 3/27/18.
//

#ifndef SERIALREADANDSAVE_ANALYZE_HEAD_H
#define SERIALREADANDSAVE_ANALYZE_HEAD_H

#include <stdint.h>

#pragma pack(1)
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
} STATIC_DATA_BLOCK;
#pragma pack()

int GetTotalBlocksNum(const char *working_dir,char *user_id)
{
    STATIC_DATA_BLOCK *data_block ;
    FILE *infile;
    char file_name[100]={'\0'};
    MergeString3(file_name,working_dir,user_id,"/data_blocks_file_head.dat");
    infile = fopen(file_name, "rb");
    if(infile == NULL )
    {
        printf("head file not exit\n");
        exit(1);
    }
    int rc = 0;
    unsigned char read_buf[66000] = {0x00};
    rc = fread(read_buf,sizeof(read_buf),1 ,infile);
    data_block = (STATIC_DATA_BLOCK *)(read_buf + 2);
    int num = data_block->nand_block_num;
    fclose(infile);
    return num;
}
#pragma pack()





#endif //SERIALREADANDSAVE_ANALYZE_HEAD_H
