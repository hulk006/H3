//
// Created by Yanghao on 18-1-23.
//
#include <pwd.h>
#include "serial.h"
#include "protocol.h"
#include "analyze_head.h"
#include "data.h"
#include "unused.h"

#define send_time_stamp 1

#define HOSTNAME "43.255.224.59"

int main(int argc, char *argv[])
{
    //init_daemon();//初始化为Daemon
    CheckWORKING_DIR();
    while (1)
    {
        setvbuf(stdout,NULL,_IONBF,0);
        /*
        if(argv[1])
        {
            N = atof(argv[1]);
            printf("package N  = %d\n",N);
        }
        if(argv[2])
        {
            READ_NUMBER = atof(argv[2]);
            printf("READ_NUMBER = %d\n",READ_NUMBER);
        }
         */
        if(argv[1])
        {
            printf("argv[1]= %s,log on \n",argv[3]);
            Log();
        }
        InitStatus();
        int serial_handle = open_port(UART_DEVICE,BAUDRATE);
        printf("open serial fd =%d\n",serial_handle);
        printf(" Ask H3 box is ready?");
        if(SerialCommand(serial_handle, REUQEST_01_ISREADY, 8) != 8)
        {
            printf("not ready \n");
            //printf("sleep 60 s");
            //sleep(60);
            //continue;////master下达第一条指令
        }

        if(HandleAnswer01Isready(serial_handle,5))//发现串口，每隔30查看一次串口
        {
            while (1) //无限循环
            {
                SerialCommand(serial_handle, REUQEST_10_NET_CONFIG, 8);////master下达第二条指令
                if (!HandleAnswer10NetConfig(serial_handle, 68)) continue;

                SerialCommand(serial_handle, REUQEST_11_DEVICE_INFO, 8);
                if (!HandleAnswer11DeviceInfo(serial_handle, 24)) continue;
                printf("length=%d\n", sizeof(status.device_info.name));
                SerialCommand(serial_handle, REUQEST_12_USER_INFO, 8);
                if (!HandleAnswer12UserInfo(serial_handle, 17)) continue;
#if send_time_stamp
                Send13Command(serial_handle);////master下达13指令
                if (!HandleAnswer13SysncTime(serial_handle, 9)) continue;//读取回答失败，重新启动
                printf("need sleep 120 s ,wait to write \n");
                //sleep(200);//s
                if(CheckWifiConnect(HOSTNAME)<=0)
                    break;
#endif
                //这一层循环，实现数据到中断传输，14 和15 条命令组合在一起可以实现中断传输data blocks
                SaveHeadFile(&status);//读取信息完毕，存储用户信息和wifi信息
                int sync_result = SyncDataProcess(serial_handle, WORKING_DIR);//同步的状态，有任何异常都返回-1，在同步数据道时候需要进行数据道存储
                if (sync_result < 0)
                {
                    Rename();//把存储的文件修改为可以上传的文件
                    continue;//数据读取异常
                }
                else //数据同步成功
                {
                    printf("data read finished ,Closing serial ...\n");
                    close(serial_handle);
                    break;
                }
            }
        }
        else
        {
            printf("no wifi connect or no H3 box is not ready！, sleep60 s");
            sleep(60);
            close(serial_handle);
        }
        printf("sleep 60 s");
        sleep(60); //s

    }
    return 0;
}
