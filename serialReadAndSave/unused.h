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
 * @brief  把整数转化成16进制到字符
 * @param aa 类型 int 输入整数
 * @param buffer 类型 char 输出的16进制字符
 * @return buffer
*/
char * IntToHex(int aa,char *buffer)//int <4095
{
    if(aa >= 4095)
    {
        perror("input int is extend 4095!");
        return NULL;
    }
    static int i = 0;
    if (aa < 16)            //递归结束条件
    {
        if (aa < 10)        //当前数转换成字符放入字符串
            buffer[i] = aa + '0';
        else
            buffer[i] = aa - 10 + 'A';
        buffer[i+1] = '\0'; //字符串结束标志
    }
    else
    {
        IntToHex(aa / 16,buffer);  //递归调用
        i++;                //字符串索引+1
        aa %= 16;           //计算当前值
        if (aa < 10)        //当前数转换成字符放入字符串
            buffer[i] = aa + '0';
        else
            buffer[i] = aa - 10 + 'A';
    }
    return (buffer);
}
/**
 * @brief  将16进制到字符串，格式化为3字节的字符串，如F 00F
 * @param input_hex 类型 char 输入16进制字符串
 * @param output 类型 char 格式化输出
 * @return output
*/
char *FormatTo3ByteHex( char *input_hex,char *output)
{
    int len = strlen(input_hex);
    switch (len){
        case 1:
            MergeString2(output,"00",input_hex);
            break;
        case 2:
            MergeString2(output,"0",input_hex);
            break;
        case 3:
            strcpy(output,input_hex);
            break;
        default:
            perror("error in FormatTo3Byte");
            break;
    }
    return output;
}
/**
 * @brief  把整数转化成16进制到字符，并且格式化输入 如：15 --> 00F
 * @param   input 类型 int 输入整数
 * @param   output 类型 char 格式化输出的16进制字符
 * @return  output
*/
char *IntTo3ByteHex(int input,char *output)
{
    char buffer[4]="\0";
    IntToHex(input,buffer);
    char final[3] = "\0";
    FormatTo3ByteHex(buffer,final);
    strcpy(output,final);
    return output;
}

/**
 * @brief  16进制到字符串转化为10进制的整数
 * @param   hex 类型 char 输入的16进制字符串
 * @return  int 10进制整数
*/
int HexToInt(char *hex)
{
    int len = strlen(hex);
    int power_index = 0; //幂级数
    int sum = 0;
    if (len-1<0) return 0;
    for (int i = len - 1 ; i >= 0; -- i, ++ power_index)//反向遍历字符串
    {
        char temp = hex[i];

        int mark1 = temp - '0';
        int mark2 = temp - 'A';
        if (mark1 >= 0 && mark1 < 10)//如果是小于10的整数
        {
            sum += mark1 * pow(16, power_index);
            continue;
        }
        if (mark2 >= 0 && mark2 <= 5)//如果是大于10的整数
        {
            sum += (mark2 + 10) * pow(16,power_index);
            continue;
        }
        else
        {
            perror("not a hex number!");
            return 0;
        }
    }

    return sum;
}
/**
 * @brief  整数转字符串，保证3个字节
*/
char * IntToString(char *des,int n)//整数转换成字符串的函数
{
    sprintf(des, "%d", n);
    return des;
}
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
