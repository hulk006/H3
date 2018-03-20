//
// Created by yh on 3/12/18.
//

#ifndef SERIALREADANDSAVE_UNUSED_H
#define SERIALREADANDSAVE_UNUSED_H


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

/**
 * @brief  把int型整数到的block数量，转化成3个字节到字符串
 * @param  des 返回的3字节字符从串
 * @param  m block数量
 * @return char *
*/
char* TransBlockNumTo3Char(char *des,int m)//将整数转化3字节的字符串,固定block数量为3个字节
{
    memset(des,0,3);
    if (m < 1000)
    {
        char s[3]={'\0'};
        IntToString(s,m);
        if(m < 10 && m > 0)
        {
            MergeString2(des,"00",s);
            return des;
        }
        else if(m >= 10 && m < 100)
        {
            MergeString2(des,"0",s);
            return des;
        }
        else if (m >= 100&& m < 1000)
        {
            strcpy(des,s);
            return des;
        }
        else if(m == 0)
        {
            des = "000";
            return des;
        }
    }
    else
        puts("nblocks>=1000");

    return des;
}


/**
 * @brief  求x的n次方
 * @param   x 类型 int 输入整数
 * @param   n 类型 int n次方
 * @return  int x^n
*/
int Power(const int x,const int n)//求x的n次方, x<100 n<4
{
    int a=1;
    for(int i = 1;i <= n;++ i)
        a *= x;
    return a;
}
/**
 * @brief  处理14指令收到的消息，同步串口一次收到信息
 * @param fd 类型 int 串口句柄
 * @param data_file 输出文件句柄
 * @return bool
*/
int HandleAnswer14SysncData( char *rec_buf,const int fd, FILE * data_file)
{
    memset(rec_buf,0, sizeof(rec_buf));
    int read_result = SerialRead(fd, rec_buf, MAXSIZE, WAIT_TIME_RECV);

    printf("the answer 14 receive data buf :%s\n", rec_buf);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(rec_buf))
        {
            fputs(rec_buf,data_file);
            status.once_data_sync_state ='1';//本次更新成功
            status.ndata_blocks = status.ndata_blocks + NUM;//更新受到到block的数量
            return NUM;
        }
        else
        {
            SerialClose(fd,WAIT_TIME_RESTART);
            return 0;
        }
    }
    else // 读取错误,读取超时 重新发送指令
    {
        SerialClose(fd,WAIT_TIME_RESTART);
        return 0;
    }
    return NUM;

}


/**
 * @brief  生成文件名，以用户名作为目录，以时间作为单个到文件名
 * @param file_name 一个空到字符串数组，存储文件名字
 * @return void
 * TODO
*/
void GenerateFilename(char *file_name)
{
    char file_dir_name[17]={'\0'} ;
    MergeString2(file_dir_name,status.user_bind_info.user_name,"/");//用户名作为目录
    CreatDir(file_dir_name);
    //获取时间当前时间
    struct timeval tv;
    char time_str[128];
    gettimeofday(&tv, NULL);//时间初始化
    struct tm *now_time = localtime(&tv.tv_sec);//s
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", now_time);//把时间结构体整理成字符串

    memset(file_name,0, sizeof(file_name));
    char *name_head = "H3DATA";
    char *postfix = ".dat";
    MergeString4(file_name,file_dir_name,name_head,time_str,postfix);//合并
    return ;
}
#endif //SERIALREADANDSAVE_UNUSED_H
