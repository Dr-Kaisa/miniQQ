#include "myNetwork.h"
void mySend(char *send_string, int target_fd)
{
    int len = strlen(send_string);
    int network_int = htonl(len);
    write(target_fd, &network_int, sizeof(int));
    write(target_fd, send_string, len);
    free(send_string);
}

int myRecv(char *recv_string, int target_fd)
{
    int recv_int = 0;
    if (read(target_fd, &recv_int, 4) == 4) // 读到数据则处理数据
    {

        int host_int = ntohl(recv_int);
        read(target_fd, recv_string, host_int);
        recv_string[host_int] = '\0';
        return 0;
    }
    else
    {
        return -1;
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
