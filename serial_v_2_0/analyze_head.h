//
// Created by yh on 3/27/18.
//

#ifndef SERIALREADANDSAVE_ANALYZE_HEAD_H
#define SERIALREADANDSAVE_ANALYZE_HEAD_H

#include <stdint.h>
#include "data.h"
#include "flash_head_data.h"
#include "utils.h"
#include "init.h"


int GetDynamicDataHeader(DYNAMIC_DATA_HEADER *dynamic_data_header_all)
{
    memset(dynamic_data_header_all,0, 2048*sizeof(DYNAMIC_DATA_HEADER));
    memset(g_origin_blocks_info,0, 2048*sizeof(BLOCK_INFO));
    memset(g_need_read_ecg_block_info,0, 2048*sizeof(BLOCK_INFO));

    FILE* infile;
    char file_name[100]={'\0'};
    MergeString3(file_name,WORKING_DIR,g_status.device_info.device_num,"/data_blocks_file_head.HEAD");
    infile = fopen(file_name, "rb");
    if(infile == NULL )
    {
        printf("data head file not exit\n");
        exit(1);
    }
    int rc = 0;
    unsigned char read_buf[66000] = {'\0'};
    //TODO
    rc = fread(read_buf,66000,1 ,infile);

    fclose(infile);
    for (int i = 0; i < 2048; ++ i)
    {
        //强制结构体转化
        DYNAMIC_DATA_HEADER *dynamic_data_header;
        dynamic_data_header = (DYNAMIC_DATA_HEADER *)(read_buf + 2 + sizeof(STATIC_DATA_BLOCK)+i * sizeof(DYNAMIC_DATA_HEADER));
        //判断是否有效
        if (dynamic_data_header->checksum == 305419896)//if effect
        {

            g_ALL_EFFECT_NUMBER ++;
            dynamic_data_header_all[i] = *dynamic_data_header;
            g_origin_blocks_info[i].effect = 1;
            g_origin_blocks_info[i].type = dynamic_data_header_all[i].type;
            g_origin_blocks_info[i].block_id = dynamic_data_header_all[i].start_addr;
            g_origin_blocks_info[i].length = dynamic_data_header_all[i].length;

            g_origin_blocks_info[i].is_bad = 0;
            g_origin_blocks_info[i].have_read = 0;
            bool data_normal = !g_origin_blocks_info[i].have_read
                               && !g_origin_blocks_info[i].is_bad
                               && g_origin_blocks_info[i].length;

            if(data_normal)//没有读取过并且有效
            {
                switch  (g_origin_blocks_info[i].type)
                {
                    case 1://ECG data need to read
                        for(int j = 0;j<8;++j)
                        {
                            g_origin_blocks_info[i].time_stamp += dynamic_data_header_all[i].start_time[j]<<(8*j);
                        }
                        g_need_read_ecg_block_info[g_NEED_READ_ECG_NUMBER] = g_origin_blocks_info[i];
                        printf("origin time :%d block id:%d\n", g_need_read_ecg_block_info[g_NEED_READ_ECG_NUMBER].time_stamp,g_need_read_ecg_block_info[g_NEED_READ_ECG_NUMBER].block_id);
                        g_NEED_READ_ECG_NUMBER ++;
                        break;
                    case 2:
                        break;
                    case 3:
                        break;
                    case 4:
                        break;
                }
            }
        }
        else//无效的数据
        {
            g_NO_EFFECT_NUMBER ++;
            g_origin_blocks_info[i].effect = 0;
            g_origin_blocks_info[i].type = 0;
            g_origin_blocks_info[i].block_id = 0;
            g_origin_blocks_info[i].time_stamp = 0;
            g_origin_blocks_info[i].is_bad = 0;
            g_origin_blocks_info[i].have_read = 0;
            break;
        }
    }
    return g_ALL_EFFECT_NUMBER;//有效的数据
}

/**
 * @func 按照时间从小到大的顺序升序排列
 * @param a
 * @param b
 * @return
 */
int CompareBlockTime(const BLOCK_INFO *a, const BLOCK_INFO *b)
{
    return a->time_stamp - b->time_stamp;
}

/** TODO
 * 对获得的原始数据头数组进行按照时间戳排序
 */
void SortBlockByTime()
{
    qsort(g_need_read_ecg_block_info, (size_t)g_NEED_READ_ECG_NUMBER, sizeof(BLOCK_INFO), CompareBlockTime);
    printf("g_NEED_READ_ECG_NUMBER = %d\n", g_NEED_READ_ECG_NUMBER);
    for(int i = 0  ;i <g_NEED_READ_ECG_NUMBER ;++ i)//读取的流程
    {
        printf("time :%d,block %d\n",g_need_read_ecg_block_info[i].time_stamp, g_need_read_ecg_block_info[i].block_id);
    }
    return;
}

void HandleDataHead()
{

    GetDynamicDataHeader(dynamic_data_header);
    SortBlockByTime();
    return;
}

unsigned char * GetDataBlockTime(unsigned char *time, const int index)
{
    for (int i = 0; i < 8; ++i) {
        time[i] = dynamic_data_header[index].start_time[i];
    }
    return time;
}


#endif //SERIALREADANDSAVE_ANALYZE_HEAD_H
