//
// Created by yh on 6/4/18.
//

#ifndef SERIAL_V_2_0_INIT_H
#define SERIAL_V_2_0_INIT_H

#define LED_BRIGHTNESS "/root/led_control/led_bright.sh "
#define LED_TWINKLE "sh /root/led_control/led_twinkle.sh"
#define TIME_UPDATE "/etc/init.d/sysfixtime start"
/********************************GLOBAL DATA***************************************************************************************/
/**
 * 自己定义的结构体的初始化
 */
#include "data.h"
#include "flash_head_data.h"

/******************************************new global***************************************************/

int g_ALL_EFFECT_NUMBER    = 0;//2048中，有效的数据块 数量
int g_NO_EFFECT_NUMBER     = 0;//2048中，无效的数据块 数量
int g_NEED_READ_ECG_NUMBER = 0;//有效的数据块中，本次需要读取的数据块 数量
int g_HAVE_READ_ECG_NUMBER = 0;
int g_DATA_HEAD_LENGTH     = 64*1024 + 4;
int g_DIR_MAX_SIZE         = 4;//MB

int g_BAUDRATE             = 2000000;
char g_UART_DEVICE[30]     = "/dev/ttyUSB0";

int g_baude_speed_arr[]    = {2000000,921600,115200, 57600, 38400, 19200, 9600,
                     4800, 2400, 1200, 300, 115200, 38400, 19200, 9600, 4800, 2400, 1200, 300 };

DYNAMIC_DATA_HEADER dynamic_data_header[2048]={'\0'};//原始动态头
BLOCK_INFO g_origin_blocks_info[2048] = {'\0'};//原始的结构体,包括所有的block信息
BLOCK_INFO g_need_read_ecg_block_info[2048] = {'\0'};//时间升序的结构体，只含有本次需要读取的ecg数据块


STATUS g_status = {

        .state = 0,
        .time_sync_state      = 1,
        .once_data_sync_state = '0',
        .secret_key           = "\0",
        .server_ip            = "\0",
        .update_time[0]       = '0',
        .update_time[1]       = '0',
        .update_time[2]       = '0',
        .update_time[3]       = '0',
        .is_ready             = '1',
        .user_bind_info       =
                {
                .user_id      = "unbinduser",
                .bind         = 0,
                },
        .device_info          =
                {
                .mac_id       = "macID",
                .name = "device name",
                },
        .net_config           =
                {
                .SSID         ="YHtest",
                .PWD          ="qwertyuiop",
                },

};
//定义接收data_block 并且初始化
DATA_BLOCK g_data_block  = {
        .n_data_block    = 1,
        .rec_buf         = {'\0'},
        .upload_to_cloud = 0,//上传云端是否成功
};

void InitDataBlock()
{
    g_data_block.n_data_block    = NUM;
    g_data_block.upload_to_cloud = 0;
    bzero(g_data_block.rec_buf, sizeof(g_data_block.rec_buf));
    return;
}
void InitStatus()
{
    InitDataBlock();
    g_status.state = 0;//
    g_status.time_sync_state = 0;//
    g_status.once_data_sync_state = 0;

    g_status.update_time[0] = '0';g_status.update_time[1] = '0';g_status.update_time[2] = '0';g_status.update_time[3] = '0';
    g_status.is_ready = '1';//盒子的状态0 ready ，1 busy

    return;
}



#endif //SERIAL_V_2_0_INIT_H
