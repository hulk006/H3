//
// Created by yh on 6/4/18.
//

#ifndef SERIAL_V_2_0_UTILS_H
#define SERIAL_V_2_0_UTILS_H

/****************************************************TOOL FUNCTION*****************************************************/
#include <string.h>
#include "init.h"

/**
 * @brief  合并字符串
 * @return
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

void Split(char **arr, char *str, const char *del) {
    char *s = strtok(str, del);
    while(s != NULL) {
        *arr++ = s;
        s = strtok(NULL, del);
    }
}

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

bool CheckBaudeRate(const int speed)
{
    for (int i = 0; i < sizeof(g_baude_speed_arr); ++i)
    {
        if(speed == g_baude_speed_arr[i])
            return true;
    }
    return false;
}

/**
 * @func 把当前的时间转换成时间戳
 * @param
 */
void GetTimeCurrent(unsigned char * time_buf,int length)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);

    long int time = tv.tv_sec;
    printf("%x\n",time);
    unsigned char *p = (&time);
    for (int i = 0; i < 4; ++i)
    {
        time_buf[i+4] = *(p+i);
    }
}

/**
 * @func 修改文件的名称
 * @param old_name
 * @param new_name
 */


int CreatFile(const char *file_name,FILE *fp)
{
    const char *mode = "w+";
    fp = fopen(file_name,mode);//"w" 写 如果文件存在，把文件截断至0长；如果文件不存在，会创建文件
    if(fp != NULL)
        return 1;
    else
    {
        perror("CreatFile");
        return -1;
    }
}



#endif //SERIAL_V_2_0_UTILS_H
