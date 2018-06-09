#include <iostream>
#include <zconf.h>
#include <sys/time.h>
#include "data_sruct.h"

DYNAMIC_DATA_HEADER dynamic_data_header[1000]={'\0'};


int DATA_NUMBER = 0;

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
    infile = fopen("/home/yh/check_data/user_data/13800138000/data_blocks_file_head.HEAD", "rb");
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


int Get_Dynamic_Data_Header(DYNAMIC_DATA_HEADER *dynamic_data_header_all)
{

    FILE* infile;
    char file_name[100]={'\0'};
    //MergeString3(file_name,WORKING_DIR,status.user_bind_info.user_id,"/data_blocks_file_head.HEAD");
    infile = fopen("/home/yh/user_data/E9A794B15333/data_blocks_file_head.HEAD", "rb");
    if(infile == NULL )
    {
        printf("not exit/n");
        exit(1);
    }
    int rc = 0;
    unsigned char read_buf[66000] = {'\0'};
    rc = fread(read_buf,66000,1 ,infile);
    for (int i = 0; i < 1000; ++ i)
    {
        DYNAMIC_DATA_HEADER *dynamic_data_header;
        dynamic_data_header = (DYNAMIC_DATA_HEADER *)(read_buf + 2 + sizeof(STATIC_DATA_BLOCK)+i*sizeof(DYNAMIC_DATA_HEADER));
        if (dynamic_data_header->checksum == 305419896)
        {
            DATA_NUMBER ++;
            dynamic_data_header_all[i]=*dynamic_data_header;
        }
        else{
            break;
        }
    }
    return DATA_NUMBER;
}

uint32_t Get_Blocks_Address(int index)
{
    uint32_t address = dynamic_data_header[index].start_addr;
    return address;

}



int main() {

    setvbuf(stdout,NULL,_IONBF,0);

    printf ("This is printed to a file!\n");
    STATIC_DATA_BLOCK *data_block ;
    FILE *infile;
    infile = fopen("/home/yh/user_data/E9A794B15333/data_blocks_file_head.HEAD", "rb");
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
    DATA_NUMBER = Get_Dynamic_Data_Header(dynamic_data_header);
    int number=DATA_NUMBER;

    for (int j = 0; j < DATA_NUMBER; ++j)
    {
        uint32_t address = dynamic_data_header[j].start_addr;
        uint8_t type = dynamic_data_header[j].type;

        printf("index=%d,address=%d type = %d,length = %d\n",j,address,type,dynamic_data_header[j].length);
        long *time_stamp1;
        time_stamp1= (long *)(dynamic_data_header[j].start_time);
        uint32_t time2=0;
        for (int i = 0; i < 8; ++i)
        {
         printf("%x ",dynamic_data_header[j].start_time[i]);
            time2 += dynamic_data_header[j].start_time[i]<<(8*i);

        }
        printf("\n");

    }





    return 0;
}