#include <iostream>
#include <zconf.h>
#include <sys/time.h>
#include "data_sruct.h"
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
    copy = fopen ("stdio_test.txt", "w");
    dup2(fileno(copy),1);
    printf ("start time:%s\n",time_str);
}

int main() {
    Log();
    printf ("This is printed to a file!\n");
    STATIC_DATA_BLOCK *data_block ;
    FILE *infile;
    infile = fopen("/home/yh/CLionProjects/serialReadAndSave/unbinduser/data_blocks_file_head.dat", "rb");
    if(infile == NULL )
    {
        printf("not exit/n");
        exit(1);
    }
    int rc = 0;
    unsigned char read_buf[66000] = {0x00};
    rc = fread(read_buf,sizeof(read_buf),1 ,infile);
    data_block = (STATIC_DATA_BLOCK *)(read_buf+2);
    printf("%s",data_block->data_format_ver);
    u_int16_t a = data_block->nand_block_num;
    printf("%d",a);
    fclose(infile);
    //system("PAUSE");
    return 0;
}