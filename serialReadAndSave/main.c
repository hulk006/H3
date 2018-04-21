//
// Created by Yanghao on 18-1-23.
//
#include <pwd.h>
#include "serial.h"
#include "protocol.h"
#include "unused.h"

//#define HOSTNAME "43.255.224.59"
#define HOSTNAME "www.baidu.com"

int main(int argc, char *argv[])
{
    setvbuf(stdout,NULL,_IONBF,0);
    //init_daemon();//初始化为Daemon
    CheckWORKING_DIR();
    if(atoi(argv[1])>0)
    {
        printf("argv[1]= %s,log on \n",argv[1]);
        Log();
    }
    while (1)
    {
        int serial_handle = open_port(UART_DEVICE,BAUDRATE);
        printf("open serial fd = %d\n",serial_handle);
        if(argv[2])
        {
            printf("send time only ,wait to write \n");
            Send13Command(serial_handle);////master下达13指令
            DeleteDataBloack(serial_handle,1,200);
            printf("send time success！\n");
            exit(0);
        }

        while(1) //once read and save
        {
            DATA_NUMBER=0;
            ADDRESS = 0;//block address + 1
            InitStatus();
            printf(" Ask H3 box is ready?\n");
            SerialCommand(serial_handle, REUQEST_01_ISREADY, 8);
            if(HandleAnswer01Isready(serial_handle,5))//发现串口，每隔30查看一次串口
            {
                SerialCommand(serial_handle, REUQEST_10_NET_CONFIG, 8);
                if (!HandleAnswer10NetConfig(serial_handle, 68)) continue;

                SerialCommand(serial_handle, REUQEST_11_DEVICE_INFO, 8);
                if (!HandleAnswer11DeviceInfo(serial_handle, 24)) continue;

                SerialCommand(serial_handle, REUQEST_12_USER_INFO, 8);
                if (!HandleAnswer12UserInfo(serial_handle, 17)) continue;

                Send13Command(serial_handle);
                if (!HandleAnswer13SysncTime(serial_handle, 9)) continue;
                if(CheckWifiConnect(HOSTNAME) <= 0)
                {
                    sleep(60);
                    break;
                }


                SaveHeadFile(&status);//读取信息完毕，存储用户信息和wifi信息
                int sync_result = SyncDataProcess(serial_handle, WORKING_DIR);//同步的状态，有任何异常都返回-1，在同步数据道时候需要进行数据道存储
                if (sync_result < 0)
                {
                    Rename();//to delete
                    sleep(60);
                    continue;//数据读取异常,退出当前
                }
                else //数据同步成功
                {
                    printf("***************data read finished ,Closing serial,sleep(60)*****************\n");
                    sleep(60);
                    break;//退出循环
                }

            }
            else
            {
                printf("no wifi connect or no H3 box is not ready！, sleep60 s\n");
                sleep(60);
            }

            printf("sleep 60 s\n");
            sleep(60);
        }
        close(serial_handle);
        sleep(120);//一次数据同步成功，sleep（60）
    }
    return 0;
}
