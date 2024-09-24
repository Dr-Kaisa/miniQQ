#include "myClient.h"
// 建立TCP连接并允许端口复用
int init_socket(const char *IP, unsigned int port)
{
    // 初始化客户端
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    // 客户端绑定地址
    struct sockaddr_in saddr;     // 定义服务器和客户端的地址结构
    saddr.sin_family = AF_INET;   // 设置地址族为 IPv4
    saddr.sin_port = htons(port); // 设置端口号 8000，使用 htons() 将端口号转换为网络字节序
    saddr.sin_addr.s_addr = inet_addr(IP);

    // 允许端口复用
    int opt = 1;
    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    connect(sock_fd, (struct sockaddr *)&saddr, sizeof(saddr));

    return sock_fd;
}

// 将json对象中的errorMessage打印出来
void show_error(cJSON *recv_data)
{
    cJSON *error = cJSON_GetObjectItem(recv_data, "errorMessage");
    printf("%s!\n", error->valuestring);
}

// 从json对象中提取一个整数项，并返回该整数
int get_num_from_json(cJSON *c, char *name)
{
    cJSON *target = cJSON_GetObjectItem(c, name);
    return target->valueint;
}

// 从json对象中提取一个字符项，并返回指向字符串的指针
char *get_str_from_json(cJSON *c, char *name)
{
    cJSON *target = cJSON_GetObjectItem(c, name);
    return target->valuestring;
}
