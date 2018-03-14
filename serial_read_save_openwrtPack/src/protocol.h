//
// Created by yanghao on 18-1-23.
// 通信协议
//

#ifndef SERIALPROJECT_PROTOCOL_H
#define SERIALPROJECT_PROTOCOL_H

#include <stdint.h>
#include <string.h>
#include "data.h"
#include "dir.h"
#include <strings.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include<stdlib.h>
#include <math.h>

#define WAIT_TIME_RESTART 10
#define FRAME_HEAD_LENGTH 2
#define FRAME_TAIL_LENGTH 2
#define SSID_LENGTH 32
#define PWD_LENGTH 32
#define DEVICE_NAME_LENGTH 12
#define DEVICE_SN_LENGTH 12
#define DEVICE_MACID_LENGTH 6
#define USER_ID_LENGTH 12
#define USER_NAME_LENGTH 16
#define CHAR_STOP_BITS 2
//// \r\n 不同的系统下不一样需要修改
#define NUM 4//一次发送几个block
#define NUM_C "4"
//#define test 1
//第一个命令
char FRAME_HEAD[] = "H3";
char FRAME_TAIL[] = "5A";

const char REUQEST_01_ISREADY[]     = "H301005A";//第一个命令：询问是否可以同步数据的指令
const char REUQEST_10_NET_CONFIG[]  = "H30A005A";//第二个命令：请求读取盒子的网络配置的指令
const char REUQEST_11_DEVICE_INFO[] = "H30B005A";//第三个命令：请求读取盒子的设备信息的指令
const char REUQEST_12_USER_INFO[]   = "H30C005A";//第四个命令：请求读取与盒子绑定用户信息的指令
//to change
char REUQEST_13_SYNC_TIME[]  = "H30D00005A";//第五个命令：请求同步时间
char REUQEST_14_SYNC_DATA[10] = "";//第六个命令：请求同步离线数据,变化的
char REUQEST_15_STATUS[10] = "";//第七个命令：询问盒子同步状态

//const char REQUEST_NULL[8]           = "H300005A";
//const char REQUEST_ERROR[8]          = "H3ffff5A";
/**
 * @brief  字符串转转整数
*/
int CharToInt(const char *p)
{
    return atoi(p);
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
/**
 * @brief  判断串口收到到消息是否合法
 * @return bool
*/
bool AnswerIsLegal(const char *rec_buf)
{
    //这里认为包头是前两个字节，包尾是后两个字节 不算换行符
    int length = strlen(rec_buf);
    //memcpy(*数组1，* 数组2，要赋值的大小)
    char buf_head[FRAME_HEAD_LENGTH+1];
    char buf_tail[FRAME_TAIL_LENGTH+1];
    for(int i = 0;i < FRAME_TAIL_LENGTH; ++i )
    {
        buf_tail[i] = rec_buf[length - FRAME_TAIL_LENGTH - CHAR_STOP_BITS + i];
    }
    buf_tail[FRAME_TAIL_LENGTH] = '\0';
    memccpy(buf_head, rec_buf,0, FRAME_HEAD_LENGTH);

    if(strcmp(buf_head,FRAME_HEAD) == 0) //开始两个字节必须相等
    {
        if (strcmp(buf_tail, FRAME_TAIL) == 0)
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
bool HandleAnswer01Isready(char *rec_buf,const int fd)
{
    memset(rec_buf,0, sizeof(rec_buf));
    //int read_result = SerialRead(fd, rec_buf, MAXSIZE, WAIT_TIME_RECV);
    int read_result = SerialRead(fd, rec_buf, MAXSIZE, WAIT_TIME_RECV);
    printf("the serial receieve buf :%s", rec_buf);
    if ( read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(rec_buf))
        {
            status.is_ready = rec_buf[FRAME_HEAD_LENGTH];//0 or 1
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
bool HandleAnswer10NetConfig(char *rec_buf,const int fd)
{
    memset(rec_buf,0, sizeof(rec_buf));
    int read_result = SerialRead(fd, rec_buf, MAXSIZE, WAIT_TIME_RECV);
    printf("the serial receive buf :%s\n", rec_buf);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(rec_buf))
        {
            /*
            for (int i = FRAME_HEAD_LENGTH,j = 0; i < (FRAME_HEAD_LENGTH+SSID_LENGTH) && j< 32; ++i,++ j)
            {
                status.net_config.SSID[j] = rec_buf[i];
            }
            for (int i = (FRAME_HEAD_LENGTH + SSID_LENGTH),j = 0; i < (FRAME_HEAD_LENGTH+SSID_LENGTH+PWD_LENGTH) && j<32; ++i,++ j)
            {
                status.net_config.PWD[j] = rec_buf[i];
            }
             */
            strncpy( status.net_config.SSID ,rec_buf+FRAME_HEAD_LENGTH,SSID_LENGTH);
            strncpy(status.net_config.PWD ,rec_buf+FRAME_HEAD_LENGTH+SSID_LENGTH,PWD_LENGTH);
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
 * @brief  处理11指令收到的消息，读取设备信息
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer11DeviceInfo(char *rec_buf, const int fd)
{
    memset(rec_buf,0, sizeof(rec_buf));
    int read_result = SerialRead(fd, rec_buf, MAXSIZE, WAIT_TIME_RECV);
    printf("the serial receieve buf :%s\n", rec_buf);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(rec_buf))
        {
            strncpy(status.device_info.name,rec_buf+FRAME_HEAD_LENGTH,DEVICE_NAME_LENGTH);
            strncpy(status.device_info.serial_no,rec_buf + FRAME_HEAD_LENGTH + DEVICE_NAME_LENGTH,
                    DEVICE_SN_LENGTH);
            strncpy(status.device_info.mac_id,rec_buf + FRAME_HEAD_LENGTH + DEVICE_NAME_LENGTH + DEVICE_SN_LENGTH,
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
    puts(status.device_info.name);
    puts(status.device_info.serial_no);
    puts(status.device_info.mac_id);
    return true;
}
/**
 * @brief  处理12指令收到的消息，读取绑定用户信息
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer12UserInfo( char *rec_buf, const int fd)
{
    memset(rec_buf,0, sizeof(rec_buf));
    int read_result = SerialRead(fd, rec_buf, MAXSIZE, WAIT_TIME_RECV);
    printf("the serial receieve buf :%s\n", rec_buf);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(rec_buf))
        {
            strncpy(status.user_bind_info.user_id,rec_buf+FRAME_HEAD_LENGTH,USER_ID_LENGTH);//2 ,12
            strncpy(status.user_bind_info.user_name,rec_buf + FRAME_HEAD_LENGTH + USER_ID_LENGTH,USER_NAME_LENGTH);//2+12,16 长度
            status.user_bind_info.bind = rec_buf[FRAME_HEAD_LENGTH + USER_ID_LENGTH + USER_NAME_LENGTH];//2 12 16,
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
    printf("UID:%s\n",status.user_bind_info.user_id);
    printf("Uname:%s\n",status.user_bind_info.user_name);
    printf("bind:%c\n",status.user_bind_info.bind);
    return true;
}
////TODO
/**
 * @brief  处理13指令收到的消息，询问同步时间的信息
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer13SysncTime(char *rec_buf, const int fd)//需要多次发送请求，同步时间
{
    memset(rec_buf,0, sizeof(rec_buf));
    int read_result = SerialRead(fd, rec_buf, MAXSIZE, WAIT_TIME_RECV);
    printf("the serial receive buf :%s\n", rec_buf);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(rec_buf))
        {
            status.time_sync_state = rec_buf[FRAME_HEAD_LENGTH];
            if(status.time_sync_state == '2')//字符的比较而不是字符串的比较，只有回答已经同步才更新
            {
                strncpy(status.update_time,rec_buf + 1 + FRAME_HEAD_LENGTH,4 );
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
    return true;
}
//受到同步的命令,rec_buf变成
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
 * @brief  处理15指令收到的消息，读取盒子发送状态
 * @param fd 类型 int 串口句柄
 * @return bool
*/
bool HandleAnswer15Status(char *rec_buf, const int fd)//需要多次发送请求，同步时间
{
    memset(rec_buf,0, sizeof(rec_buf));
    int read_result = SerialRead(fd, rec_buf, MAXSIZE, WAIT_TIME_RECV);
    printf("the answer 15 receive buf :%s\n", rec_buf);
    if (read_result > 0)//正常读取到数据
    {
        if(AnswerIsLegal(rec_buf))
        {
            //状态
            status.state = rec_buf[FRAME_HEAD_LENGTH];
            //剩余到blocks数量
            char remain_blocks_c[3+1]={'\0'};//为什么不能用3
            strncpy(remain_blocks_c,rec_buf + FRAME_HEAD_LENGTH + 1,3);
            //status.remain_blocks = CharToInt(remain_blocks_c);
            status.remain_blocks = HexToInt(remain_blocks_c);
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
 * @brief  发送14命令
 * @param  fd 串口句柄
 * @return bool
*/
bool Send14Command(const int fd)
{
    char nblock_address[3] ={'\0'};
    IntTo3ByteHex(status.ndata_blocks,nblock_address);
    //指令从第几个block发送nblock_address，1000
    if ((int)strlen(nblock_address) == 3)//保证命令正确
    {
        memset(REUQEST_14_SYNC_DATA, 0, sizeof(REUQEST_14_SYNC_DATA));
        strcpy(REUQEST_14_SYNC_DATA, "H30D");
        MergeString3(REUQEST_14_SYNC_DATA, nblock_address, NUM_C, FRAME_TAIL);
        puts("code 14 buf :");
        SerialCommand(fd, REUQEST_14_SYNC_DATA);////master下达询问同步状态的指令
        return true;
    } else
    {
        puts("SYNC ERROR");
        return  false;
    }
}
/**
 * @brief  发送15命令
 * @param  fd 串口句柄
 * @return bool
*/
bool Send15Command(const int fd)
{
    char nblock_address[3] ={'\0'};//当前已经同步了多少个blocks
    IntTo3ByteHex(status.ndata_blocks,nblock_address);

    //TransBlockNumTo3Char(nblock_address,status.ndata_blocks);
    //指令从第几个block发送nblock_address，1000
    if ((int)strlen(nblock_address) == 3)//保证命令正确
    {
        printf("Master have received %s blocks\n",nblock_address);
        memset(REUQEST_15_STATUS,0, sizeof(REUQEST_15_STATUS));
        strcpy(REUQEST_15_STATUS,"H30F");
        ////TODO表示上一次同步状态，0代表成功，1代表失败，默认成功
        char state[2] = "1";
        MergeString3(REUQEST_15_STATUS,nblock_address,state,FRAME_TAIL);
        SerialCommand(fd,REUQEST_15_STATUS);////master下达询问同步状态的指令
        return true;
    }
    else
    {
        puts("SYSNC ERROR");
        return  false;
    }
}
/**
 * @brief 循环读取一个或者多个block
 * @param fd 串口的句柄
 * @return 读取到字节数,-1 代表传输错误，0代表不合法
 */
int HandleAnswer14SysncData2(const int fd)
{
    data_block.rec_buf;
    int read_result = SerialReadDataBlock(fd, data_block.rec_buf, WAIT_TIME_RECV);
    printf("data block have %d Bytes\n", read_result);
    if(read_result >= 0)
    {
        if(AnswerIsLegal(data_block.rec_buf))
        {
            //TODO 加入文件存储？
            status.time_sync_state = '1';//表示读入内存成功
            status.ndata_blocks += NUM;//表示已经接收了NUM个blocks
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
    return read_result;
}
/**
 * @brief  同步block数据
 * @param  rec_buf 收到到消息
 * @return 1 成功 -1 失败
*/
int SyncDataProcess(const int fd)//心电盒子上传数据到操作，返回值-1，表示出错需要从来，1表示成功
{
    /**循环1：控制 data block 的发送和接收，命令15询问还剩多少个block没有发送，直到只剩下0个的时候停止接收*/
    while(true)
    {
        if(Send15Command(fd)== false) break;//发送命令询问盒子的装状态和还有多少个block才能同步完
        char status_buf[MAXSIZE]={'\0'};//读取的data blocks的数量
        printf("\nPlease input 15 buf :\n");
        if(!HandleAnswer15Status(status_buf,fd)) return -1;//回到错误从新开始

        if(status.remain_blocks == 0)
        {
            printf("remain blocks is 0 ,updated finished");
            return 0;
        }
        /***********************************************************************************************/
        int i = 0;
        while (true)//循环2：控制一次data block的发送，如果成功就ok，失败会重发3次，否则就失败
        {
            //修改命令参数
            ++i;
            if(Send14Command(fd) == false) return -1;//发送命令失败，整体退出
            int handle14 = HandleAnswer14SysncData2(fd);
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
                SaveDataBlocksFile(&data_block,status.user_bind_info.user_name);
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
