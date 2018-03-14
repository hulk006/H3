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

    CreatDir(input_tatus->user_bind_info.user_name);
    //获取时间当前时间
    struct timeval tv;
    char time_str[128];
    gettimeofday(&tv, NULL);//时间初始化
    struct tm *now_time = localtime(&tv.tv_sec);//s
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M", now_time);

    FILE *head_file;
    const char *mode = "w";
    char filename[50]={'\0'} ;
    MergeString2(filename,input_tatus->user_bind_info.user_name,"/head_file.txt");
    head_file = fopen(filename,mode);//"w" 写，如果文件存在，把文件截断至0长；如果文件不存在，会创建文件
    assert(head_file != NULL);

    fprintf(head_file, "this file is save user info:\n");
    fprintf(head_file,"{");
    fprintf(head_file,"user_name:%s,",input_tatus->user_bind_info.user_name);
    fprintf(head_file,"user_id:%s,",input_tatus->user_bind_info.user_id);
    //TODO 设备登录道时候需要的变量
    fprintf(head_file,"token:,");
    fprintf(head_file,"}");

    fprintf(head_file,"{");
    fprintf(head_file,"wifi_name:,");
    fprintf(head_file,"wifi_password:,");
    fprintf(head_file,"}");
    //TODO
    fclose(head_file);
    return 0;
}
/**
 * @func 将一次收到的打他block的字符串存储进文件。存储规则：n*num个data blocks为一个文件
 * 如果少于这个数n*num就接着打开这个文件，如果大于n*num这个数据就重新打开一个文件去存储。
 * @param data_block
 * @param user
 * @return
 */
int SaveDataBlocksFile(const struct DataBlock *data_block,  char const *user)
{
    //文件名以开始存储的时间命名，将一次读取道blocks数量存储进去，
    char filename[50]={'\0'} ;
    //N = 1000 * NUM 个block为一个文件，比如NUM=2 则到达block 到达2000个的时候就停止存储
    static int num_data_block = 0;//只初始化一次
    static int num_1000 = 0;
    if(num_data_block += data_block->n_data_block <= 1000) //如果文件没有达到一个文件的最大block的数量
    {
        //打开已有的data文件
        FILE *data_blocks_file;
        const char *mode = "a+";
        MergeString4(filename,user,"/data_blocks_file","0",".dat");
        data_blocks_file = fopen(filename, mode);//"w" 写，如果文件存在，把文件截断至0长；如果文件不存在，会创建文件
        assert(data_blocks_file != NULL);
        fprintf(data_blocks_file,data_block->rec_buf,num_data_block);
        fclose(data_blocks_file);
    }
    else if (num_data_block > 1000)//如果大于1000，则新开一个文件进行存储
    {
        num_1000++;
        num_data_block = 0;
        FILE *data_blocks_file;
        const char *mode = "a+";
        char num_string[25];
        sprintf(num_string, "%d", num_1000);
        MergeString4(filename,user,"/data_blocks_file",num_string,".dat");
        data_blocks_file = fopen(filename, mode);
        assert(data_blocks_file != NULL);
        num_data_block += data_block->n_data_block;
        fprintf(data_blocks_file,data_block->rec_buf,num_1000*1000 + num_data_block);
        fclose(data_blocks_file);
    }
    //TODO
    return 0;
}
#endif //SERIALPROJECT_DIR_H
