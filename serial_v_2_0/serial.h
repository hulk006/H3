//
// Created by Yanghao on 18-1-23.
// 串口到操作
//
#ifndef SERIALPROJECT_SERIAL_H
#define SERIALPROJECT_SERIAL_H

#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <memory.h>
#include <wait.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#define align32 0
#define MAXSIZE 1024
#define WAIT_TIME_RECV 20//未收到应答到等待时间

static struct termios newtios, oldtios; /*termianal settings */
/**
 * @brief  设置串口的延时，单位为ms，串口通信的效率低下
 * @param  ms     类型 int  延时的时间
 * @return  void
*/
void Sleep(int ms)
{
    struct timeval delay;
    delay.tv_sec = 0;
    delay.tv_usec = ms * 1000; // 20 ms
    select(0, NULL, NULL, NULL, &delay);
}

/**
 * @brief  超时等待读取串口的内容
 * @param  fd     类型 int  串口句柄
 * @param  p      char 串口收到的buffer
 * @param  desire_get_len int最大长度
 * @param  time_out int 串口等待时间
 * @return  读取buf的长度
*/
int SerialRead(const int fd,unsigned char p[],const int desire_get_len,const int time_out)
{
    struct timeval tv;
    tv.tv_sec = time_out;
    tv.tv_usec = 0;
    fd_set read_fds;
    int nread = 0 ;
    FD_ZERO(&read_fds);
    FD_SET(fd,&read_fds);
    puts("start serial select .......");
    if(select(fd+1,&read_fds,NULL,NULL,&tv))
    {
        Sleep(5);
        nread = read(fd,p, desire_get_len);
        if(nread<=0)
        {
            printf("read get problem!\r\n");
            return -1;
        }
    }
    else
    {
        perror("select():time out");
    }

    if(nread>0)
    {
        printf("receive buf:");
        for (int i = 0;i < nread;i ++)
        {
            printf("%x ", p[i]);
        }
        printf("\n");
    }

    return nread;
}

/**
 * @brief 读取数据头，读取数据头和读取ecg数据的策略不一样
 * @param fd 串口
 * @param p buff
 * @param desire_get_len 读取多长
 * @param time_out
 * @return 读取的长度
 */
int SerialReadDataHead(const int fd,unsigned char * p,const int desire_get_len,const int time_out)
{
    int nBytes = 0;
    unsigned char read_head_temp[2048];
    struct timeval tv;
    tv.tv_sec = time_out;
    tv.tv_usec = 0;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd,&read_fds);
    puts("select data head");
    //获取时间当前时间
    time_t t1;
    t1 = time(NULL);
    /**循环读取data head */
    while (select(fd+1,&read_fds,NULL,NULL,&tv))
    {
        Sleep(10);//ms
        memset(read_head_temp,'\0',2048);
        int nread = read(fd, read_head_temp, 2048);//一次最多读取2048
        int start =  nBytes;
        nBytes += nread;//一共读取了多少个字符
        int stop = nBytes;
        for(int i = start;i < stop;++i)
        {
            p[i] = read_head_temp[i - start];
        }
        if(nread == 0)
        {
            break;
        }
    }
    //获取时间当前时间
    time_t t2;
    t2 = time(NULL);
    //获取时间当前时间
    while (1)
    {
        Sleep(10);
        memset(read_head_temp,'\0',2048);
        int nread_0 = read(fd, read_head_temp, 2048);//一次最多读取2048字符
        int start_0 =  nBytes;
        if(nread_0>0)
        {
            nBytes += nread_0;//一共读取了多少个字符
        }
        int stop_0 = nBytes;
        for(int i = start_0;i < stop_0;++i)
        {
            p[i] = read_head_temp[i - start_0];
        }
        time_t t4;
        t4 = time(NULL);
        int flag = (int)(t4-t2);
        if((p[nBytes] == 'A'&&p[nBytes-1]=='5')||flag > 10)
        {
            break;
        }
    }
    time_t t3;
    t3 = time(NULL);
    printf("read head cost time = %ds ",(int)(t3 - t1));
    return nBytes;
}
/**
 * @brief  读取ecg data的数据
 * @param  fd     类型 int  串口句柄
 * @param  p      char 输出的字符串
 * @param  desire_get_len int最大长度
 * @param  time_out int 串口等待时间
 * @return  读取buf的长度
*/
int SerialReadEcgDataBlock(const int fd,unsigned char * p,const int desire_get_len,const int time_out)
{
    int nBytes = 0;
    int sleep_time = 5;//us
    unsigned char read_temp[1024];

    struct timeval tv;
    tv.tv_sec = time_out;
    tv.tv_usec = 0;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd,&read_fds);
    puts("select ecg block");
    //获取时间当前时间
    time_t t1;

    int not_read_num=0;
    if (select(fd+1,&read_fds,NULL,NULL,&tv))
    {

        t1 = time(NULL);
        /**循环读取data blocks */

        while (1)
        {

            Sleep(sleep_time);
            memset(read_temp,'\0', 1024);
            int nread = read(fd, read_temp, 1024);

            if(nread <= 0)
            {
                not_read_num++;
                if(not_read_num == 5)//50 ms 没有读取到数据
                {
                    printf("end");
                    break;
                }
            }
            else
            {
                not_read_num = 0;
                int start =  nBytes;
                nBytes += nread;//一共读取了多少个字符
                int stop = nBytes;
                for(int i = start;i < stop;++i)
                {
                    p[i] = read_temp[i - start];
                }

            }


        }
    }

    time_t t4;
    t4 = time(NULL);
    printf("ecg data cost time = %ds ",(int)(t4 - t1));
    return nBytes;
}

/**
 * @brief  串口发送命令到函数
 * @param  fd       类型 int  串口的句柄
 * @param  command  类型 char 命令码
 * @return int 1 发送命令成功 -1失败
*/
int SerialCommand(const int fd,const unsigned char *command,const int len)
{
#if align32
    unsigned  char send_buff_len;
    unsigned char send_buff[70];
    for(send_buff_len = 0;send_buff_len < len;send_buff_len ++)
    {
        send_buff[send_buff_len] = command[send_buff_len];
    }


    if( send_buff_len < 32 )
    {
        for(;send_buff_len < 32;send_buff_len ++)
            send_buff[send_buff_len] = 0x00;
    }
    else if(send_buff_len < 64)
    {
        for( ; send_buff_len < 64; send_buff_len ++)
            send_buff[send_buff_len] = 0x00;
    }
#endif

    tcflush(fd, TCIOFLUSH);
    int write_res = write(fd, command, len);//向串口写数据
    if (write_res == -1)
    {
        perror(" SerialCommand write error!");
    }
    printf("comand send :");
    for (int i = 0;i < len;i ++)
        printf("  %x",command[i]);
    printf("\n");
    printf("write result :%d\n",write_res);
    return write_res;
}


static void reset_tty_atexit(const int fd)
{
    if(fd != -1)
    {
        tcsetattr(fd,TCSANOW,&oldtios);
    }
}
static void reset_tty_handler(const int fd,int signal)
{
    if(fd != -1)
    {
        tcsetattr(fd,TCSANOW,&oldtios);
    }
    _exit(EXIT_FAILURE);
}

int OpenPort(const char *portname, unsigned int speed)
{
    struct sigaction sa;
    int portfd;

    if((portfd = open(portname,O_RDWR | O_NOCTTY | O_NDELAY)) < 0 )
    {
        printf("open serial port %s fail \n ",portname);
        return portfd;
    }
    printf("opening serial port:%s,baude rate%d\n",portname,speed);
    /**get serial port parnms,save away */
    tcgetattr(portfd,&newtios);
    memcpy(&oldtios,&newtios,sizeof newtios);
    /** configure new values */
    cfmakeraw(&newtios); /*see man page */
    newtios.c_iflag |=IGNPAR; /*ignore parity on input */
    newtios.c_oflag &= ~(OPOST | ONLCR | OLCUC | OCRNL | ONOCR | ONLRET | OFILL);
    newtios.c_cflag = CS8 | CLOCAL | CREAD;
    newtios.c_cc[VMIN]=1; /* block until 1 char received */
    newtios.c_cc[VTIME]=0; /*no inter-character timer */
    /**115200 bps */
    switch(speed)
    {
        case 1200: cfsetospeed(&newtios,B1200);
            cfsetispeed(&newtios,B1200);
            break;
        case 2400: cfsetospeed(&newtios,B2400);
            cfsetispeed(&newtios,B2400);
            break;
        case 4800: cfsetospeed(&newtios,B4800);
            cfsetispeed(&newtios,B4800);
            break;
        case 9600: cfsetospeed(&newtios,B9600);
            cfsetispeed(&newtios,B9600);
            break;
        case 115200: cfsetospeed(&newtios,B115200);
            cfsetispeed(&newtios,B115200);
            break;
        case 921600: cfsetospeed(&newtios,B921600);
            cfsetispeed(&newtios,B921600);
            break;
        case 1000000: cfsetospeed(&newtios,B1000000);
            cfsetispeed(&newtios,B1000000);
            break;
        case 4000000: cfsetospeed(&newtios,B2000000);
            cfsetispeed(&newtios,B2000000);
            break;
        case 2000000: cfsetospeed(&newtios,B2000000);
            cfsetispeed(&newtios,B2000000);
            break;
    }
    /** register clean up stuff */
    atexit(reset_tty_atexit);
    memset(&sa,0,sizeof sa);
    sa.sa_handler = reset_tty_handler;
    sigaction(SIGHUP,&sa,NULL);
    sigaction(SIGINT,&sa,NULL);
    sigaction(SIGPIPE,&sa,NULL);
    sigaction(SIGTERM,&sa,NULL);
    /*apply modified termios */
    int fd = portfd;
    tcflush(portfd,TCIFLUSH);
    tcsetattr(portfd,TCSADRAIN,&newtios);
    return portfd;
}


#endif //SERIALPROJECT_SERIAL_H
