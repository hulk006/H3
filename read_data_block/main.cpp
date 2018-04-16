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


DYNAMIC_DATA_HEADER * GetDYNAMIC_DATA_HEADER(DYNAMIC_DATA_HEADER *dynamic_data_header,int address, unsigned char *head_buf)
{
    FILE* infile;
    infile = fopen("/home/yh/user_data/unbinduser/data_blocks_file_head.HEAD", "rb");
    if(infile == NULL )
    {
        printf("not exit/n");
        exit(1);
    }
    int rc = 0;
    unsigned char read_buf[66000] = {'\0'};
    rc = fread(read_buf,66000,1 ,infile);
    dynamic_data_header = (DYNAMIC_DATA_HEADER *)(read_buf + 2 + sizeof(STATIC_DATA_BLOCK)+address*sizeof(DYNAMIC_DATA_HEADER));
    uint8_t *time0 = dynamic_data_header->start_time;
    int check0 = dynamic_data_header->checksum;
    return dynamic_data_header;
}





int main() {

    setvbuf(stdout,NULL,_IONBF,0); //冲区禁止了. 它就直接输出了.
    printf ("This is printed to a file!\n");
    STATIC_DATA_BLOCK *data_block ;
    FILE *infile;
    infile = fopen("/home/yh/user_data/unbinduser/data_blocks_file_head.HEAD", "rb");
    if(infile == NULL )
    {
        printf("not exit/n");
        exit(1);
    }
    int rc = 0;
    unsigned char read_buf[66000] = {'\0'};
    rc = fread(read_buf,66000,1 ,infile);

    data_block = (STATIC_DATA_BLOCK *)(read_buf + 2);
    uint8_t *hw_ver=data_block->hw_ver; // Hardware version
    printf("%s\n",hw_ver);
    u_int16_t a = data_block->nand_block_num;
    printf("%d\n",a);


    DYNAMIC_DATA_HEADER *dynamic_data_header_0;
    dynamic_data_header_0 = GetDYNAMIC_DATA_HEADER(dynamic_data_header_0,0,read_buf);
    int address = dynamic_data_header_0->start_addr;
    DYNAMIC_DATA_HEADER *dynamic_data_header_1;
    dynamic_data_header_1 = GetDYNAMIC_DATA_HEADER(dynamic_data_header_1,1,read_buf);
    address = dynamic_data_header_0->start_addr;


    dynamic_data_header_0 = (DYNAMIC_DATA_HEADER *)(read_buf + 2 + sizeof(STATIC_DATA_BLOCK));
    uint8_t *time0 = dynamic_data_header_0->start_time;
    DYNAMIC_DATA_HEADER *dynamic_data_header_2;
    dynamic_data_header_2 = GetDYNAMIC_DATA_HEADER(dynamic_data_header_0,2,read_buf);
    DYNAMIC_DATA_HEADER *dynamic_data_header_3;
    dynamic_data_header_3 = GetDYNAMIC_DATA_HEADER(dynamic_data_header_0,3,read_buf);

    for(int i=0;i<8;i++)
    {
        printf("%x",dynamic_data_header_0->start_time[i]);
    }
    printf("\n");

    int check0 = dynamic_data_header_0->checksum;
    printf("1111:");
    printf("%x\n",check0);

    int check1=dynamic_data_header_1->checksum;
    printf("2222:");
    printf("%x\n",check1);
    fclose(infile);



    infile = fopen("/home/yh/user_data/unbinduser/data_blocks_file_0.dat", "rb");
    unsigned char data_buf[102];
    rc = fread(data_buf,5,1 ,infile);

    for(int i=0;i<102;i++)
    {
        printf("%x ",data_buf[i]);
    }












    return 0;
}