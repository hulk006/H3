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
//#include "data.h"

#define BAUDRATE        115200
//笔记本上ttyS1

#define FALSE  -1
#define TRUE   0
#define MAXSIZE 1024
#define TRY "try again\n"
#define WAIT_TIME_RECV 20//未收到应答到等待时间
#define debug 1

#if debug
#define UART_DEVICE     "/dev/ttyS1"
#else
#define UART_DEVICE     "/dev/ttyS0"
#endif


static struct termios newtios,oldtios; /*termianal settings */
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
 * @brief  设置串口通信速率
 * @param  fd     类型 int  打开串口的文件句柄
 * @param  speed  类型 int  串口速度
 * @return  void
*/
speed_t speed_arr[] = {B115200,B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300, B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300, 115200, 38400, 19200, 9600, 4800, 2400, 1200, 300, };
void SerialSetBaudRate(int fd, int speed)
{
    int   status;
    struct termios   Opt;
    tcgetattr(fd, &Opt);
    for (int i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
    {
        if  (speed == name_arr[i])
        {
            tcflush(fd, TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(fd, TCSANOW, &Opt);
            if  (status != 0)
            {
                perror("tcsetattr fd1");
                return;
            }
            tcflush(fd,TCIOFLUSH);
        }
    }
}
/**
 * @brief   设置串口数据位，停止位和效验位
 * @param  fd     类型  int  打开的串口文件句柄
 * @param  databits 类型  int 数据位   取值 为 7 或者8
 * @param  stopbits 类型  int 停止位   取值为 1 或者2
 * @param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int SerialSetParity(int fd,int databits,int stopbits,int parity)
{
    struct termios options;
    if  ( tcgetattr( fd,&options)  !=  0) {
        perror("SetupSerial 1");
        return(FALSE);
    }
    options.c_cflag &= ~CSIZE;
    switch (databits) /*设置数据位数*/
    {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            fprintf(stderr,"Unsupported data size\n"); return (FALSE);
    }
    switch (parity)
    {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;   /* Clear parity enable */
            options.c_iflag &= ~INPCK;     /* Enable parity checking */
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB); /* 设置为奇效验*/
            options.c_iflag |= INPCK;             /* Disnable parity checking */
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;     /* Enable parity */
            options.c_cflag &= ~PARODD;   /* 转换为偶效验*/
            options.c_iflag |= INPCK;       /* Disnable parity checking */
            break;
        case 'S':
        case 's':  /*as no parity*/
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;break;
        default:
            fprintf(stderr,"Unsupported parity\n");
            return (FALSE);
    }
    /* 设置停止位*/
    switch (stopbits)
    {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
            break;
        default:
            fprintf(stderr,"Unsupported stop bits\n");
            return (FALSE);
    }
    /* Set input parity option */
    if (parity != 'n')
        options.c_iflag |= INPCK;
    tcflush(fd,TCIFLUSH);
#if debug
    options.c_cc[VTIME] = 100; /* 设置超时15 seconds*/
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
#else
    options.c_cc[VTIME] = 1; /* 设置超时15 seconds*/
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
#endif
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        perror("SetupSerial 3");
        return (FALSE);
    }
    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    options.c_oflag  &= ~OPOST;   /*Output*/
    return (TRUE);
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
        Sleep(200);
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
 * @brief  超时等待读取串口收到到的data block
 * @param  fd     类型 int  串口句柄
 * @param  p      char 输出的字符串
 * @param  desire_get_len int最大长度
 * @param  time_out int 串口等待时间
 * @return  读取buf的长度
*/
int SerialReadDataBlock(const int fd,unsigned char * p,const int desire_get_len,const int time_out)
{
    static int n_data = 0;
    int nBytes = 0;
    unsigned char read_temp[2048];
    struct timeval tv;
    tv.tv_sec = time_out;
    tv.tv_usec = 0;
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd,&read_fds);
    puts("select");

    //获取时间当前时间
    time_t t1;
    t1 = time(NULL);
    /**循环读取data blocks */
    if(n_data == 0)
    {

        while (select(fd+1,&read_fds,NULL,NULL,&tv))
        {
            Sleep(10);//ms
            memset(read_temp,'\0',2048);
            int nread = read(fd, read_temp, 2048);//一次最多读取22k字符
            int start =  nBytes;
            nBytes += nread;//一共读取了多少个字符
            int stop = nBytes;
            for(int i = start;i < stop;++i)
            {
                p[i] = read_temp[i - start];
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
            memset(read_temp,'\0',2048);
            int nread_0 = read(fd, read_temp, 2048);//一次最多读取22k字符

            int start_0 =  nBytes;
            if(nread_0>0)
            {
                nBytes += nread_0;//一共读取了多少个字符
            }
            int stop_0 = nBytes;
            for(int i = start_0;i < stop_0;++i)
            {
                p[i] = read_temp[i - start_0];
            }
            time_t t4;
            t4 = time(NULL);
            int flag = (int)(t4-t2);
            if((p[nBytes] == 'A'&&p[nBytes-1]=='5')||flag > 10)
            {
                printf("time flag=%d\n",flag);
                break;
            }
        }

    }
    else
    {
        int not_read_num=0;
        while (select(fd+1,&read_fds,NULL,NULL,&tv))
        {
            Sleep(10);//ms
            memset(read_temp,'\0', 2048);
            int nread = read(fd, read_temp, 2048);//一次最多读取22k字符
            if(nread == 0 || nread== -1)
            {
                not_read_num++;
                if(not_read_num == 5)
                    break;
            }
            else
            {
                int start =  nBytes;
                nBytes += nread;//一共读取了多少个字符
                int stop = nBytes;
                for(int i = start;i < stop;++i)
                {
                    p[i] = read_temp[i - start];
                }
            }

        }
        time_t t4;
        t4 = time(NULL);
        printf("\ncost time = %ds ",(int)(t4 - t1));

    }
    n_data++;
    time_t t3;
    t3 = time(NULL);
    printf("\ncost time = %ds ",(int)(t3 - t1));
    return nBytes;
}

/**
 * @brief  打开串口函数
 * @param  serial_name     类型 char  串口的名称
 * @param  bau             类型 int   串口波特率
 * @return  串口句柄        int
*/
int SerialOpen(const char *serial_name,const int bau)
{
    int   serial_handle = 0;
    printf("Start...\n");
    //打开串口返回handle
    serial_handle = open(serial_name,  O_RDWR| O_NOCTTY |O_NDELAY);

    if (serial_handle < 0)
    {
        perror(serial_name);
        exit(-1);
    }
    printf("%s serial is open\n", serial_name);
    SerialSetBaudRate(serial_handle, bau);

    if (SerialSetParity(serial_handle, 8, 1, 'N') == FALSE)
    {
        printf("Set Parity Error\n");
        exit(-1);
    }
    printf("serial'bauderate is:%d\n", bau);
    return serial_handle;
}

/**
 * @brief  串口发送命令到函数
 * @param  fd       类型 int  串口的句柄
 * @param  command  类型 char 命令码
 * @return int 1 发送命令成功 -1失败
*/
int SerialCommand(const int fd,const unsigned char *command,const int len)
{
    tcflush(fd, TCIOFLUSH);
    int write_res = write(fd,command,len);//向串口写数据
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

/**
 * @brief  关闭串口
 * @param  time       类型 int  关闭串口之后等待时间
 * @return void
*/
void SerialClose(const int fd, const int time)//串口到句柄
{
    printf("serial closed,wait %d s,%s", time,TRY);
    close(fd);//关闭串口
    sleep((unsigned int) time);//30S
    return;
}
//********************************************************
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

int open_port(const char *portname, unsigned int speed)
{
    struct sigaction sa;
    int portfd;
    printf("opening serial port:%s\n",portname);
    /*open serial port */
    if((portfd=open(portname,O_RDWR | O_NOCTTY | O_NDELAY)) < 0 )
    {
        printf("open serial port %s fail \n ",portname);
        return portfd;
    }

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
