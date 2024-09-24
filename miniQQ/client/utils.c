#include "utils.h"
// 获取当前时间字符串并传给指针
void get_now_time(char *timeString)
{

    time_t current_time;
    struct tm *time_info;

    time(&current_time);
    time_info = localtime(&current_time);

    strftime(timeString, 21, "%Y-%m-%d %H:%M:%S", time_info);
}
// 传入文件指针，返回文件所占字节数,只能计算文件的大小，不能计算文件夹
long int get_file_size(FILE *fp)
{

    int len;
    if (fp == NULL)
    {
        perror("打开文件错误");
        return (-1);
    }
    fseek(fp, 0, SEEK_END);

    len = ftell(fp);

    return len;
}
