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

#define BAUDRATE        115200
#define UART_DEVICE     "/dev/ttyS1"
#define FALSE  -1
#define TRUE   0
#define MAXSIZE 1024
#define DATASIZE 10000 //10k个字节暂定
#define TRY "try again\n"
#define WAIT_TIME_RECV 20//未收到应答到等待时间
#define SEND_DELAY_TIME 200//ms,串口的数据发送和接收比较慢，串口发送发送完后需要等待一段时间
#define debug 0

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
    options.c_cc[VTIME] = 0; /* 设置超时15 seconds*/
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
int SerialRead(const int fd,char * p,const int desire_get_len,const int time_out)
{
    int nread = 0 ;
    char read_temp[desire_get_len];

    struct timeval tv;
    tv.tv_sec = time_out;
    tv.tv_usec = 0;
    bzero(read_temp,sizeof(read_temp));
    tcflush(fd, TCIOFLUSH);//清除串口缓存
    fd_set read_fds;
   while(1)
   {
       FD_ZERO(&read_fds);
       FD_SET(fd,&read_fds);
       tcflush(fd, TCIOFLUSH);//清除串口缓存
       puts("select");

       if(select(fd+1,&read_fds,NULL,NULL,&tv))
       {
           Sleep(200);
           nread = read(fd,read_temp, sizeof(read_temp));
           if(nread<=0)
           {
               printf("read get problem!\r\n");
               return -1;
           }
           break;
       }
       else
       {
           perror("select():time out");
           break;
       }

   }

/*
    FD_ZERO(&read_fds);
    FD_SET(fd,&read_fds);
    tcflush(fd, TCIOFLUSH);//清除串口缓存
    puts("select");
    if(select(fd+1,&read_fds,NULL,NULL,&tv))
    {
        sleep(1000);
        char read_once[100];
        while(read(fd,read_once, sizeof(read_once))>0)
        {
            strcat (read_temp,read_once);
        }

    }
    else
    {
        perror("select():time out");
        return -1;
    }
*/
    memcpy(p,read_temp, sizeof(read_temp));
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
int SerialReadDataBlock(const int fd,char * p,const int time_out)
{
    int nBytes = 0;
    char read_temp[1024];
    struct timeval tv;
    tv.tv_sec = time_out;
    tv.tv_usec = 0;

    tcflush(fd, TCIOFLUSH);//清除串口缓存
    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd,&read_fds);
    tcflush(fd, TCIOFLUSH);//清除串口缓存
    puts("select");
    if(select(fd+1,&read_fds,NULL,NULL,&tv))
    {
        Sleep(1000);//1000ms
        while (1)
        {
            int nread = 0;
            bzero(read_temp,sizeof(read_temp));
            nread = read(fd, read_temp, sizeof(read_temp));
            if (nread > 0)
            {
                strcat(p, read_temp);
                nBytes += nread;
            }
            else if (nread == 0)
            {

                printf("read finish");
                break;
            }
            else
            {
                printf("read error");
                break;
            }
        }
    }

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
#if debug
    serial_handle = open(serial_name,  O_RDWR| O_NOCTTY );
#else
    serial_handle = open(serial_name,  O_RDWR| O_NOCTTY |O_NDELAY);
#endif
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
int SerialCommand(const int fd,const char *command)
{
    printf("master sending command:");
    int write_res = write(fd, command, strlen(command));//向串口写数据
    if (write_res == -1)
    {
        perror(" SerialCommand write error!");
        exit(-1);
    }
    Sleep(SEND_DELAY_TIME);
    printf("%s  success",command);
    return 1;
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



#endif //SERIALPROJECT_SERIAL_H
