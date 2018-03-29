//
// Created by Yanghao on 18-1-23.
//
#include "serial.h"
#include "protocol.h"

int main(int argc, char *argv[])
{
    setvbuf(stdout,NULL,_IONBF,0); //如果你嫌上个方法麻烦, 就使用这个函数. 直接将缓冲区禁止了. 它就直接输出了.
    //Log();
    perror("check : 1baud rate; 2serial name ;3: stop bits");
    InitStatus();
    int serial_handle = SerialOpen(UART_DEVICE,BAUDRATE);
    ///command 01 to 13
    SerialCommand(serial_handle, REUQEST_01_ISREADY, 8);////master下达第一条指令
    if(HandleAnswer01Isready(serial_handle,5))//发现串口，每隔30查看一次串口
    {
        while (1) //无限循环
        {
            SerialCommand(serial_handle, REUQEST_10_NET_CONFIG, 8);////master下达第二条指令
            if (!HandleAnswer10NetConfig(serial_handle, 68)) continue;

            SerialCommand(serial_handle, REUQEST_11_DEVICE_INFO, 8);
            if (!HandleAnswer11DeviceInfo(serial_handle, 24)) continue;

            SerialCommand(serial_handle, REUQEST_12_USER_INFO, 8);
            if (!HandleAnswer12UserInfo(serial_handle, 17)) continue;

            SerialCommand(serial_handle, REUQEST_13_SYNC_TIME, 10);////master下达13指令
            if (!HandleAnswer13SysncTime(serial_handle, 9)) continue;//读取回答失败，重新启动

            //这一层循环，实现数据到中断传输，14 和15 条命令组合在一起可以实现中断传输data blocks
            SaveHeadFile(&status);//读取信息完毕，存储用户信息和wifi信息
            int sync_result = SyncDataProcess(serial_handle, "/home/yh/CLionProjects/serialReadAndSave/");//同步的状态，有任何异常都返回-1，在同步数据道时候需要进行数据道存储
            if (sync_result < 0)
                continue;//数据读取异常
            else //数据同步成功
            {
                printf("data read finished ,Closing serial ...\n");
                close(serial_handle);
                return 0;
            }
        }
    }
    else
    {
        sleep(30);
        close(serial_handle);
    }
    return 0;
}