#include "myNetwork.h"
// 将目标字符串发送至目标地址，并释放堆内存
void mySend(char *send_string, int target_fd)
{
    int len = strlen(send_string);
    int network_int = htonl(len);
    write(target_fd, &network_int, sizeof(int));
    write(target_fd, send_string, len);
    free(send_string);
}
// 从目标地址接收消息，并返回指针
char *myRecv(int target_fd)
{
    int recv_int = 0;
    if (read(target_fd, &recv_int, 4) == 4) // 读到数据则处理数据
    {
        int host_int = ntohl(recv_int);
        char *recv_string = malloc(host_int * sizeof(char) + 1);
        read(target_fd, recv_string, host_int);
        recv_string[host_int] = '\0';
        return recv_string;
    }
    else
    {
        return NULL;
    }
}

char *generateOKJSON()
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "type", OK);
    char *json_string = cJSON_Print(root);
    cJSON_Delete(root);
    return json_string;
}

char *generateERRORJSON(const char *str)
{
    cJSON *root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "type", ERROR);
    cJSON_AddStringToObject(root, "errorMessage", str);
    char *json_string = cJSON_Print(root);
    cJSON_Delete(root);
    return json_string;
}
