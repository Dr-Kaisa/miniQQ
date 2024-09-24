

// int main()
// {

//     while (1)
//     {
//         // 准备并发送数据
//         cJSON *root = cJSON_CreateObject();

//         cJSON_AddNumberToObject(root, "type", SIGN_UP);
//         cJSON_AddStringToObject(root, "nickname", "kimila");
//         cJSON_AddNumberToObject(root, "account", 121);
//         cJSON_AddStringToObject(root, "groupName", "什么");
//         cJSON_AddNumberToObject(root, "userAccount", 1816087404);
//         cJSON_AddNumberToObject(root, "userAccount", 1816087404);
//         cJSON_AddNumberToObject(root, "groupAccount", 10001);
//         cJSON_AddNumberToObject(root, "targetAccount", 121);
//         cJSON_AddStringToObject(root, "password", "123456");
//         cJSON_AddStringToObject(root, "content", "igasgiaegfiuageifgai");

//         cJSON *members = cJSON_CreateArray();
//         cJSON_AddItemToArray(members, cJSON_CreateNumber(121));
//         cJSON_AddItemToArray(members, cJSON_CreateNumber(123));
//         // cJSON_AddItemToArray(members, cJSON_CreateNumber(124));
//         cJSON_AddItemToObject(root, "members", members);
//         char *json_string = cJSON_Print(root);
//         printf("json_string:%s\n", json_string);
//         mySend(json_string, clifd);
//         json_string = NULL;

//         cJSON_Delete(root);
//         // 等待回应
//         int recv_int = 0;
//         char recv_data[1000];
//         int ret = myRecv(recv_data, clifd);
//         if (ret == -1)
//         {
//             printf("recv failed!\n");
//         }
//         else
//         {
//             printf("recv:%s", recv_data);
//         }
//         sleep(99);
//     }
//     printf("ok\n");
//     close(clifd);

//     return 0;
// }