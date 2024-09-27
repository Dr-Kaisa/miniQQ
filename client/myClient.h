#ifndef __MYCLIENT_H__
#define __MYCLIENT_H__

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <sys/select.h>
#include <fcntl.h>

#include "start.h"
#include "myClient.h"

#include "../common/cJSON.h"
#include "../common/package_type.h"
#include "../common/myNetwork.h"
#include "../common/myThread.h"

typedef struct User
{
    char nickname[40];
    int account;
    char password[40];
    int socket_fd;
} User;

int init_socket(const char *IP, unsigned int port);
void show_error(cJSON *recv_data);
int get_num_from_json(cJSON *c, char *name);
char *get_str_from_json(cJSON *c, char *name);
#endif