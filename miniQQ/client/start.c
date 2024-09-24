#include "start.h"
// 输入进度条加载秒数
static void printProgressBar()
{

        for (int i = 0; i <= 100; i++)
        {

                printf("[");
                for (int j = 0; j < 100; j++)
                {
                        if (j < i)
                        {
                                printf("=");
                        }
                        else if (j == i)
                        {
                                printf(">");
                        }
                        else
                        {
                                printf(" ");
                        }
                }
                printf("] %.2f%%\r", (double)i);
                fflush(stdout);
                usleep(5000);
        }
        printf("\n");
}
// 启动动画
void boot()
{

        printf("NOW LAODING...\n");
        printProgressBar();
        printf("欢迎使用miniQQ,喵~\n");
        char time_str[21];
        get_now_time(time_str);
        printf("当前时间：%s\n", time_str);
}