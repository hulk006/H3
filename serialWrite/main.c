#include <sys/types.h>
#include <stdio.h>
//#include <io.h>
//#include <alloc.h>
#include <fcntl.h>
//#include <process.h>
#include <sys/stat.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <zconf.h>
#include <stdlib.h>
#include <memory.h>

#define BAUDRATE        B57600
#define UART_DEVICE     "/dev/ttyS0"

#define FALSE  -1
#define TRUE   0
////////////////////////////////////////////////////////////////////////////////
/**
*@brief  设置串口通信速率
*@param  fd     类型 int  打开串口的文件句柄
*@param  speed  类型 int  串口速度
*@return  void
*/
int speed_arr[] = {B115200,B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300,
                   B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300, };
int name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300,
                  115200, 38400, 19200, 9600, 4800, 2400, 1200, 300, };
void set_speed(int fd, int speed){
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
////////////////////////////////////////////////////////////////////////////////
/**
*@brief   设置串口数据位，停止位和效验位
*@param  fd     类型  int  打开的串口文件句柄
*@param  databits 类型  int 数据位   取值 为 7 或者8
*@param  stopbits 类型  int 停止位   取值为 1 或者2
*@param  parity  类型  int  效验类型 取值为N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
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
    options.c_cc[VTIME] = 150; /* 设置超时15 seconds*/
    options.c_cc[VMIN] = 0; /* Update the options and do it NOW */
    if (tcsetattr(fd,TCSANOW,&options) != 0)
    {
        perror("SetupSerial 3");
        return (FALSE);
    }
    options.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG);  /*Input*/
    options.c_oflag  &= ~OPOST;   /*Output*/
    return (TRUE);
}
////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{

    int fd = 0;
    char  buf[256] = {'\0'};
    puts("Start...\n");
    fd = open(UART_DEVICE, O_RDWR);

    if (fd < 0)
    {
        perror(UART_DEVICE);
        exit(1);
    }

    puts("Open...\n");
    set_speed(fd,57600);
    if (set_Parity(fd,8,1,'N') == FALSE)
    {
        printf("Set Parity Error\n");
        exit (0);
    }

    puts("writing...\n");
    while(1)
    {
        printf("please input a char \n");
        char *input_ptr = gets(buf);
        puts(buf);
        if(strcmp(buf,"stop") == 0)
        {
            break;
        }
        if((write(fd, buf, 255)) == -1 )
        {
            perror("write error!");
            exit(1);
        }
        write(fd, buf, 255);
        memset(buf,'\0', sizeof (buf) );
        if(buf[0] == '\0')
            puts("buf had been clear!");

     }

    printf("Close...\n");
    close(fd);


    return 0;
}