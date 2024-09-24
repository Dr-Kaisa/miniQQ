#ifndef __MYNETWORK_H__
#define __MYNETWORK_H__

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include "cJSON.h"
#include "package_type.h"

void mySend(char *send_string, int target_fd);
int myRecv(char *recv_string, int target_fd);
char *generateOKJSON();
char *generateERRORJSON(const char *str);

#endif