//
// Created by Yanghao on 18-1-23.
//
#include <pwd.h>
#include <time.h>
#include "serial.h"
#include "protocol.h"
#include "daemon.h"
#define HOSTNAME "www.baidu.com"

bool SerialReadSave(const int serial_handle)
{
    InitStatus();
    GetLocalTime();
    tcflush(serial_handle, TCIOFLUSH);
    SerialCommand(serial_handle, REUQEST_01_ISREADY, 8);
    if(HandleAnswer01Isready(serial_handle,5))//发现串口，每隔30查看一次串口
    //if(1)
    {
        tcflush(serial_handle, TCIOFLUSH);
        SerialCommand(serial_handle, REUQEST_10_NET_CONFIG, 8);
        if (!HandleAnswer10NetConfig(serial_handle, 68)) return false;

        SerialCommand(serial_handle, REUQEST_11_DEVICE_INFO, 8);
        if (!HandleAnswer11DeviceInfo(serial_handle, 24)) return false;

        SerialCommand(serial_handle, REUQEST_12_USER_INFO, 8);
        if (!HandleAnswer12UserInfo(serial_handle, 17)) return false;

        if(CheckWifiConnect(HOSTNAME) <= 0)
        {
            printf("no network sleep 60s\n");
            sleep(60);//s
            return false;
        }
        system(TIME_UPDATE);//time update
        printf("update time,wait 2s\n");
        sleep(1);//s
        /*
        SerialCommand(serial_handle, REUQEST_16_SERVER_IP, 8);
        if(!HandleAnswer16ServerIP(serial_handle,5)) return false;

        SerialCommand(serial_handle, REUQEST_17_KEY, 8);
        if(!HandleAnswer17Key(serial_handle,20)) return false;
        */

        GetLocalTime();
        Send13Command(serial_handle);
        if (!HandleAnswer13SysncTime(serial_handle, 9)) return false;

        SaveConfigFile(&g_status);//读取信息完毕，存储用户信息和wifi信息
        if (!ReadDataHead(serial_handle)) return false;   //读取数据头
        HandleDataHead(); //生成数据头

        if (g_NEED_READ_ECG_NUMBER > 0)
        {
            system(LED_BRIGHTNESS); /**open wifi led****************/
            bool read_ecg_result = ReadEcgDataBlock(serial_handle);
            if (read_ecg_result)
            {
                //close(serial_handle);
                printf("read finish\n");
                return true;
            }
            else
            {
                return false; //异常报警
            }
        }
        else if(g_NEED_READ_ECG_NUMBER == 0)
        {
            printf("no dat need read");
            //system(LED_CLOSE);
            return true;
        }
        else
        {
            return false;//异常报警
        }
    }
    else
    {
        printf("no box in H3-base");
        return false;//异常报警
    }
}


int main(int argc, char *argv[])
{
    printf("usage:1 log|device|baud\n");
    setvbuf(stdout,NULL,_IONBF,0);
    if(argc!=4)
    {
        perror("need 3 argument:log|device|baud rate");
        return false;
    }

    if(atoi(argv[1])>0)
    {
        printf("argv[1]= %s,log on \n",argv[1]);
        init_daemon();//初始化为Daemon
        Log();

    }
    if (argv[2])
    {
        memset(g_UART_DEVICE,0, sizeof(g_UART_DEVICE));
        strcpy(g_UART_DEVICE,argv[2]);
    }

    if (CheckBaudeRate(atoi(argv[3])))
    {
        g_BAUDRATE = atoi(argv[3]);
    }
    CheckWORKING_DIR();
    while (1)
    {
        int serial_handle = OpenPort(g_UART_DEVICE,g_BAUDRATE);
        if(serial_handle<0)
        {
            system(LED_TWINKLE);
            sleep(60);
            continue;
        }

        while (1)
        {
            if(!SerialReadSave(serial_handle))//读取失败
            {
                printf("##############warning##################");
                system(LED_TWINKLE);
                sleep(120);
            }
            else//读取成功或者没有数据
            {
                sleep(120);
            }
        }
        close(serial_handle);
        sleep(60);
    }

    return 0;
}
