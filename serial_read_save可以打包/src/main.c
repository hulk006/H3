//
// Created by Yanghao on 18-1-23.
//

#include "serial.h"
#include "protocol.h"
//test 控制是否整体调试
#define test 0
#define log 1

int main(int argc, char *argv[])
{
    perror("check : 1baud rate; 2serial name ;3: stop bits");
    while (1) //无限循环
    {
        InitStatus();
        int serial_handle = SerialOpen(UART_DEVICE,BAUDRATE);
        /**command 01 to 13
        //SerialCommand(serial_handle,REUQEST_01_ISREADY,8);////master下达第一条指令
        //if(!HandleAnswer01Isready(serial_handle,5)) continue;

        //SerialCommand(serial_handle,REUQEST_10_NET_CONFIG,8);////master下达第二条指令
        //if(!HandleAnswer10NetConfig(serial_handle,68))             continue;

        //SerialCommand(serial_handle,REUQEST_11_DEVICE_INFO,8);
        //if(!HandleAnswer11DeviceInfo(serial_handle,34))            continue;

        //SerialCommand(serial_handle,REUQEST_12_USER_INFO,8);
        //if(!HandleAnswer12UserInfo(serial_handle,33))              continue;

        //SerialCommand(serial_handle,REUQEST_13_SYNC_TIME,10);////master下达13指令
        //if(!HandleAnswer13SysncTime(serial_handle,9))            continue;//读取回答失败，重新启动
        */
        //这一层循环，实现数据到中断传输，14 和15 条命令组合在一起可以实现中断传输data blocks
        //TODO 加入时间控制防止长时间的死循环
        SaveHeadFile(&status);//读取信息完毕，存储用户信息和wifi信息
        int sync_result = SyncDataProcess(serial_handle);//同步的状态，有任何异常都返回-1，在同步数据道时候需要进行数据道存储
        if(sync_result < 0)
            continue;//数据读取异常
        else
        {

            printf("data read finished ,Closing serial ...\n");
            close(serial_handle);
            return 0;
        }
        break;
    }
    return 0;
}