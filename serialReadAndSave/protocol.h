//
// Created by yanghao on 18-1-23.
// 通信协议
//

#ifndef SERIALPROJECT_PROTOCOL_H
#define SERIALPROJECT_PROTOCOL_H
#include <string.h>
#include <strings.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include <time.h>
#include <math.h>

#include "data.h"
#include "dir.h"
#include "serial.h"
#include "config.h"
#include "analyze_head.h"

/**
 * @func 把16进制的字符转为字符串：如果小于10就转换成数字，如果大于10就转换为字母
 * @param des 含有数字和字母的输出
 * @param hex 16进制字符串
 * @param length 两个字符串的长度
 */
void HexToStr(char *des, const unsigned char *hex,const int length)
{
    for (int i = 0; i < length; ++i)
        des[i] = hex[i]>10 ? hex[i]:'0' + (int)hex[i];
    return;
}
//TODO
/**
 * @brief  判断串口收到到消息是否合法
 * @return bool
*/
bool AnswerIsLegal(const unsigned char *rec_buf, const int length)
{
    //这里认为包头是前两个字节，包尾是后两个字节 不算换行符
    char buf_head[FRAME_HEAD_LENGTH];
    char buf_tail[FRAME_TAIL_LENGTH];
    int a = length - FRAME_TAIL_LENGTH - CHAR_STOP_BITS + 0;
    int b = length - FRAME_TAIL_LENGTH - CHAR_STOP_BITS + 1;

    buf_tail[0]=rec_buf[a];
    buf_tail[1]=rec_buf[b];
    buf_head[0]=rec_buf[0];
    buf_head[1]=rec_buf[1];

    if(buf_head[0] == FRAME_HEAD[0] && buf_head[1] == FRAME_HEAD[1]) //开始两个字节必须相等
    {
        if (buf_tail[0] == FRAME_TAIL[0] && buf_tail[1] == FRAME_TAIL[1])
            return true;
        else
        {
            printf("receive buf is not legal or length beyond the %d limit ",MAXSIZE);
            return false;
        }
    }
    return false;
}
/**
 * @brief  处理01指令收到的消息，判断盒子是否准备好了
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer01Isready(const int fd, const int answer_length)
{
    unsigned char isready_buf[10]={'\0'};
    int read_result = SerialRead(fd,isready_buf,answer_length, WAIT_TIME_RECV);
    if ( read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(isready_buf,answer_length))
        {
            status.is_ready = isready_buf[FRAME_HEAD_LENGTH];//0 or 1
            if(status.is_ready == '\0')
                return true;
            else
                return false;
        }
        else
        {
            SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        SerialClose(fd,WAIT_TIME_RESTART);
        return false;
    }
    return true;
}
/**
 * @brief  处理10指令收到的消息，读取网络配置信息
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer10NetConfig(const int fd,int answer_length)
{
    unsigned char net_buf[70]={'\0'};
    int read_result = SerialRead(fd, net_buf,answer_length,WAIT_TIME_RECV);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(net_buf,answer_length))
        {
            memset(status.net_config.SSID,0, sizeof(status.net_config.SSID));
            memset(status.net_config.PWD,0, sizeof(status.net_config.PWD));
            strncpy(status.net_config.SSID ,net_buf+FRAME_HEAD_LENGTH,SSID_LENGTH);
            strncpy(status.net_config.PWD ,net_buf+FRAME_HEAD_LENGTH+SSID_LENGTH,PWD_LENGTH);
        }
        else
        {
            SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        SerialClose(fd,WAIT_TIME_RESTART);
        return false;
    }
    printf("\nwifiSSID:%s    password:%s",status.net_config.SSID,status.net_config.PWD);
    return true;
}
/**
 * @brief  处理11指令收到的消息，读取设备信息
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer11DeviceInfo( const int fd,const int answer_length)
{
    unsigned char device_buf[24]={'\0'};
    int read_result = SerialRead(fd, device_buf, answer_length, WAIT_TIME_RECV);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(device_buf,answer_length))
        {
            strncpy(status.device_info.name,device_buf+FRAME_HEAD_LENGTH,DEVICE_NAME_LENGTH);
            //strncpy(status.device_info.serial_no,device_buf + FRAME_HEAD_LENGTH + DEVICE_NAME_LENGTH,DEVICE_SN_LENGTH);
            strncpy(status.device_info.mac_id,device_buf + FRAME_HEAD_LENGTH + DEVICE_NAME_LENGTH ,
                    DEVICE_MACID_LENGTH);
        }
        else
        {
            SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        SerialClose(fd,WAIT_TIME_RESTART);
        return false;
    }
    puts("\ndevice info is:");
    puts(status.device_info.name);
    //puts(status.device_info.serial_no);
    puts(status.device_info.mac_id);
    return true;
}


/**
 * @brief  处理12指令收到的消息，读取绑定用户信息
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer12UserInfo( const int fd, const int answer_length )
{
    unsigned char user_buf[33]={'\0'};
    int read_result = SerialRead(fd, user_buf, answer_length, WAIT_TIME_RECV);
    unsigned char user_id[12]={0x00};
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(user_buf,answer_length))
        {
            status.user_bind_info.bind = user_buf[FRAME_HEAD_LENGTH + USER_ID_LENGTH];//2 12 16,
            if(status.user_bind_info.bind == 1)
            {
                printf("bind:%x\n",status.user_bind_info.bind);
                strncpy(user_id,user_buf+FRAME_HEAD_LENGTH,USER_ID_LENGTH);//2 ,12
                HexToStr(status.user_bind_info.user_id, user_id,answer_length);
                printf("UID:%s\n",status.user_bind_info.user_id);
            }
            else
            {
                puts("a unbind user id ,please bind ,then upload ...");
                ////如果未绑定，不需要同步云端，给一个固定的UID,unbinduser
            }
        }
        else
        {
            SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        SerialClose(fd,WAIT_TIME_RESTART);
        return false;
    }
    return true;
}

////TODO
/**
 * @brief  处理13指令收到的消息，询问同步时间的信息
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer13SysncTime(const int fd,const int answer_length)//需要多次发送请求，同步时间
{
    unsigned char time_buf[9]={'\0'};
    int read_result = SerialRead(fd, time_buf, answer_length, WAIT_TIME_RECV);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(time_buf,answer_length))
        {
            status.time_sync_state = time_buf[FRAME_HEAD_LENGTH];
            if(status.time_sync_state == 2)//只有回答已经同步才更新
            {
                strncpy(status.update_time,time_buf + 1 + FRAME_HEAD_LENGTH,4 );
            }
            else //其他情况失败从新来
            {
                puts("answer 13 :response is not 2");
                return false;
            }

        }
        else
        {
            SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        SerialClose(fd,WAIT_TIME_RESTART);
        return false;
    }
    printf("time:%s\n",status.update_time);
    printf("reponse:%d\n",status.time_sync_state);
    return true;
}
//受到同步的命令,rec_buf变成

/**
 * @brief  处理15指令收到的消息，读取盒子发送状态
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer15Status(const int fd, const int answer_length)//需要多次发送请求，同步时间
{
    //TODO 变量类型需要修改
    unsigned char remain_buf[8]={'\0'};
    //tcflush(fd, TCIOFLUSH);//清除串口缓存
    int read_result = SerialRead(fd, remain_buf, answer_length, WAIT_TIME_RECV);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(remain_buf,answer_length))
        {
            //状态
            status.state = remain_buf[FRAME_HEAD_LENGTH];
            //剩余到blocks数量
            int n = 0;//10进制表示
            for (int i = 0; i < 3; ++i)
            {
                status.n_remain_blocks[i] = remain_buf[FRAME_HEAD_LENGTH + i + 1];
                n = n + (status.n_remain_blocks[i]<<(i*8));
            }
            status.remain_blocks = n;

        }
        else
        {
            SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        SerialClose(fd,WAIT_TIME_RESTART);
        return false;
    }
    printf("盒子剩余%d data blocks", status.remain_blocks);
    return true;
}



/**
 * @brief  发送14命令,告诉盒子发送道起始地址status.n_blocks 16进制的数
 * @param  fd 串口句柄
 * @return bool
*/
bool Send14Command(const int fd)
{
    unsigned char command_14_buf[10]={'H','3',0x14,0x00,0x00,0x00,0x00,0x01,'5','A'};
    int n=0;//16进制转为整数
    for (int i = 0; i < 3; ++i)
    {
        command_14_buf[i+4] = status.n_blocks[i];
        n = n + (status.n_blocks[i]<<(i*8));
    }
    SerialCommand(fd,command_14_buf,10);
    printf("command 14 ：master have receive %d data blocks\n",n);
    return true;
}
/**
 * @brief  发送15命令,询问盒子还剩多少个block没发送完
 * @param  fd 串口句柄
 * @return bool
*/
bool Send15Command(const int fd)
{
    unsigned char command_15_buf[10]={'H','3',0x15,0x00,0x00,0x00,0x00,0x01,'5','A'};
    int n=0;//16进制转为整数
    for (int i = 0; i < 3; ++i)
    {
        command_15_buf[i+4] = status.n_blocks[i];//
        n = n + (status.n_blocks[i]<<(i*8));
    }
    SerialCommand(fd,command_15_buf,10);
    printf("command 15：received %d blocks,please tell me remain N blocks\n",n);
    return true;
}
/**
 * @brief 循环读取一个或者多个block
 * @param fd 串口的句柄
 * @return 读取到字节数,-1 代表传输错误，0代表不合法
 */
int HandleAnswer14SysncData(const int fd)
{
    /**每次处理串口数据之前都清空内存的数据**/
    size_t rec_buf_size = sizeof(data_block.rec_buf);
    memset(data_block.rec_buf,'\0', sizeof(data_block.rec_buf));
    int read_result = SerialReadDataBlock(fd,data_block.rec_buf,rec_buf_size, WAIT_TIME_RECV);
    printf("data block have %d Bytes\n", read_result);
    if(read_result >= 0)
    {
        if(AnswerIsLegal(data_block.rec_buf,read_result))
        {
            //TODO 加入文件存储？
            status.time_sync_state = 1;//表示读入内存成功
            ////把内存中的 几个 block 加上 当前接收到的block数量
            int  low_byte = status.n_blocks[0] + NUM_ONCE_BLOCKS;
            unsigned  char flag = 0x00;
            if (low_byte <= 255)
            {
                status.n_blocks[0] = status.n_blocks[0] + NUM_ONCE_BLOCKS;
            }
            else
            {
                status.n_blocks[0] = 0xFF;
                flag = 0x01;
            }

            for (int i = 1; i < 3; ++i)
            {
                if(status.n_blocks[i] + flag <= 255 )
                {
                    status.n_blocks[i]=status.n_blocks[i] + flag;
                    flag = 0x00;//进位标志为0
                }
                else
                {
                    status.n_blocks[i] = 0xFF;
                    flag = 0x01;//进位标志为0
                }
            }

            ////to 10 jin zhi
            status.ndata_blocks = 0;
            for (int i = 0; i < 3; ++i)
            {
                status.ndata_blocks  = status.ndata_blocks  + (status.n_blocks[i]<<(i*8));
            }
        }
        else
        {
            printf("data block is not legal");
            return 0;
        }
    }
    else
    {
        return -1;
    }
    printf("got the data block:total %d\n", status.ndata_blocks);
    return read_result;
}

/**
 * @brief  同步block数据
 * @param  rec_buf 收到到消息
 * @return 1 成功 -1 失败
*/
int SyncDataProcess(const int fd, const char *working_dir)//心电盒子上传数据到操作，返回值-1，表示出错需要从来，1表示成功
{
    /**循环1：控制 data block 的发送和接收，命令15询问还剩多少个block没有发送，直到只剩下0个的时候停止接收*/
    while(true)
    {
        /**
        if(Send15Command(fd)== false) break;//发送命令询问盒子的装状态和还有多少个block才能同步完

        printf("Please input 15 buf :\n");
        if(!HandleAnswer15Status(fd,8)) return -1;//回到错误从新开始
        */
        if(status.ndata_blocks >= status.total_blocks_num)
        {
            SaveSyncStatusSucess(status.user_bind_info.user_name);/**读取完成*/
            printf("remain blocks is 0 ,updated finished\n");
            return 0;
        }
        /***********************************************************************************************/
        int i = 0;
        while (true)//循环2：控制一次data block的发送，如果成功就ok，失败会重发3次，否则就失败
        {
            //修改命令参数
            ++i;
            if(Send14Command(fd) == false) return -1;//发送命令失败，整体退出
            int handle14 = HandleAnswer14SysncData(fd);

            if(handle14 == 0)//此次数据接收格式不对，直接退出数据同步程序
            {
                return -1;
            }
            else if (handle14 < 0 && i< 4)//此次数据接收失败，小于4次，重新执行循环2
            {
                continue;
            }
            else if(handle14 < 0 && i>= 4)//此次数据接收失败，大于4次，重新执行循环2
            {
                return -1;
            }
            else//此次数据正常接收，存储并且退出内层循环
            {
                SaveDataBlocksFile(&data_block,status.user_bind_info.user_id);

                if(status.ndata_blocks == 1)/**ndata_blocks初始化为0*/
                {
                    status.total_blocks_num = GetTotalBlocksNum(working_dir,status.user_bind_info.user_id);/**从第一block中读取总的blocks数量*/
                    printf("一共：%d blocks",status.total_blocks_num);
                }
                break;//一次数据同步结束，退出循环2
            }
        }
        /***一次数据同步结束后，命令15再次询问发送了多少个block，还剩多少个block****/
    }
    return 1;
}
//TODO...
//写文件头

#endif //SERIALPROJECT_PROTOCOL_H
