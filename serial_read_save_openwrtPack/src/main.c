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
    InitStatus();
    while (1) //无限循环
    {
        //初始化
        char  buf[MAXSIZE] = {'\0'};
        int serial_handle = SerialOpen(UART_DEVICE,BAUDRATE);
#if test
        SerialCommand(serial_handle,REUQEST_01_ISREADY);////master下达第一条指令
        puts("reading  ");
        if(!HandleAnswer01Isready(buf,serial_handle)) continue;//STA
        if(status.is_ready == '1') continue;//如果没有准备好
#endif

        SerialCommand(serial_handle,REUQEST_10_NET_CONFIG);////master下达第二条指令
         if(!HandleAnswer10NetConfig(buf,serial_handle))             continue;


#if test
        SerialCommand(serial_handle,REUQEST_11_DEVICE_INFO);
        if(!HandleAnswer11DeviceInfo(buf,serial_handle))            continue;
#endif

#if test
        SerialCommand(serial_handle,REUQEST_12_USER_INFO);
        if(!HandleAnswer12UserInfo(buf,serial_handle))              continue;
#endif
#if test
        SerialCommand(serial_handle,REUQEST_13_SYNC_TIME);////master下达13指令
#endif
        //if(!HandleAnswer13SysncTime(buf, serial_handle))            continue;//读取回答失败，重新启动
        //这一层循环，实现数据到中断传输，14 和15 条命令组合在一起可以实现中断传输data blocks
        //TODO加入时间控制防止长时间的死循环。。。。。。。。。。。。。。。。。。。。。。。。。。
        //SaveHeadFile(&status);//读取信息完毕，存储用户信息wifi信息
        //int sync_result = SyncDataProcess(serial_handle);//同步的状态，有任何异常都返回-1，在同步数据道时候需要进行数据道存储
        //if(sync_result < 0)
         //   continue;//数据读取异常
        //else
        {
            printf("Close...\n");
            close(serial_handle);
            return 0;
        }
    }
    return 0;
}