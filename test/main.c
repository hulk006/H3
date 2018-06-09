#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct  ecg_data
{
    int a;
    int b;
    int c;
    int d;
}ECG_DATA;

int compare(const ECG_DATA *a,const ECG_DATA *b)
{
    return b->a - a->a;//从小到大
}

int main() {

    ECG_DATA origin[5]={1,2,2,5,
                        2,2,2,4,
                        4,2,2,3,
                        4,2,2,2,
                        5,2,2,1};


    qsort(origin,5, sizeof(ECG_DATA),compare);


    for (int i = 0; i < 5; ++i)
    {
     printf("sort:%d",origin[i].a);
    }



    printf("Hello, World!\n");
    return 0;
}