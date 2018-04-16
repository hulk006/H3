#include <iostream>
#include <cstring>

/**
 * @func 把16进制的字符转为字符串：如果小于10就转换成数字，如果大于10就转换为字母
 * @param des 含有数字和字母的输出
 * @param hex 16进制字符串
 * @param length 两个字符串的长度
 */
void HexToStr(char *des, const unsigned char *hex,const int length)
{
    for (int i = 0; i < length; ++i)
       des[i] = hex[i]>10 ? hex[i]:'0' + (int)hex[i];
    return;
}

//测试大小端模式
void test_endian()
{
    uint32_t i=0x04030201;
    unsigned char* cp=(unsigned char*)&i; //取16进制i的地址,cp指向
    printf("%s",cp);

    if(*cp==1)//(*cp)代表首地址
        printf("little-endian\n");
    else if(*cp==4)
        printf("big-endian\n");
    else
        printf("who knows?\n");
    //exit(0);
}
void write_int()//把16进制的整数写入
{
    int a=0x01001c00;//写入文件中变为00 1c 00 01,小端模式,从右往左
    FILE* fp=fopen("int.bin","wb");

    fwrite(&a,sizeof(a),1,fp);
    fclose(fp);
    printf("write_int ok!\n");
}
void write_int2()//把十进制的数写入
{
    int a=1024;//小端模式，转换为16进制写入
    FILE* fp=fopen("int1024.bin","wb");
    fwrite(&a,sizeof(a),1,fp);
    fclose(fp);
    printf("write_int2 ok!\n");
}
void write_float()
{
    float a=1024.02;//小端模式,从右往左
    FILE* fp=fopen("float.bin","wb");
    fwrite(&a,sizeof(a),1,fp);
    fclose(fp);
    printf("write_float ok!\n");
}
void write_char()
{
    char a='m';//小端模式,从右往左
    FILE* fp=fopen("char.bin","wb");
    fwrite(&a,sizeof(a),1,fp);
    fclose(fp);
    printf("write_char ok!\n");
}
void write_string2()
{
    char *p;
    FILE *fp;
    char str[32]="example";
    for(p=str;*p!=0;p++)
        *p+=129;//将可见字符转换成不可见字符，不一定是加129
    fp=fopen("string2.txt","w+") ;
    fwrite(str,strlen(str),1,fp);
    fclose(fp);

    fp=fopen("string2.txt","r+") ;
    fread(str,1,strlen(str),fp);
    printf("%s\n",str);//在txt中显示的是乱码
    for(p=str;*p!=0;p++) *p-=129;//转换成可见字符
    printf("%s\n",str);
    fclose(fp);
}

//fwrite(&a,strlen(a),1,fp);//以二进制形式对文件进行操作 不用使用fwrite了
//以文本形式写入
void write_string()
{
    char* a="yunshouhu";//小端模式,从右往左
    FILE* fp=fopen("string.txt","w");
    fprintf(fp,"%s",a);
    fclose(fp);
    printf("write_string ok!\n");
}
//以文本形式读取
void read_string()
{
    char line[1024]={0};
    FILE* fp=fopen("string.txt","r");
    int len=fscanf(fp,"%s",&line);
    fclose(fp);
    printf("read_string=%s len=%d\n",line,len);
}

//默认8的倍数对齐 4+1+10=15=>16
typedef struct Person
{
    int age;
    char sex;
    char city[10];
} Person;

//默认8的倍数对齐 16+4+10=30=>32
typedef struct Student
{
    Person parent;//16
    int weight;
    char classroom[10];
} Student;

//默认8的倍数对齐 4+4+10=18=>24
typedef struct Student2
{
    Person* parent;//存在指针到文件中的话，指针对应的数据会丢失，因为你只存储了他的地址，没有保存他的数据
    int weight;
    char classroom[10];
} Student2;

void write_struct()
{
    Person person;
    person.age=26;
    person.sex='m';
    strcpy(person.city,"shenzhen");

    FILE* fp=fopen("person.struct.bin","wb");
    fwrite(&person,sizeof(person),1,fp);
    fclose(fp);
    printf("write_struct sizeof=%d ok!\n",sizeof(person));//16
}

void write_Student_struct()
{
    Person person;
    person.age=26;
    person.sex='m';
    strcpy(person.city,"shenzhen");

    Student stu;
    stu.parent=person;//16
    stu.weight=140;
    memset(stu.classroom,0,sizeof(stu.classroom));//清零，否则是随机值
    strcpy(stu.classroom,"204");

    FILE* fp=fopen("student.struct.bin","wb");
    fwrite(&stu,sizeof(stu),1,fp);
    fclose(fp);
    printf("write_Student_struct sizeof=%d ok!\n",sizeof(stu));//32
}
void write_Student2_struct()
{
    Person person;
    person.age=26;
    person.sex='m';
    strcpy(person.city,"shenzhen");

    Student2 stu;
    stu.parent=&person;//16
    stu.weight=140;
    memset(stu.classroom,0,sizeof(stu.classroom));//清零，否则是随机值
    strcpy(stu.classroom,"204");

    FILE* fp=fopen("student2.struct.bin","wb");
    fwrite(&stu,sizeof(stu),1,fp);
    fclose(fp);
    printf("write_Student2_struct sizeof=%d ok!\n",sizeof(stu));//24
}
/*
//c语言实现大小端判断和二进制模式写入各种数据到文件。
int main(void)
{

    test_endian();

    write_int();
    write_int2();
    write_float();
    write_char();
    write_string();
    read_string();



    write_struct();
    write_Student_struct();
    write_Student2_struct();
    printf("write ok!\n");



    return 0;
}

*/


int main() {
    //test_endian();
    //write_int();
    //write_string();
    //read_string();
    setvbuf(stdout,NULL,_IONBF,0); //如果你嫌上个方法麻烦, 就使用这个函数. 直接将缓冲区禁止了. 它就直接输出了.
    long int test = 1513668063233;
     int a =0x3e45 ;

    int *b= (&a);
    int *c = (&a+1);

    uint8_t p = *b;

    uint8_t p1 = *b+1;

    printf("%x",p1);
    int nValue = 10;
    float fValue = 10.0f;
    char cValue = 'C';
    int *pnValue = &nValue;

    float *pfValue = &fValue;
    char *pcValue = &cValue;
    printf("pnValue = %x, *pnValue = %d\n", pnValue, *pnValue);
    printf("pfValue = %x, *pfValue = %f\n", pfValue, *pfValue);
    printf("pcValue = %x, *pcValue = %c\n", pcValue, *pcValue);



    char Hex_char[8];
    FILE *test_file;
    test_file = fopen("a.bin","wb");
    fwrite( &test,8,1,test_file);
    fclose(test_file);





    std::cout << "Hello, World!" << std::endl;
    return 0;
}