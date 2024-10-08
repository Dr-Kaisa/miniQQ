#ifndef __MYSERVICE_H__
#define __MYSERVICE_H__

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include "myDB.h"
#include "../common/kernel_list.h"
#include "../common/cJSON.h"
#include "../common/package_type.h "
#include "../common/myNetwork.h"

// 保存参数的结构体
typedef struct
{
    int listen_fd;
    int epoll_fd;
    int disconnect_fd;
    MYSQL *conn;
    char *jsonData;
    int target_fd;
} task_args_t;

void init_socket(int listen_fd);
void task_handleConnection(void *arg);
void task_handleDisconnect(void *arg);
void task_handleMessage(void *arg);

#endif