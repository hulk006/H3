//
// Created by yh on 3/27/18.
//

#ifndef SERIALREADANDSAVE_ANALYZE_HEAD_H
#define SERIALREADANDSAVE_ANALYZE_HEAD_H

#include <stdint.h>
#include "data.h"


DYNAMIC_DATA_HEADER * GetDYNAMIC_DATA_HEADER(DYNAMIC_DATA_HEADER *dynamic_data_header,int address, unsigned char *head_buf)
{
    dynamic_data_header = (DYNAMIC_DATA_HEADER *) (head_buf + 2 + address*sizeof(STATIC_DATA_BLOCK));
    return dynamic_data_header;
}

int Get_Dynamic_Data_Header(DYNAMIC_DATA_HEADER *dynamic_data_header_all)
{
    int a= sizeof(*dynamic_data_header_all);
    memset(dynamic_data_header_all,0, 2048*sizeof(DYNAMIC_DATA_HEADER));


    FILE* infile;
    char file_name[100]={'\0'};
    MergeString3(file_name,WORKING_DIR,status.user_bind_info.user_id,"/data_blocks_file_head.HEAD");
    infile = fopen(file_name, "rb");
    if(infile == NULL )
    {
        printf("not exit/n");
        exit(1);
    }
    int rc = 0;
    unsigned char read_buf[66000] = {'\0'};
    rc = fread(read_buf,66000,1 ,infile);
    for (int i = 0; i < 1000; ++ i)
    {
        DYNAMIC_DATA_HEADER *dynamic_data_header;
        dynamic_data_header = (DYNAMIC_DATA_HEADER *)(read_buf + 2 + sizeof(STATIC_DATA_BLOCK)+i*sizeof(DYNAMIC_DATA_HEADER));
        if (dynamic_data_header->checksum == 305419896)
        {
            DATA_NUMBER ++;
            dynamic_data_header_all[i]=*dynamic_data_header;
        }
        else{
            break;
        }
    }
    return DATA_NUMBER;
}
uint32_t Get_Blocks_Address(int index)
{
    uint32_t address = dynamic_data_header[index].start_addr;
    return address;

}

unsigned char * GetTime(unsigned char *time,int address)
{

    FILE* infile;
    char file_name[100]={'\0'};
    MergeString3(file_name,WORKING_DIR,status.user_bind_info.user_id,"/data_blocks_file_head.HEAD");
    infile = fopen(file_name, "rb");
    if(infile == NULL )
    {
        printf("not exit/n");
        exit(1);
    }
    int rc = 0;
    unsigned char read_buf[66000] = {'\0'};
    rc = fread(read_buf,66000,1 ,infile);
    DYNAMIC_DATA_HEADER *dynamic_data_header;
    dynamic_data_header = (DYNAMIC_DATA_HEADER *)(read_buf + 2 + sizeof(STATIC_DATA_BLOCK)+address*sizeof(DYNAMIC_DATA_HEADER));
    time = dynamic_data_header->start_time;
    return time;
}

int GetTotalBlocksNum(const char *working_dir,char *user_id)
{
    STATIC_DATA_BLOCK *data_block ;
    FILE *infile;
    char file_name[100]={'\0'};
    MergeString3(file_name,working_dir,user_id,"/data_blocks_file_head.HEAD");
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
/*


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

// G sensor data block
typedef struct g_sensor_d

uint8_t * GetTimeStamp(uint8_t *time,int length,int address, unsigned char *head_buf)
{
    DYNAMIC_DATA_HEADER *dynamic_data_header_0;
    dynamic_data_header_0 = (DYNAMIC_DATA_HEADER *) (head_buf + 2 + sizeof(STATIC_DATA_BLOCK));
    for (int j = 0; j < 8; ++j) {
        time[j] = dynamic_data_header_0->start_time[j];
        printf("%x", time[j]);
    }
    return time;
}
 */
/**



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
 */