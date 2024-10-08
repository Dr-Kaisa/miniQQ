#include "menu.h"

void sign_in(User *user, int account, char *pwd);
void login(User *user);
void sign_up(User *user, char *nickname, char *pwd);
void registe(User *user);
void delete_account(User *user);
void show_main_menu(User *user);
void sign_out(User *user);
void show_contacts_list(User *user);
void show_contacts_page(User *user);
void show_contact_menu(User *user, int targetAccount);
void send_message(User *user, int targetAccount);
void show_chat_history(User *user, int targetAccount);
void send_to_server(User *user, int targetAccount, char *content);
void *thread_function(void *arg);
void removeContact(User *user, int targetAccount);
void show_contact_notifications(User *user);
void show_contact_request_menu(User *user, int targetAccount);
void agree_add_contact(User *user, int targetAccount);
void refuse_add_contact(User *user, int targetAccount);
void add_contact(User *user);
void show_groups_page(User *user);
void show_groups_list(User *user);
void show_group_menu(User *user, int targetAccount);
void send_message_to_group(User *user, int targetAccount);
void show_group_chat_history(User *user, int targetAccount);
void send_group_message_to_server(User *user, int targetAccount, char *content);
void *thread_function_group_chat(void *arg);
void invite_add_group(User *user, int targetAccount);
void remove_group(User *user, int targetAccount);
void create_group(User *user);
void show_group_notifications(User *user);
void show_group_invite_menu(User *user, int id, int groupAccount);
void agree_join_group(User *user, int id, int groupAccount);
void refuse_join_group(User *user, int id);

// 显示登录页面菜单
void show_login_menu(User *user)
{
    int section = 3;
    do
    {
        printf("==============================================================================================================\n");
        printf("请选择你的的操作:\n\t1、登录\n\t2、注册账号\n\t3、退出应用\n");
        scanf("%d", &section);
        getchar();
        switch (section)
        {
        case 1:
            printf("登录\n");
            login(user);
            break;
        case 2:
            printf("注册\n");
            registe(user);

        case 3:
            printf("正在退出...\n");
            break;
        default:
            printf("请输入正确数字\n");
            break;
        }
    } while (section != 3);
}
// 获取账号密码并登录
void login(User *user)
{
    // 输入账号密码进行验证
    int account;
    char pwd[40];
    printf("请依次输入账号，密码\n");
    scanf("%d%s", &account, pwd);
    getchar();
    sign_in(user, account, pwd);
}
// 登录
void sign_in(User *user, int account, char *pwd)
{
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", SIGN_IN);
    cJSON_AddNumberToObject(send_data, "userAccount", account);
    cJSON_AddStringToObject(send_data, "password", pwd);

    char *send_str = cJSON_Print(send_data);
    puts(send_str);
    mySend(send_str, user->socket_fd);
    send_str = NULL;

    cJSON_Delete(send_data);
    // 等待回应

    char *recv_str = myRecv(user->socket_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功,解析JSON字符串,看看是否登录成功
    {

        cJSON *recv_data = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (recv_data == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        cJSON *jsonType = cJSON_GetObjectItem(recv_data, "type");
        int type = jsonType->valueint;
        if (type == REPLY_SIGN_IN) // 成功，
        {
            // 保存用户信息

            user->account = account;
            strcpy(user->password, pwd);
            strcpy(user->nickname, get_str_from_json(recv_data, "nickname"));

            cJSON_Delete(recv_data);
            // 进入主界面
            show_main_menu(user);
        }
        else if (get_num_from_json(recv_data, "type") == ERROR) // 不成功，显示错误信息
        {
            show_error(recv_data);
            cJSON_Delete(recv_data);
        }
    }
}
// 获取昵称密码并进行注册
void registe(User *user)
{
    // 输入昵称密码进行注册
    char nickname[40];
    char pwd[40];
    printf("请依次输入昵称，密码\n");
    scanf("%s%s", nickname, pwd);
    getchar();
    sign_up(user, nickname, pwd);
}
// 注册
void sign_up(User *user, char *nickname, char *pwd)
{

    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", SIGN_UP);
    cJSON_AddStringToObject(send_data, "nickname", nickname);
    cJSON_AddStringToObject(send_data, "password", pwd);

    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);
    send_str = NULL;

    cJSON_Delete(send_data);
    // 等待回应

    char *recv_str = myRecv(user->socket_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功,解析JSON字符串,看看是否登录成功
    {

        cJSON *recv_data = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (recv_data == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(recv_data, "type") == SEND_ACCOUNT) // 成功，
        {
            // 保存用户信息
            printf("恭喜你，注册成功\n");
            printf("你的账号为：%d\n", get_num_from_json(recv_data, "userAccount"));
            sleep(1);
            sign_in(user, get_num_from_json(recv_data, "userAccount"), pwd);
        }
        else if (get_num_from_json(recv_data, "type") == ERROR) // 不成功，显示错误信息
        {
            show_error(recv_data);
        }
        cJSON_Delete(recv_data);
    }
}
// 注销账号
void delete_account(User *user)
{
    // 输入账号
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", DELETE_ACCOUNT);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);

    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);
    send_str = NULL;

    cJSON_Delete(send_data);
    // 等待回应

    char *recv_str = myRecv(user->socket_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功,解析JSON字符串,看看是否注销成功
    {

        cJSON *recv_data = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (recv_data == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(recv_data, "type") == OK) // 成功，
        {
            // 保存用户信息
            printf("已注销\n");
            sleep(1);
        }
        else if (get_num_from_json(recv_data, "type") == ERROR) // 不成功，显示错误信息
        {
            show_error(recv_data);
        }
        cJSON_Delete(recv_data);
    }
    close(user->socket_fd);
}
// 进入主界面
void show_main_menu(User *user)
{
    int section = 5;
    do
    {
        printf("==============================================================================================================\n");
        printf("请选择你的的操作:\n\t1、联系人\n\t2、群聊\n\t3、注销账号\n\t4、退出登录\n\t");
        scanf("%d", &section);
        getchar();
        switch (section)
        {
        case 1:
            printf("联系人\n");
            show_contacts_page(user);
            break;
        case 2:
            printf("群聊\n");
            show_groups_page(user);
            break;
        case 3:
            printf("注销账号\n");
            delete_account(user);
            break;
        case 4:
            sign_out(user);
            break;
        default:
            printf("请输入正确数字\n");
            break;
        }
    } while (section != 3 && section != 4);
}
// 退出登录
void sign_out(User *user)
{

    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", SIGN_OUT);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);

    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);
    send_str = NULL;

    cJSON_Delete(send_data);
    // 等待回应
    char *recv_str = myRecv(user->socket_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功,解析JSON字符串,看看是否登录成功
    {

        cJSON *recv_data = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (recv_data == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(recv_data, "type") == OK) // 成功，
        {
            // 保存用户信息
            printf("已退出登录\n");
        }
        else if (get_num_from_json(recv_data, "type") == ERROR) // 不成功，显示错误信息
        {
            show_error(recv_data);
        }
        cJSON_Delete(recv_data);
    }
}
// 展示登录界面
void show_contacts_page(User *user)
{
    int section = 4;
    do
    {
        printf("==============================================================================================================\n");
        printf("请选择你的的操作:\n\t1、我的好友\n\t2、好友验证\n\t3、添加好友\n\t4、返回\n\t");
        scanf("%d", &section);
        getchar();
        switch (section)
        {
        case 1:
            printf("我的好友\n");
            show_contacts_list(user);
            break;
        case 2:
            printf("好友验证\n");
            show_contact_notifications(user);
            break;
        case 3:
            printf("添加好友\n");
            add_contact(user);
            break;
        case 4:
            printf("返回\n");
            break;
        default:
            printf("请输入正确数字\n");
            break;
        }
    } while (section != 4);
}
// 获取联系人列表并显示
void show_contacts_list(User *user)
{
    // 发送申请
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", GET_CONTACTS);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);

    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);
    send_str = NULL;

    cJSON_Delete(send_data);
    // 等待回应
    char *recv_str = myRecv(user->socket_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(root, "type") == REPLY_GET_CONTACTS) // 成功，
        {

            printf("==============================================================================================================\n");
            printf("昵称                账号                在线状态          \n");

            cJSON *jsonMembers = cJSON_GetObjectItem(root, "friends");
            int len = cJSON_GetArraySize(jsonMembers);
            // 逐一展示联系人列表
            for (int i = 0; i < len; i++)
            {
                cJSON *data = cJSON_GetArrayItem(jsonMembers, i);
                printf("%-20s%-20d%-20s\n", get_str_from_json(data, "nickname"), get_num_from_json(data, "account"), (get_num_from_json(data, "isOnline") ? "在线" : "离线"));
            }
        }
        else if (get_num_from_json(root, "type") == ERROR) // 不成功，显示错误信息
        {
            show_error(root);
        }

        cJSON_Delete(root);
    }

    printf("请输入待操作联系人账号\n");
    int targetAccount = 0;
    scanf("%d", &targetAccount);
    getchar();
    show_contact_menu(user, targetAccount);
}
// 右键联系人进行操作
void show_contact_menu(User *user, int targetAccount)
{
    int section = 3;
    do
    {
        printf("==============================================================================================================\n");
        printf("请选择你的的操作:\n\t1、发送消息\n\t2、删除好友\n\t3、返回\n");
        scanf("%d", &section);
        getchar();
        switch (section)
        {
        case 1:
            printf("发送消息\n");
            send_message(user, targetAccount);
            break;
        case 2:
            printf("删除好友\n");
            removeContact(user, targetAccount);
            break;
        case 3:
            printf("返回\n");
            break;

        default:
            printf("请输入正确数字\n");
            break;
        }
    } while (section != 3);
}
// 线程函数，用来持续监听群消息
void *thread_function(void *arg)
{

    int *socket_fd = (int *)arg;
    // 设置为异步取消类型
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    // 持续接收消息
    while (1)
    {
        char *recv_str = myRecv(*socket_fd);

        if (recv_str == NULL)
        {

            printf("接收失败\n");
        }
        else // 接收成功
        {
            // 创建一个名为root的JSON节点
            cJSON *data = cJSON_Parse(recv_str);
            free(recv_str);
            recv_str = NULL;
            // 打印
            printf("%s:%-100s%-20s\n", get_str_from_json(data, "nickname"), get_str_from_json(data, "content"), get_str_from_json(data, "sendTime"));
            // 清理
            cJSON_Delete(data);
        }
    }
}
// 给目标联系人发消息
void send_message(User *user, int targetAccount)
{
    // 显示历史消息记录
    show_chat_history(user, targetAccount);

    // 单开一个连接用来收消息
    int new_fd = init_socket("192.168.174.100", 8080);
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();
    cJSON_AddNumberToObject(send_data, "type", REQUEST_PRIVATE_CHAT);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    char *send_str = cJSON_Print(send_data);
    mySend(send_str, new_fd);
    char *recv_str = myRecv(new_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }

        cJSON_Delete(root);
    }

    // 单开一个线程，用来接收对方发的消息
    pthread_t thread;
    // 创建一个线程
    if (pthread_create(&thread, NULL, thread_function, (void *)&new_fd) != 0)
    {
        perror("pthread_create");
        return;
    }
    // 主线程发送消息
    char content[100] = {0}; // 最大消息长度假设不超过100字节

    while (scanf("%s", content))
    {
        if (strcmp(content, "exit") == 0) // 输入exit表示终止输入
        {
            break;
        }
        send_to_server(user, targetAccount, content);
    }

    // 清理
    pthread_cancel(thread); // 请求立即取消线程
    cJSON_Delete(send_data);
    pthread_join(thread, NULL); // 等待线程结束并回收资源
    close(new_fd);
}
// 显示历史消息
void show_chat_history(User *user, int targetAccount)
{
    // 发送申请
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", PRIVATE_CHAT);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "targetAccount", targetAccount);

    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);
    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);
    // 等待回应
    char *recv_str = myRecv(user->socket_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(root, "type") == REPLY_PRIVATE_CHAT) // 成功，
        {

            cJSON *jsonMembers = cJSON_GetObjectItem(root, "messages");
            int len = cJSON_GetArraySize(jsonMembers);
            // 逐一展示联系人列表
            for (int i = 0; i < len; i++)
            {
                cJSON *data = cJSON_GetArrayItem(jsonMembers, i);
                printf("%s:%-100s%-20s\n", get_str_from_json(data, "nickname"), get_str_from_json(data, "content"), get_str_from_json(data, "sendTime"));
            }
        }
        else if (get_num_from_json(root, "type") == ERROR) // 不成功，显示错误信息
        {
            show_error(root);
        }

        cJSON_Delete(root);
    }
}
// 将消息发送至服务器
void send_to_server(User *user, int targetAccount, char *content)
{
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", SEND_PRIVATE_MES);
    cJSON_AddStringToObject(send_data, "nickname", user->nickname);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "targetAccount", targetAccount);
    cJSON_AddStringToObject(send_data, "content", content);
    char time[30];
    get_now_time(time);
    cJSON_AddStringToObject(send_data, "sendTime", time);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 发送失败，显示错误信息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }

        cJSON_Delete(root);
    }
}
// 删除好友
void removeContact(User *user, int targetAccount)
{
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", REMOVE_CONTACT);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "targetAccount", targetAccount);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 发送失败，显示错误信息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }
        else if (get_num_from_json(root, "type") == OK)
        {
            printf("删除成功！\n");
        }
        cJSON_Delete(root);
    }
}
// 显示验证消息列表
void show_contact_notifications(User *user)
{
    // 发送申请
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", GET_CONTACT_NOTIFICATIONS);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);

    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);
    send_str = NULL;

    cJSON_Delete(send_data);
    // 等待回应
    char *recv_str = myRecv(user->socket_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(root, "type") == REPLY_GET_CONTACT_NOTIFICATIONS) // 成功，
        {

            printf("==============================================================================================================\n");
            printf("昵称                账号                时间          \n");

            cJSON *jsonRequests = cJSON_GetObjectItem(root, "requests");
            int len = cJSON_GetArraySize(jsonRequests);
            // 逐一展示联系人列表
            for (int i = 0; i < len; i++)
            {
                cJSON *data = cJSON_GetArrayItem(jsonRequests, i);
                printf("%-20s%-20d%-20s\n", get_str_from_json(data, "nickname"), get_num_from_json(data, "targetAccount"), get_str_from_json(data, "time"));
            }
            printf("请输入待操作申请人账号\n");
            int targetAccount = 0;
            scanf("%d", &targetAccount);
            getchar();
            show_contact_request_menu(user, targetAccount);
        }
        else if (get_num_from_json(root, "type") == ERROR) // 不成功，显示错误信息
        {
            show_error(root);
        }

        cJSON_Delete(root);
    }
}
// 右键好友申请进行操作
void show_contact_request_menu(User *user, int targetAccount)
{
    int section = 3;
    do
    {
        printf("==============================================================================================================\n");
        printf("请选择你的的操作:\n\t1、同意申请\n\t2、拒绝\n\t3、返回\n");
        scanf("%d", &section);
        getchar();
        switch (section)
        {
        case 1:
            printf("同意\n");
            agree_add_contact(user, targetAccount);
            break;
        case 2:
            printf("拒绝\n");
            refuse_add_contact(user, targetAccount);
            break;
        case 3:
            printf("返回\n");
            break;

        default:
            printf("请输入正确数字\n");
            break;
        }
    } while (section != 1 && section != 2 && section != 3);
}
// 同意好友申请
void agree_add_contact(User *user, int targetAccount)
{
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", AGREE_ADD_CONTACT);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "targetAccount", targetAccount);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 显示返回消息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }
        else if (get_num_from_json(root, "type") == OK)
        {
            printf("已成功添加好友！\n");
        }
        cJSON_Delete(root);
    }
}
// 拒绝好友申请
void refuse_add_contact(User *user, int targetAccount)
{
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", REFUSE_ADD_CONTACT);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "targetAccount", targetAccount);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 显示返回消息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }
        else if (get_num_from_json(root, "type") == OK)
        {
            printf("成功拒绝申请\n");
        }
        cJSON_Delete(root);
    }
}
// 添加好友
void add_contact(User *user)
{
    printf("请输入账号\n");
    int account = 0;
    scanf("%d", &account);
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", ADD_CONTACT);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "targetAccount", account);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 显示返回消息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }
        else if (get_num_from_json(root, "type") == OK)
        {
            printf("成功发送好友申请\n");
        }
        cJSON_Delete(root);
    }
}
// 显示群聊模块
void show_groups_page(User *user)
{
    int section = 4;
    do
    {
        printf("==============================================================================================================\n");
        printf("请选择你的的操作:\n\t1、我的群聊\n\t2、群通知\n\t3、创建群聊\n\t4、返回\n\t");
        scanf("%d", &section);
        getchar();
        switch (section)
        {
        case 1:
            printf("我的群聊\n");
            show_groups_list(user);
            break;
        case 2:
            printf("群通知\n");
            show_group_notifications(user);
            break;
        case 3:
            printf("创建群聊\n");
            create_group(user);
            break;
        case 4:
            printf("返回\n");
            break;
        default:
            printf("请输入正确数字\n");
            break;
        }
    } while (section != 4);
}
// 显示群聊列表
void show_groups_list(User *user)
{
    // 发送申请
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", GET_GROUPS);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);

    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);
    send_str = NULL;

    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {

        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(root, "type") == REPLY_GET_GROUPS) // 成功，
        {

            printf("==============================================================================================================\n");
            printf("群名                         群号                \n");

            cJSON *jsonMembers = cJSON_GetObjectItem(root, "groups");
            int len = cJSON_GetArraySize(jsonMembers);
            // 逐一展示联系人列表
            for (int i = 0; i < len; i++)
            {
                cJSON *data = cJSON_GetArrayItem(jsonMembers, i);
                printf("%-36s%d\n", get_str_from_json(data, "name"), get_num_from_json(data, "group_account"));
            }
        }
        else if (get_num_from_json(root, "type") == ERROR) // 不成功，显示错误信息
        {

            show_error(root);
        }

        cJSON_Delete(root);
    }

    printf("请输入待操作群的群号\n");
    int targetAccount = 0;
    scanf("%d", &targetAccount);
    getchar();
    show_group_menu(user, targetAccount);
}
// 右键群，选择操作
void show_group_menu(User *user, int targetAccount)
{
    int section = 3;
    do
    {
        printf("==============================================================================================================\n");
        printf("请选择你的的操作:\n\t1、发送消息\n\t2、邀请好友加入\n\t3、退出群聊\n\t4、返回\n");
        scanf("%d", &section);
        getchar();
        switch (section)
        {
        case 1:
            printf("发送消息\n");
            send_message_to_group(user, targetAccount);
            break;
        case 2:
            printf("邀请好友加入\n");
            invite_add_group(user, targetAccount);
            break;
        case 3:
            printf("退出群聊\n");
            remove_group(user, targetAccount);
            break;
        case 4:
            printf("返回\n");
            break;

        default:
            printf("请输入正确数字\n");
            break;
        }
    } while (section != 3 && section != 4);
}
// 持续监听群消息
void *thread_function_group_chat(void *arg)
{

    int *socket_fd = (int *)arg;
    // 设置为异步取消类型
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    // 持续接收消息
    char *recv_str = NULL;
    while (1)
    {
        recv_str = myRecv(*socket_fd);

        if (recv_str == NULL) // 接收成功
        {

            printf("接收失败\n");
        }
        else
        {
            // 创建一个名为root的JSON节点
            cJSON *data = cJSON_Parse(recv_str);
            // 打印
            printf("%s:%-100s%-20s\n", get_str_from_json(data, "nickname"), get_str_from_json(data, "content"), get_str_from_json(data, "sendTime"));
            // 清理
            cJSON_Delete(data);
        }
    }
}
// 进行群聊
void send_message_to_group(User *user, int targetAccount)
{
    // 显示历史消息记录
    show_group_chat_history(user, targetAccount);

    // 单开一个连接用来收消息
    int new_fd = init_socket("192.168.174.100", 8080);
    // 连接失败则退出
    if (new_fd == -1)
    {
        return;
    }
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();
    cJSON_AddNumberToObject(send_data, "type", REQUEST_GROUP_CHAT);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    char *send_str = cJSON_Print(send_data);
    mySend(send_str, new_fd);
    char *recv_str = myRecv(new_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }

        cJSON_Delete(root);
    }

    // 单开一个线程，用来接收对方发的消息
    pthread_t thread;
    // 创建一个线程
    if (pthread_create(&thread, NULL, thread_function_group_chat, (void *)&new_fd) != 0)
    {
        perror("pthread_create");
        return;
    }
    // 主线程发送消息
    char content[100] = {0}; // 最大消息长度假设不超过100字节

    while (scanf("%s", content))
    {
        if (strcmp(content, "exit") == 0) // 输入exit表示终止输入
        {
            break;
        }
        send_group_message_to_server(user, targetAccount, content);
    }

    // 清理
    pthread_cancel(thread); // 请求立即取消线程
    cJSON_Delete(send_data);
    pthread_join(thread, NULL); // 等待线程结束并回收资源
    close(new_fd);
}
// 显示群聊历史消息
void show_group_chat_history(User *user, int targetAccount)
{
    // 发送申请
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", GROUP_CHAT);
    cJSON_AddNumberToObject(send_data, "groupAccount", targetAccount);

    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);
    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);
    // 等待回应
    char *recv_str = myRecv(user->socket_fd);
    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(root, "type") == REPLY_GROUP_CHAT) // 成功
        {

            cJSON *jsonMembers = cJSON_GetObjectItem(root, "messages");
            int len = cJSON_GetArraySize(jsonMembers);
            // 逐一解析群消息
            for (int i = 0; i < len; i++)
            {
                cJSON *data = cJSON_GetArrayItem(jsonMembers, i);
                printf("%s:%-100s%-20s\n", get_str_from_json(data, "nickname"), get_str_from_json(data, "content"), get_str_from_json(data, "sendTime"));
            }
        }
        else if (get_num_from_json(root, "type") == ERROR) // 不成功，显示错误信息
        {
            show_error(root);
        }

        cJSON_Delete(root);
    }
}
// 将群消息发送至服务器
void send_group_message_to_server(User *user, int targetAccount, char *content)
{
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", SEND_GROUP_MES);
    cJSON_AddStringToObject(send_data, "nickname", user->nickname);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "groupAccount", targetAccount);
    cJSON_AddStringToObject(send_data, "content", content);
    char time[30];
    get_now_time(time);
    cJSON_AddStringToObject(send_data, "sendTime", time);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 发送失败，显示错误信息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }

        cJSON_Delete(root);
    }
}
// 邀请好友加入群聊
void invite_add_group(User *user, int targetAccount)
{
    printf("请输入好友账号\n");
    int account = 0;
    scanf("%d", &account);
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", INVITE_TO_GROUP);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "targetAccount", account);
    cJSON_AddNumberToObject(send_data, "groupAccount", targetAccount);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 显示返回消息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }
        else if (get_num_from_json(root, "type") == OK)
        {
            printf("成功发送邀请\n");
        }
        cJSON_Delete(root);
    }
}
// 退出群聊
void remove_group(User *user, int targetAccount)
{

    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", LEAVE_GROUP);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "groupAccount", targetAccount);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 显示返回消息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }
        else if (get_num_from_json(root, "type") == OK)
        {
            printf("成功退出群聊\n");
        }
        cJSON_Delete(root);
    }
}
// 创建群聊（群名，账号1,账号2,账号3...）————return ok
void create_group(User *user)
{
    printf("请输入群名\n");
    char groupName[30] = {0};
    scanf("%s", groupName);
    cJSON *members = cJSON_CreateArray();
    printf("请输入要邀请的好友账号(输入0表示终止)\n");
    int account = 0;

    while (scanf("%d", &account))
    {
        if (account == 0) // 输入0
        {
            break;
        }
        cJSON_AddItemToArray(members, cJSON_CreateNumber(account));
    }

    cJSON *send_data = cJSON_CreateObject();
    cJSON_AddNumberToObject(send_data, "type", CREATE_GROUP);
    cJSON_AddStringToObject(send_data, "groupName", groupName);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddItemToObject(send_data, "members", members);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 显示返回消息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }
        else if (get_num_from_json(root, "type") == OK)
        {
            printf("成功发送邀请\n");
        }
        cJSON_Delete(root);
    }
}
// 获取群聊验证消息表（本人账号）———返回验证消息表
void show_group_notifications(User *user)
{
    // 发送申请
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", GET_GROUP_NOTIFICATIONS);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);

    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);
    send_str = NULL;

    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {

        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }

        if (get_num_from_json(root, "type") == REPLY_GET_GROUP_NOTIFICATIONS) // 成功，
        {

            printf("==============================================================================================================\n");
            printf("邀请编号            邀请人              邀请人账号          群名           群号                邀请时间                    \n");

            cJSON *jsonMembers = cJSON_GetObjectItem(root, "invitations");
            int len = cJSON_GetArraySize(jsonMembers);
            // 逐一展示联系人列表
            for (int i = 0; i < len; i++)
            {
                cJSON *data = cJSON_GetArrayItem(jsonMembers, i);
                printf("%-20d%-20s%-20d%-20s%-20d%-20s\n", get_num_from_json(data, "inviteId"), get_str_from_json(data, "inviteUserName"), get_num_from_json(data, "inviteUserAccount"), get_str_from_json(data, "targetGroupName"), get_num_from_json(data, "targetGroupAccount"), get_str_from_json(data, "time"));
            }
            printf("请输入待操作邀请的编号,群号\n");
            int id = 0;
            int groupAccount = 0;
            scanf("%d%d", &id, &groupAccount);
            show_group_invite_menu(user, id, groupAccount);
        }
        else if (get_num_from_json(root, "type") == ERROR) // 不成功，显示错误信息
        {

            show_error(root);
        }

        cJSON_Delete(root);
    }
}
// 右键群邀请进行操作
void show_group_invite_menu(User *user, int id, int groupAccount)
{
    int section = 3;
    do
    {
        printf("==============================================================================================================\n");
        printf("请选择你的的操作:\n\t1、同意申请\n\t2、拒绝\n\t3、返回\n");
        scanf("%d", &section);
        getchar();
        switch (section)
        {
        case 1:
            printf("同意\n");
            agree_join_group(user, id, groupAccount);
            break;
        case 2:
            printf("拒绝\n");
            refuse_join_group(user, id);
            break;
        case 3:
            printf("返回\n");
            break;

        default:
            printf("请输入正确数字\n");
            break;
        }
    } while (section != 1 && section != 2 && section != 3);
}
// 同意加入群聊
void agree_join_group(User *user, int id, int groupAccount)
{
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", AGREE_JOIN_GROUP);
    cJSON_AddNumberToObject(send_data, "inviteId", id);
    cJSON_AddNumberToObject(send_data, "userAccount", user->account);
    cJSON_AddNumberToObject(send_data, "groupAccount", groupAccount);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 显示返回消息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }
        else if (get_num_from_json(root, "type") == OK)
        {
            printf("成功加入群聊！\n");
        }
        cJSON_Delete(root);
    }
}
// 拒绝加入群聊.
void refuse_join_group(User *user, int id)
{
    // JSON序列化
    cJSON *send_data = cJSON_CreateObject();

    cJSON_AddNumberToObject(send_data, "type", REFUSE_JOIN_GROUP);
    cJSON_AddNumberToObject(send_data, "inviteId", id);
    // 转化为字符串并发送
    char *send_str = cJSON_Print(send_data);

    mySend(send_str, user->socket_fd);

    // 清理
    send_str = NULL;
    cJSON_Delete(send_data);

    // 等待回应
    char *recv_str = myRecv(user->socket_fd);

    if (recv_str == NULL) // 接收失败
    {
        printf("recv failed!\n");
        return;
    }
    else // 接收成功
    {
        // 创建一个名为root的JSON节点
        cJSON *root = cJSON_Parse(recv_str);
        free(recv_str);
        recv_str = NULL;
        if (root == NULL)
        {
            printf("parse JSON failed!\n");
            return;
        }
        // 显示返回消息
        if (get_num_from_json(root, "type") == ERROR)
        {
            show_error(root);
        }
        else if (get_num_from_json(root, "type") == OK)
        {
            printf("成功拒绝申请\n");
        }
        cJSON_Delete(root);
    }
}