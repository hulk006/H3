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
#include "data.h"


#define ACCESS access
#define MKDIR(a) mkdir((a),0755)

/**
 * @brief  合并字符串
 * @return des为p0+p1
*/
char* MergeString2(char *des,char const *p0,char const *p1)
{
    strcat(des,p0);
    strcat(des,p1);
    return des;
}
char * MergeString3(char *des,char const  *p0,char const  *p1,char const *p2)
{
    strcat(des,p0);
    strcat(des,p1);
    strcat(des,p2);
    return des;
}
char * MergeString4(char *des,char const *p0, char const *p1,char const *p2,char const *p3)
{
    strcat(des,p0);
    strcat(des,p1);
    strcat(des,p2);
    strcat(des,p3);
    return des;
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
    int i = 0;
    int iRet;
    int iLen;
    char* pszDir;

    if(NULL == pDir)
    {
        return 0;
    }

    pszDir = strdup(pDir);//复制一个字符串到副本
    iLen = (int) strlen(pszDir);//长度
    // 创建中间目录
    for (i = 0;i < iLen;++i)
    {
        if (pszDir[i] == '\\' || pszDir[i] == '/')//检测文件目录标志'\\'
        {
            pszDir[i] = '\0';
            //如果不存在,创建access()函数，存在返回0，不存在返回-1
            iRet = ACCESS(pszDir,0);//检查文件的存在性
            if (iRet != 0)
            {
                iRet = MKDIR(pszDir);//建立目录
                if (iRet != 0)
                {
                    return -1;
                }
            }
            //支持linux,将所有\换成/
            pszDir[i] = '/';
        }
    }

    iRet = MKDIR(pszDir);
    free(pszDir);
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
 * @func 存储用户信息，wifi配置信息道文件中
 * @param input_tatus data定义道结构体
 * @return 0
 */
int SaveHeadFile( struct Status const *input_tatus)
{
    char buf[80];
    getcwd(buf,sizeof(buf));
    printf("current working directory: %s\n", buf);

    CreatDir(input_tatus->user_bind_info.user_id);
    FILE *head_file;
    const char *mode = "w";
    char filename[50]={'\0'} ;
    MergeString2(filename,input_tatus->user_bind_info.user_id,"/head_file.txt");
    head_file = fopen(filename,mode);//"w" 写，如果文件存在，把文件截断至0长；如果文件不存在，会创建文件
    assert(head_file != NULL);

    printf("saving user info:\n");
    fprintf(head_file,"{");
    fprintf(head_file,"\'userId\':\'%s\',",input_tatus->user_bind_info.user_id);
    fprintf(head_file,"\'bindState\':\'%d\',",input_tatus->user_bind_info.bind);
    fprintf(head_file,"\'deviceId\':\'%s\',",input_tatus->device_info.name);
    //TODO 设备登录道时候需要的变量
    fprintf(head_file,"\'token\':\'111\',");

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
    static int N = 10;
    //文件名以开始存储的时间命名，将一次读取道blocks数量存储进去，
    char filename[50] = {'\0'} ;
    static int num_data_block = 0;//只初始化一次
    static int num_N = 0;
    num_data_block += data_block->n_data_block;//已经接收了多少个data block
    num_N = num_data_block/N;//根据有多少个100倍数来命名文件
    /** 存数data block 的头文件*/
    if(num_data_block == 1)
    {
        const char *mode = "wb";//如果不存在，就新建一个
        MergeString4(filename,user_id,"/data_blocks_file","_head",".dat");
        //打开已有的data文件
        FILE *data_blocks_file;
        data_blocks_file = fopen(filename, mode);
        assert(data_blocks_file != NULL);
        /**存储data block*/
        //TODO
        fwrite(data_block->rec_buf, sizeof(data_block->rec_buf),1,data_blocks_file);
        //fprintf(data_blocks_file,"%s",data_block->rec_buf);
        fclose(data_blocks_file);//关闭
        return num_data_block;
    }
    /**定义文件名100个存成一个文件*/
    char num_string[10];
    sprintf(num_string, "%d", num_N*N);
    const char *mode = "a+";//追加模式，如果不存在，就新建一个
    MergeString4(filename,user_id,"/data_blocks_file_",num_string,".dat");
    /**打开已有的data文件*/
    FILE *data_blocks_file;
    data_blocks_file = fopen(filename, mode);
    assert(data_blocks_file != NULL);
    /**存储data block*/
    //TODO
    fwrite(data_block->rec_buf, sizeof(data_block->rec_buf),1,data_blocks_file);
    //fprintf(data_blocks_file,"%s",data_block->rec_buf);
    fclose(data_blocks_file);//关闭
    return num_data_block;
}

void SaveSyncStatusSucess(char const *user)
{
    int serial_to_H3_OK = 1;
    FILE *sync_status;
    char num_string[25];
    MergeString2(num_string,user,"/sync_status.txt");
    sync_status = fopen(num_string,"w");
    fprintf(sync_status,"serial_to_H3_OK,%d",serial_to_H3_OK);
    fclose(sync_status);
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
    printf ("OPEN LOG%d\n",bak_out);
    copy = fopen ("log.txt", "w");
    dup2(fileno(copy),1);
    printf ("start time:%s\n",time_str);
}

#endif //SERIALPROJECT_DIR_H
