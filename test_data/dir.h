//
// Created by openwrt on 18-2-2.
//建立文件夹
//TODO
#ifndef SERIALPROJECT_DIR_H
#define SERIALPROJECT_DIR_H

#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <zconf.h>
#include <assert.h>
#include <time.h>
#include <pwd.h>
#include "data.h"
#include "analyze_head.h"
#include "protocol.h"
#include <sys/stat.h>
#include <dirent.h>

#define HEAD_POSTFIX ".HEAD"
#define ACCESS access
#define MKDIR(a) mkdir((a),0755)
#define HOME_DIR (getpwuid(getuid())->pw_dir)

float GetDirSize(const char* path)
{
    DIR *dir;
    float size=0;
    dir = opendir(path);
    struct dirent *ptr;
    while((ptr = readdir(dir)) != NULL)
    {
        char filename[100]={'\0'};
        struct stat stat_buff;
        MergeString2(filename,path,ptr->d_name);
        stat(filename,&stat_buff);
        size += stat_buff.st_size;
    }
    closedir(dir);
    return size/(1024*1024);
}


/**
 * @func 把当前的时间转换成时间戳
 * @param
 */
void GetTimeCurrent(unsigned char * time_buf,int length)
{
    struct timeval tv;
    gettimeofday(&tv,NULL);

    long int time = tv.tv_sec;
    printf("%x\n",time);
    unsigned char *p = (&time);
    for (int i = 0; i < 4; ++i)
    {
        time_buf[i+4] = *(p+i);
    }
}

/**
 * @func 修改文件的名称
 * @param old_name
 * @param new_name
 */
void ChangeFileName(const char *old_name,const char *new_name)
{
    if (rename(old_name, new_name) == 0)
        printf("已经把文件 %s 修改为 %s.\n", old_name, new_name);
    else
        perror("rename");
    return ;
}
int CreatDir(char const  *pDir)
{
    int iRet;
    if(NULL == pDir)
    {
        return 0;
    }
    iRet = ACCESS(pDir,0);//检查文件夹的存在性
    if (iRet != 0) {
        iRet = MKDIR(pDir);//建立目录
        if (iRet != 0) {
            return -1;
        }
    }
    return iRet;
}

int CreatFile(const char *file_name,FILE *fp)
{
    const char *mode = "w+";
    fp = fopen(file_name,mode);//"w" 写 如果文件存在，把文件截断至0长；如果文件不存在，会创建文件
    if(fp != NULL)
        return 1;
    else
    {
        perror("CreatFile");
        return -1;
    }
}
/**
 * @func 把14进制的device——id 转化为字符串
 * @param
 * @return
 */
int GetDeviceID(const unsigned char *input,char *out,int length)
{
    for (int i = 0; i < length; ++i) {
        int high = 0x00;
        int low = 0x00;
        high = (input[i] & 0xf0) / 16;
        low = input[i] & 0x0f;
        if (high >= 0 && high < 16)
        {
            char tmp='0';
            switch (high) {
                case 10:
                    tmp='A';
                    break;
                case 11:
                    tmp='B';
                    break;
                case 12:
                    tmp='C';
                    break;
                case 13:
                    tmp='D';
                    break;
                case 14:
                    tmp='E';
                    break;
                case 15:
                    tmp='F';
                    break;
                default:
                    tmp= high + '0';
                    break;
            }
            out[2*i]=tmp;
        }
        else
        {
            printf("error,chang");
            return -1;
        }
        if (low >= 0 && low < 16) {
            char tmp='0';
            switch (low) {
                case 10:
                    tmp='A';
                    break;
                case 11:
                    tmp='B';
                    break;
                case 12:
                    tmp='C';
                    break;
                case 13:
                    tmp='D';
                    break;
                case 14:
                    tmp='E';
                    break;
                case 15:
                    tmp='F';
                    break;
                default:
                    tmp= low + '0';
                    break;
            }
            out[2*i+1]=tmp;
        }
        else {
            printf("error,change");
            return -1;
        }
    }
}
/**
 * @func 存储用户信息，wifi配置信息道文件中
 * @param input_tatus data定义道结构体
 * @return 0
 */
int SaveHeadFile( struct Status const *input_tatus)
{
    char user_dir[80]={'\0'};
    MergeString2(user_dir, WORKING_DIR,input_tatus->user_bind_info.user_id);
    printf("current working directory: %s\n", user_dir);
    CreatDir(user_dir);
    FILE *head_file;
    const char *mode = "w";
    char filename[100]={'\0'} ;
    MergeString2(filename,user_dir,"/head_file.txt");
    /**验证文件是否存在*/
    if (!access(filename,0) )
        printf("file %s  exist\n",filename);
    else
        CreateDataFile(filename);
    /**打开已有的data文件*/
    head_file = fopen(filename,mode);//"w" 写，如果文件存在，把文件截断至0长；如果文件不存在，会创建文件
    assert(head_file != NULL);
    fprintf(head_file,"{");
    fprintf(head_file,"\'userId\':\'%s\',",input_tatus->user_bind_info.user_id);
    fprintf(head_file,"\'bindState\':\'%d\',",input_tatus->user_bind_info.bind);
    //fprintf(head_file,"\'deviceId\':\'%s\',",input_tatus->device_info.name);
    char deviceId[12+1]={'0'};
    GetDeviceID(input_tatus->device_info.mac_id,deviceId,6);
    fprintf(head_file,"\'deviceId\':\'%s\',",deviceId);

    //TODO 设备登录道时候需要的变量
    fprintf(head_file,"\'token\':\'aaaa\',");

    fprintf(head_file,"\'wifiName\':\'%s\',",input_tatus->net_config.SSID);
    fprintf(head_file,"\'wifiPassword\':\'%s\',",input_tatus->net_config.PWD);
    fprintf(head_file,"}");
    //TODO
    fclose(head_file);
    return 0;
}
/**
 * @func 一个block为256k个字节大小，计划100个block打包发送一次，需要将100个打他block进行存储一次。
 * 将一次收到的打他block的字符串存储进文件。存储规则：n*num个data blocks为一个文件
 * 如果少于这个数n*num就接着打开这个文件，如果大于n*num这个数据就重新打开一个文件去存储。
 * @param data_block
 * @param user
 * @return 收到的block 个数
 */
int SaveDataBlocksFile(const struct DataBlock *data_block,  char const *user_id)
{
    //文件名以开始存储的时间命名，将一次读取道blocks数量存储进去，
    char filename[100] = {'\0'} ;
    int num_data_block = status.ndata_blocks;//只初始化一次
    /** 存数data block 的头文件*/
    if(num_data_block == 1)
    {
        const char *mode = "wb";//如果不存在，就新建一个
        MergeString2(filename,WORKING_DIR,user_id);
        MergeString3(filename,"/data_blocks_file","_head",HEAD_POSTFIX);
        /**验证文件是否存在*/
        if (!access(filename,0) )
            printf("file %s  exist\n",filename);
        else
            CreateDataFile(filename);
        /**打开已有的data文件*/
        //打开已有的data文件
        FILE *data_blocks_file;
        data_blocks_file = fopen(filename, mode);
        assert(data_blocks_file != NULL);
        /**存储data block*/
        //TODO
        int a=sizeof(data_block->rec_buf);
        fwrite(data_block->rec_buf, HEAD_DATA_BLOCK_SIZE,1,data_blocks_file);
        fclose(data_blocks_file);//关闭
        return num_data_block;
    }
    else //第二个block才是data block
    {
        int data_type = dynamic_data_header[status.ndata_blocks-2].type;
        int data_length = dynamic_data_header[status.ndata_blocks-2].length;
        if(data_type != 1||data_length <= 0)
        {
            return num_data_block;
        }

        int n_block = (status.ndata_blocks - 2);//第n个，从0 开始，代表第几个block，不包含头
        /**合并时间戳和data*/
        unsigned char time_buff[8]={0x00};
        GetTime(time_buff,n_block);

        unsigned char merge_buff[8 + 256*1024]={'\0'};
        for(int i=0;i<8;++i)
        {
            merge_buff[i]=time_buff[i];
        }

        for(int i = 0;i< data_length;++i)
        {
            merge_buff[i+8] = data_block->rec_buf[i+2];
        }

        char num_string[10];
        //nu
        sprintf(num_string, "%d", ADDRESS * N);
        const char *mode = "wb";//如果不存在，就新建一个
        MergeString2(filename,WORKING_DIR,user_id);
        MergeString3(filename,"/data_blocks_file_",num_string,".bin");

        /**验证文件是否存在*/
        if (!access(filename,0) )
            printf("file %s  exist\n",filename);
        else
            CreateDataFile(filename);
        /**打开已有的data文件*/
        FILE *data_blocks_file;
        data_blocks_file = fopen(filename, mode);
        assert(data_blocks_file != NULL);

        /**存储data block*/
        //TODO
        fwrite(merge_buff, data_length + 8,1,data_blocks_file);
        fclose(data_blocks_file);//关闭
        RecordADDRESS();//记录收到第几个block ，绝对地址 16进制，4字节，小端

        /**修改成我们需要的文件名*/
        if((n_block % N) == (N-1)||num_data_block == READ_NUMBER) //比如22个，刚好可以打包成一个dat文件
        {
            char finish_name[100] = {'\0'};
            MergeString2(finish_name,WORKING_DIR,user_id);
            MergeString3(finish_name,"/data_blocks_file_",num_string,".dat");
            rename(filename, finish_name);
            printf("rename:%s",finish_name);
        }
    }
    return num_data_block;
}

void SaveSyncStatusSucess()
{
    FILE *count_file;
    const char *mode = "w";
    char filename[100]={'\0'};
   MergeString3(filename, WORKING_DIR,status.user_bind_info.user_id, "/count.txt");
    /**creat count_file*/
    if (!access(filename,0))
        printf("file %s  exist\n",filename);
    else
        CreateDataFile(filename);
    count_file = fopen(filename,mode);//"w" 写，如果文件存在，把文件截断至0长；如果文件不存在，会创建文件
    assert(count_file != NULL);
    fprintf(count_file,"{");
    fprintf(count_file,"\'total blocks\':\'%d\',",status.total_blocks_num);
    fprintf(count_file,"\'received\':\'%d\',",status.ndata_blocks);
    fprintf(count_file,"}");
    fclose(count_file);
    return;
}

void Log()
{
    //获取时间当前时间
    struct timeval tv;
    char time_str[128];
    gettimeofday(&tv, NULL);//时间初始化
    struct tm *now_time = localtime(&tv.tv_sec);//s

    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", now_time);
    FILE *copy;
    int bak_out = dup(1);// can use fileno(stdout) to replace 1
    printf ("OPEN LOG%d\n", bak_out);
    //char *filename = "serial_read_save.log";
    char *filename = "/root/log/serial_read_save.log";
    /**验证文件是否存在*/
    if (!access(filename,0) )
        printf("file %s  exist\n",filename);
    else
        CreateDataFile(filename);
    /**打开已有的data文件*/
    copy = fopen (filename, "a+");
    dup2(fileno(copy),1);
    printf ("start time:%s\n",time_str);
}
int CheckWORKING_DIR()
{
    MergeString2(WORKING_DIR, HOME_DIR,"/user_data/");
    printf("current usr dir : %s\n", WORKING_DIR);
    if (!access(WORKING_DIR,0) )
        printf("current usr dir : %s  exist\n", WORKING_DIR);
    else
    {
        printf("current usr dir : %s  make dir WORKING_DIR\n", WORKING_DIR);
        mkdir(WORKING_DIR,S_IRWXO|S_IRWXU|S_IRWXG);
    }
    return 0;
}
void  CreateDataFile(char *filename)
{
    if(creat(filename,0755)<0){
        printf("create file %s failure!\n",filename);
        //exit(EXIT_FAILURE);
    }else{
        printf("create file %s success!\n",filename);
    }
}
int RecordADDRESS()
{

    char filename[100]={'\0'};
    const char *mode = "wb";//如果存在，重写
    MergeString3(filename,WORKING_DIR,status.user_bind_info.user_id,"/ADDRESS.bin");
    /**验证文件是否存在*/
    if (!access(filename,0) )
        printf("file %s  exist\n",filename);
    else
        CreateDataFile(filename);

    FILE *address_file;
    address_file = fopen(filename,mode);
    unsigned char address_buff[4]={0x00};
    unsigned char *p=(&ADDRESS);
    for (int i = 0; i < 4; ++i)
    {
        address_buff[i] = *(p+i);
    }
    fwrite(address_buff,4,1,address_file);
    fclose(address_file);
    return ADDRESS;
}
void Rename()
{
    char filename[100] = {'\0'} ;
    MergeString2(filename,WORKING_DIR,status.user_bind_info.user_id);
    int num_data_block = ADDRESS + 1;//一共多少个，包括数据头
    int n_block = (num_data_block  - 2);//第n个，从0 开始，代表第几个block，不包含头
    char num_string[10];
    int num_N = n_block/N;
    sprintf(num_string, "%d", num_N * N);
    MergeString3(filename,"/data_blocks_file_",num_string,".bin");

    if (!access(filename,0))
    {
        printf("file *****.bin %s  exist\n",filename);
        char finish_name[100] = {'\0'};
        MergeString2(finish_name,WORKING_DIR,status.user_bind_info.user_id);
        MergeString3(finish_name,"/data_blocks_file_",num_string,".dat");
        rename(filename, finish_name);
        printf("rename:%s",finish_name);
    }

}

#endif //SERIALPROJECT_DIR_H
