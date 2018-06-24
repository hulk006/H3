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
#include <netdb.h>

#include "data.h"
#include "dir.h"
#include "serial.h"
#include "config.h"
#include "analyze_head.h"
#include <arpa/inet.h>


bool HandleAnswer01Isready(const int fd, const int answer_length)
{
    unsigned char isready_buf[10]={'\0'};
    int read_result = SerialRead(fd,isready_buf,answer_length, WAIT_TIME_RECV);
    if ( read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(isready_buf,answer_length))
        {
            g_status.is_ready = isready_buf[FRAME_HEAD_LENGTH];//0 or 1
            if(g_status.is_ready == '\0')
                return true;
            else
                return false;
        }
        else
        {
            //SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        //SerialClose(fd,WAIT_TIME_RESTART);
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
            memset(g_status.net_config.SSID,0, sizeof(g_status.net_config.SSID));
            memset(g_status.net_config.PWD,0, sizeof(g_status.net_config.PWD));
            strncpy(g_status.net_config.SSID ,net_buf+FRAME_HEAD_LENGTH,SSID_LENGTH);
            strncpy(g_status.net_config.PWD ,net_buf+FRAME_HEAD_LENGTH+SSID_LENGTH,PWD_LENGTH);

        }
        else
        {
            //SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        //SerialClose(fd,WAIT_TIME_RESTART);
        return false;
    }
    printf("\nwifiSSID:%s\npassword:%s\n",g_status.net_config.SSID,g_status.net_config.PWD);
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
            memset( g_status.device_info.name,0,DEVICE_NAME_LENGTH);
            for (int i = 0; i < DEVICE_NAME_LENGTH; ++i)
            {
                g_status.device_info.name[i]=device_buf[FRAME_HEAD_LENGTH + i];
            }
            for (int i = 0; i < DEVICE_MACID_LENGTH; ++i)
            {
                g_status.device_info.mac_id[i]=device_buf[FRAME_HEAD_LENGTH + DEVICE_NAME_LENGTH+ i];
            }
            GetDeviceID(g_status.device_info.mac_id, g_status.device_info.device_num,6);

        }
        else
        {
            //SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        //SerialClose(fd,WAIT_TIME_RESTART);
        return false;
    }

    printf("\ndevice info is:%s,%d",g_status.device_info.name,strlen(g_status.device_info.name));
    printf("mac_id:");
    for (int i = 0; i < DEVICE_MACID_LENGTH; ++i) {
        printf("%x ",g_status.device_info.mac_id[i]);
    }
    printf("\n");
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
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(user_buf,answer_length))
        {
            g_status.user_bind_info.bind = user_buf[FRAME_HEAD_LENGTH + USER_ID_LENGTH];//2 12 16,
            if(g_status.user_bind_info.bind == 1)
            {
                memset(g_status.user_bind_info.user_id,0, sizeof(g_status.user_bind_info.user_id));
                for (int i = 0; i < USER_ID_LENGTH; ++i)
                {
                    g_status.user_bind_info.user_id[i]=user_buf[FRAME_HEAD_LENGTH + i];
                }
                printf("%s\n",g_status.user_bind_info.user_id);
            }
            else
            {
                puts("a unbind user id ,please bind ,then upload ...");
                ////如果未绑定，不需要同步云端，给一个固定的UID,unbinduser
                memset(g_status.net_config.SSID,0, sizeof(g_status.net_config.SSID));
                memset(g_status.net_config.PWD,0, sizeof(g_status.net_config.PWD));
                char *ssid="yhtest";
                char *key="123456789";
                strncpy(g_status.net_config.SSID ,ssid,SSID_LENGTH);
                strncpy(g_status.net_config.PWD ,key,PWD_LENGTH);
            }
        }
        else
        {
            //SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        //SerialClose(fd,WAIT_TIME_RESTART);
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
            g_status.time_sync_state = time_buf[FRAME_HEAD_LENGTH];
            if(g_status.time_sync_state == 2)//只有回答已经同步才更新
            {
                strncpy(g_status.update_time,time_buf + 1 + FRAME_HEAD_LENGTH,4 );
            }
            else //其他情况失败从新来
            {
                puts("answer 13 :response is not 2");
                return false;
            }

        }
        else
        {
            //SerialClose(fd,WAIT_TIME_RESTART);
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        //SerialClose(fd,WAIT_TIME_RESTART);
        return false;
    }
    printf("reponse:%d\n",g_status.time_sync_state);
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
    unsigned char remain_buf[8]={'\0'};
    int read_result = SerialRead(fd, remain_buf, answer_length, WAIT_TIME_RECV);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(remain_buf,answer_length))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}


/**
 * @brief
 * @param  fd 串口句柄
 * @return bool
*/
bool Send13Command(const int fd)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);

    long int time = tv.tv_sec;
    unsigned char *p = (&time);
    for (int i = 0; i < 4; ++i)
    {
        REUQEST_13_SYNC_TIME[i+4] = *(p+i);
    }
    SerialCommand(fd,REUQEST_13_SYNC_TIME,10);
    printf("command 13 time: %ld\n",time);
    return true;
}

/**
 * @brief  发送14命令,告诉盒子发送道起始地址ADRESS 16进制的数
 * @param  fd 串口句柄
 * @return bool
*/
bool Send14ReadEcgCommand(const int fd, const int block_id)
{
    int n=0;//16进制转为整数
    unsigned char command_14_buf[10]={'H','3',0x14,0x00,0x00,0x00,0x00,0x01,'5','A'};
    int ADDRESS = block_id;
    unsigned char *p = (&ADDRESS);
    for (int i = 0; i < 3; ++i)
    {
        command_14_buf[i+4] = *(p+i);
    }
    SerialCommand(fd,command_14_buf,10);
    printf("command 14 ：send %d data blocks\n", ADDRESS);
    return true;
}
/**
 * @brief  发送14命令,读取flash数据头 16进制的数
 * @param  fd 串口句柄
 * @return bool
*/
bool Send14ReadHeadCommand(const int fd)
{
    unsigned char command_14_buf[10]={'H','3',0x14,0x00,0x00,0x00,0x00,0x01,'5','A'};
    SerialCommand(fd,command_14_buf,10);
    return true;
}

/**
 * @brief  发送15命令,询问盒子还剩多少个block没发送完
 * @param  fd 串口句柄
 * @return bool
*/
bool Send15Command(const int fd, const int block_id)
{
    unsigned char command_15_buf[10]={'H','3',0x15,0x00,0x00,0x00,0x00,0x01,'5','A'};
    int block_adress = block_id;
    unsigned char *p = (&block_adress);
    for (int i = 0; i < 3; ++i)
    {
        command_15_buf[i+4] = *(p+i);
    }
    SerialCommand(fd,command_15_buf,10);
    printf("command 15：have read %d block\n",block_adress);
    return true;
}
/**
 * @brief  发送15命令,询问盒子还剩多少个block没发送完
 * @param  fd 串口句柄
 * @return bool
*/
bool Send15SyncFinishedCommand(const int fd)
{
    unsigned char command_15_buf[10]={'H','3',0x15,0x00,0xff,0xff,0xff,0x01,'5','A'};
    SerialCommand(fd,command_15_buf,10);
    printf("*********************command 15：finished sync***********************************************");
    return true;
}


/**
 * @brief  发送16命令,读取登录的设备地址
 * @param  fd 串口句柄
 * @return bool
*/
bool HandleAnswer16ServerIP(const int fd, const int answer_length)
{
    unsigned char server[5]={'\0'};
    int read_result = SerialRead(fd, server, answer_length, WAIT_TIME_RECV);

    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(server,answer_length))
        {
            switch (server[2])
            {
                case 0x01:
                    strncpy(g_status.server_ip,"pro", sizeof(g_status.server_ip));
                    break;
                case 0x02:
                    strncpy(g_status.server_ip,"staging", sizeof(g_status.server_ip));
                    break;
                case 0x03:
                    strncpy(g_status.server_ip,"dev", sizeof(g_status.server_ip));
                    break;
                default:
                    strncpy(g_status.server_ip,"pro", sizeof(g_status.server_ip));
                    break;
            }


        }
        else
        {
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {

        return false;
    }
    printf("device login:%s",g_status.server_ip);
    return true;
}
/**
 * @brief  发送17命令,读取登录的secret key
 * @param  fd 串口句柄
 * @return bool
*/
bool HandleAnswer17Key(const int fd, const int answer_length)
{
    unsigned char key_buff[20]={'\0'};
    int read_result = SerialRead(fd, key_buff, answer_length, WAIT_TIME_RECV);


    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(key_buff,answer_length))
        {
            for (int i = 0; i < 16; ++i)
            {
                g_status.secret_key[i] = key_buff[i+2];
            }
        }
        else
        {
            return false;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {

        return false;
    }
    printf("KEY:%s",g_status.secret_key);
    return true;
}



/**
 * @brief 循环读取一个或者多个block
 * @param fd 串口的句柄
 * @return 读取到字节数,-1 代表传输错误，0代表不合法
 */
int HandleAnswer14ReadEcgData(const int fd)
{
    /**每次处理串口数据之前都清空内存的数据**/
    size_t rec_buf_size = sizeof(g_data_block.rec_buf);
    memset(g_data_block.rec_buf,'\0', rec_buf_size);
    int read_result = SerialReadEcgDataBlock(fd,g_data_block.rec_buf,rec_buf_size, WAIT_TIME_RECV);
    printf("data block have %d Bytes\n", read_result);
    if(read_result >= 0)
    {
        if(AnswerIsLegal(g_data_block.rec_buf,read_result))
        {
            g_status.time_sync_state = 1;//表示读入内存成功
        }
        else
        {
            printf("ecg data block is not legal\n");
            return 0;
        }
    }
    else
    {
        return -1;
    }
    return read_result;
}

char* GetOldInfo(char *p)
{
    char head_file_name[100]={'\0'};
#if debug
    MergeString3(head_file_name,"/home/yh/user_data/",g_status.device_info.device_num, "/head_file.txt");
#else
    MergeString3(head_file_name,"/root/user_data/",g_status.device_info.device_num,"/head_file.txt");
#endif
    FILE *fp;
    fp = fopen(head_file_name,"r");
    if (fp == NULL)
    {
        printf("head_file %s not",head_file_name);
        return NULL;
    }
    char str[400] = {'\0'};
    char txt[400] = {'\0'};
    fseek(fp,0,SEEK_END);
    rewind(fp);
    while((fgets(txt,400,fp))!=NULL){
        strcat(str,txt);
        strcat(p,str);
    }
    fclose(fp);
    return p;
}


int CheckWifiConnect(char *host_name)
{
    /**input 0**/
    if(g_status.net_config.SSID[0]=='0' && g_status.net_config.PWD[0]=='0')
    {
        system("wifi reload");
        //direct judge
        int tmp = -1;
        for(int i=0;i<3;i++)
        {
            tmp = system("ping  -c1 www.baidu.com");

            if (tmp == 0)
            {
                return 1;
            }
        }
        return 0; //failed
    }

    /** input not 0******/
    char* all_info_list[7];
    char str_info[500]={'\0'};
    GetOldInfo(str_info);
    if(strlen(str_info) == 0)//head_file.txt not exit!
    {
        printf("a new user\n");
        char neconfig[200]={"\0"};
        MergeString4(neconfig,"sh /root/foropenwrt/netconfig.sh ",g_status.net_config.SSID," ",g_status.net_config.PWD);
        system(neconfig);
        sleep(20);
        //direct judge
        int tmp = -1;
        for(int i=0;i<3;i++)
        {
            tmp = system("ping  -c1 www.baidu.com");
            if (tmp == 0)
            {
                return 1;
            }
        }
        return 0; //failed
    }

    char del1[] = ",";
    Split(all_info_list, str_info, del1);

    char *old_wifi_ssid[2];
    char *old_wifi_pwd[2];
    char del2[]=":";
    Split(old_wifi_ssid, all_info_list[4], del2);
    Split(old_wifi_pwd, all_info_list[5], del2);
    char ssid_old[32]={'\0'},pwd_old[32]={'\0'};

    strncpy(ssid_old, old_wifi_ssid[1]+1, strlen(old_wifi_ssid[1])-2);
    strncpy(pwd_old, old_wifi_pwd[1]+1, strlen(old_wifi_pwd[1])-2);
    //compare ssid
    printf("%s=%s",ssid_old,g_status.net_config.SSID);
    printf("%s=%s",pwd_old,g_status.net_config.PWD);
    if(strcmp(ssid_old, g_status.net_config.SSID) == 0 && strcmp(pwd_old, g_status.net_config.PWD) == 0)//same
    {
        //direct judge
        int tmp = -1;
        for(int i = 0 ;i < 3 ;i ++)
        {
            tmp = system("ping  -c1 www.baidu.com");
            if (tmp == 0)
            {
                return 1;
            }
        }
        char neconfig[200]={"\0"};
        MergeString4(neconfig,"sh /root/foropenwrt/netconfig.sh ",g_status.net_config.SSID," ",g_status.net_config.PWD);
        system(neconfig);
        sleep(20);
        for(int i = 0 ;i < 3 ;i ++)
        {
            tmp = system("ping  -c1 www.baidu.com");
            if (tmp == 0)
            {
                return 1;
            }
        }
        return 0; //failed
    }
    else//new wifi
    {
        printf("new wifi!");
        char neconfig[200]={"\0"};
        MergeString4(neconfig,"sh /root/foropenwrt/netconfig.sh ",g_status.net_config.SSID," ",g_status.net_config.PWD);
        system(neconfig);
        sleep(20);
        //ConfigWIFI();
        int tmp = -1;
        for(int i=0;i<3;i++)
        {
            tmp = system("ping -c1 www.baidu.com");
            if (tmp == 0)
            {
                return 1;
            }
        }
        return 0;//failed
    }

}

void GetLocalTime()
{
    struct timeval tv;
    char time_str[128];
    gettimeofday(&tv, NULL);//时间初始化
    struct tm *now_time = localtime(&tv.tv_sec);//s
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", now_time);
    printf ("now time:%s\n",time_str);
}

bool ReadDataHead(const int fd)
{
    int read_number = 0;
    memset(g_data_block.rec_buf,'\0', sizeof(g_data_block.rec_buf));
    if (!Send14ReadHeadCommand(fd)) return false;//如果发送命令失败
    read_number = SerialReadDataHead(fd, g_data_block.rec_buf, g_DATA_HEAD_LENGTH, 20);
    printf("data head have %d Bytes\n", read_number);

    if(read_number >= 0)
    {
        if(AnswerIsLegal(g_data_block.rec_buf,read_number))//合法
        {
            //TODO 加入文件存储？
            SaveDataHeadFile(&g_data_block,g_status.device_info.device_num);
            return true;

        }
        else//非法
        {
            printf("data head is not legal!");
            return false;
        }
    }
    else
    {
        return false;
    }
}

//TODO
bool ReadEcgDataBlock(const int fd)
{
    //没有读取到最后一个
   while (g_HAVE_READ_ECG_NUMBER != g_NEED_READ_ECG_NUMBER)
   {
       /*****************判断缓存文件的大小******************************/
       char user_dir[100]={'\0'};
       MergeString3(user_dir,WORKING_DIR,g_status.device_info.device_num,"/");
       float user_dir_size = GetDirSize(user_dir);

       printf("have read %d ecg block,user_dir_size %1f MB\n",g_HAVE_READ_ECG_NUMBER,user_dir_size);
       printf("read %d\n",g_need_read_ecg_block_info[g_HAVE_READ_ECG_NUMBER].block_id + 1);

       if(user_dir_size > g_DIR_MAX_SIZE
          ||Send14ReadEcgCommand(fd,g_need_read_ecg_block_info[g_HAVE_READ_ECG_NUMBER].block_id + 1) == false)
       {
           printf("user_dir_size is big than %d\n",g_DIR_MAX_SIZE);
           return false;//网络不正常就报警
       }

       int handle14 = HandleAnswer14ReadEcgData(fd);//串口读取心电数据

       if(handle14 <= 0)//此次数据接收  格式不对，直接退出数据同步程序
       {
           printf("failed in read ecg data quit\n");
           return false;
       }
       else
       {

           Send15Command(fd,g_need_read_ecg_block_info[g_HAVE_READ_ECG_NUMBER].block_id + 1);//发送命令确认收到
           if(HandleAnswer15Status(fd, ANSWER_15_LENGTH))//两次握手成功之后才存文件
           {
               printf("hands shake success");
               SaveEcgDataBlocksFile(&g_data_block, g_status.device_info.device_num);
               g_HAVE_READ_ECG_NUMBER ++;
           }

           else
           {
               printf("hands shake failed\n");
               return false;
           }
       }
   }
    //完全上传成功
    Send15SyncFinishedCommand(fd);
    SaveSyncStatusSucess();
    return true;
}

/**
 * @brief just for delete data
 * @param fd
 * @param start adress
 * @param end
 * @return
 */
bool DeleteDataBloack(const int fd,int start,int end)
{
    unsigned char command_15_buf[10]={'H','3',0x15,0x00,0xff,0xff,0xff,0x01,'5','A'};
    int num=0;
    for(num = start;num < end;++num)
    {
        for (int i = 0; i < 3; ++i)
        {
            unsigned char *p = (&num);
            command_15_buf[i+4] = *(p+i);
        }
        SerialCommand(fd,command_15_buf,10);
        printf("command 15：ADDRESS %d can be delete",num);
        sleep(1);
    }
    return true;
}

#endif //SERIALPROJECT_PROTOCOL_H
