//
// Created by Yanghao on 18-1-23.
//
#include <pwd.h>
#include <time.h>
#include "serial.h"
#include "protocol.h"
#include "unused.h"

#define HOSTNAME "www.baidu.com"

int main(int argc, char *argv[])
{
    printf("serial_read_save|0 console,not daemon,1 log to file daemon|");
    setvbuf(stdout,NULL,_IONBF,0);
#if debug
    printf("this is debug");
#else
    if(atoi(argv[1])>0)
    {
        init_daemon();//初始化为Daemon
    }
    if(atoi(argv[1])>0)
    {
        printf("argv[1]= %s,log on \n",argv[1]);
        Log();
    }
#endif
    CheckWORKING_DIR();

    while (1)
    {
        int serial_handle = open_port(UART_DEVICE,BAUDRATE);
        printf("\nopen serial fd = %d\n",serial_handle);
        while(1) //once read and save
        {

            DATA_NUMBER=0;
            ADDRESS = 0;//block address + 1
            InitStatus();
            GetLocalTime();
            SerialCommand(serial_handle, REUQEST_01_ISREADY, 8);
            if(HandleAnswer01Isready(serial_handle,5))//发现串口，每隔30查看一次串口
            {
                SerialCommand(serial_handle, REUQEST_10_NET_CONFIG, 8);
                if (!HandleAnswer10NetConfig(serial_handle, 68)) continue;


                SerialCommand(serial_handle, REUQEST_11_DEVICE_INFO, 8);
                if (!HandleAnswer11DeviceInfo(serial_handle, 24)) continue;

                SerialCommand(serial_handle, REUQEST_12_USER_INFO, 8);
                if (!HandleAnswer12UserInfo(serial_handle, 17)) continue;

                if(CheckWifiConnect(HOSTNAME) <= 0)
                {
                    printf("no network sleep 60s");
                    sleep(60);
                    break;
                }

                system("echo \"1\" > /sys/class/leds/zbt-wr8305rt:green:wifi/brightness ");
                system("/etc/init.d/sysfixtime start");//time update
                printf("update time,wait 2s\n");
                sleep(2);

                GetLocalTime();
                Send13Command(serial_handle);
                if (!HandleAnswer13SysncTime(serial_handle, 9)) continue;

                SaveHeadFile(&status);//读取信息完毕，存储用户信息和wifi信息
                int sync_result = SyncDataProcess(serial_handle, WORKING_DIR);//同步的状态，有任何异常都返回-1，在同步数据道时候需要进行数据道存储
                system("echo \"0\" > /sys/class/leds/zbt-wr8305rt:green:wifi/brightness ");
                if (sync_result < 0)
                {
                    Rename();//to delete
                    sleep(60);
                    continue;//数据读取异常,退出当前
                }
                else //数据同步成功
                {
                    printf("**************data read finished ,Closing serial,sleep(60)*****************\n");

                    sleep(60);
                    break;//退出循环
                }
            }
            else
            {
                printf("no wifi connect or no H3 box is not ready！, sleep60 s\n");
                system("echo \"0\" > /sys/class/leds/zbt-wr8305rt:green:wifi/brightness ");
                sleep(60);
            }
        }
        close(serial_handle);
        sleep(60);//一次数据同步成功，sleep（60）
    }
    return 0;
}
