#include "myService.h"

void handleSignUp(cJSON *root, int target_fd, MYSQL *conn);
void handleSignIn(cJSON *root, int target_fd, MYSQL *conn);
void handleDeleteAccount(cJSON *root, int target_fd, MYSQL *conn);
void handleReturnContacts(cJSON *root, int target_fd, MYSQL *conn);
void handleRemoveContact(cJSON *root, int target_fd, MYSQL *conn);
void handlePrivateChat(cJSON *root, int target_fd, MYSQL *conn);
void handlePrivateChatRequst(cJSON *root, int target_fd, MYSQL *conn);
void handleSendMessage(char *jsonData, cJSON *root, int target_fd, MYSQL *conn);
void handleAddContact(cJSON *root, int target_fd, MYSQL *conn);
void handleGetContactNotifications(cJSON *root, int target_fd, MYSQL *conn);
void handleAgreeAddContact(cJSON *root, int target_fd, MYSQL *conn);
void handleRefuseAddContact(cJSON *root, int target_fd, MYSQL *conn);
void handleGetGroups(cJSON *root, int target_fd, MYSQL *conn);
void handleGroupChat(cJSON *root, int target_fd, MYSQL *conn);
void handleGroupChatRequst(cJSON *root, int target_fd, MYSQL *conn);
void handleInviteToGroup(cJSON *root, int target_fd, MYSQL *conn);
void handleSendGroupMessage(char *jsonData, cJSON *root, int target_fd, MYSQL *conn);
void handleLeaveGroup(cJSON *root, int target_fd, MYSQL *conn);
void handleCreateGroup(cJSON *root, int target_fd, MYSQL *conn);
void handleGetGroupNotifications(cJSON *root, int target_fd, MYSQL *conn);
void handleAgreeJoinGroup(cJSON *root, int target_fd, MYSQL *conn);
void handleRefuseJoinGroup(cJSON *root, int target_fd, MYSQL *conn);
void handleSignOut(cJSON *root, int target_fd, MYSQL *conn);

// 初始化服务器
void init_socket(int listen_fd)
{
    struct sockaddr_in service_addr;
    service_addr.sin_family = AF_INET;
    service_addr.sin_port = htons(8080);
    // 绑定本地IP，只接收来自本地的网络请求
    if (inet_aton("127.0.0.1", &service_addr.sin_addr) == 0)
    {
        printf("Invalid IP address\n");
        return;
    }
    if (bind(listen_fd, (struct sockaddr *)&service_addr, (socklen_t)sizeof(service_addr)) == -1)
    {
        perror("bind");
        return;
    }
    // 允许端口复用
    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    // 监听
    listen(listen_fd, 100);
}
// 处理新连接
void handleConnection(int epoll_fd, int listen_fd)
{

    int conn_fd = 0;
    // 接受一个新连接
    conn_fd = accept(listen_fd, NULL, NULL);
    if (conn_fd == -1)
    {
        perror("accept");
        return;
    }

    // 将新连接加入epoll
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = conn_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &event) == -1)
    {
        perror("epoll_ctl: conn_fd");
        close(conn_fd);
    }
}
// 处理客户端断开连接
void handleDisconnect(int epoll_fd, int disconnect_fd, MYSQL *conn)
{
    // 查看该连接是否仍处于在线状态（以处理异常退出）
    char sql_str[1000];

    sprintf(sql_str, "select account from online_user where socket_fd=%d", disconnect_fd);
    puts(sql_str);
    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    MYSQL_RES *r_set = NULL;
    r_set = mysql_store_result(conn);
    if (ret == 0)
    {
        printf("执行成功\n");
        if (mysql_num_rows(r_set))
        {
            printf("不为空\n");
            MYSQL_ROW rec = mysql_fetch_row(r_set);
            // 并将在线状态设为0（离线）
            sprintf(sql_str, "update user set is_online=0 where account=%d", atoi(rec[0]));
            puts(sql_str);
            mysql_real_query(conn, sql_str, strlen(sql_str)); // 偷个懒，不检查返回值了

            // 从在线用户表中删除
            sprintf(sql_str, "delete from online_user where account=%d", atoi(rec[0]));
            puts(sql_str);
            mysql_real_query(conn, sql_str, strlen(sql_str)); // 偷个懒，不检查返回值了
        }
    }

    mysql_free_result(r_set);
    // 移出监听
    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, disconnect_fd, NULL) == -1)
    {
        perror("epoll_ctl");
    }
}

// 处理客户端发来的各种包
void handleMessage(char *jsonData, int target_fd, MYSQL *conn)
{
    printf("thisMES：%s\n", jsonData);
    cJSON *root = cJSON_Parse(jsonData);
    if (root == NULL)
    {
        printf("parse JSON failed!\n");
        return;
    }
    cJSON *jsonType = cJSON_GetObjectItem(root, "type");
    int type = jsonType->valueint;
    switch (type)
    {
    case SIGN_UP:
        printf("注册\n");
        handleSignUp(root, target_fd, conn);
        break;
    case SIGN_IN:
        printf("登录\n");
        handleSignIn(root, target_fd, conn);
        break;
    case DELETE_ACCOUNT:
        printf("注销\n");
        handleDeleteAccount(root, target_fd, conn);
        break;
    case GET_CONTACTS:
        printf("获取联系人列表\n");
        handleReturnContacts(root, target_fd, conn);
        break;
    case REMOVE_CONTACT:
        printf("删除联系人\n");
        handleRemoveContact(root, target_fd, conn);
        break;
    case PRIVATE_CHAT:
        printf("获取聊天记录\n");
        handlePrivateChat(root, target_fd, conn);
        break;
    case REQUEST_PRIVATE_CHAT:
        printf("创建单聊连接申请\n");
        handlePrivateChatRequst(root, target_fd, conn);
        break;
    case SEND_PRIVATE_MES:
        printf("发送消息\n");
        handleSendMessage(jsonData, root, target_fd, conn);
        break;
    case ADD_CONTACT:
        printf("添加好友\n");
        handleAddContact(root, target_fd, conn);
        break;
    case GET_CONTACT_NOTIFICATIONS:
        printf("获取好友验证消息\n");
        handleGetContactNotifications(root, target_fd, conn);
        break;
    case AGREE_ADD_CONTACT:
        printf("同意好友申请\n");
        handleAgreeAddContact(root, target_fd, conn);
        break;
    case REFUSE_ADD_CONTACT:
        printf("拒绝好友申请\n");
        handleRefuseAddContact(root, target_fd, conn);
        break;
    case GET_GROUPS:
        printf("获取群列表\n");
        handleGetGroups(root, target_fd, conn);
        break;
    case GROUP_CHAT:
        printf("进行群聊\n");
        handleGroupChat(root, target_fd, conn);
        break;
    case REQUEST_GROUP_CHAT:
        printf("创建群聊连接申请\n");
        handleGroupChatRequst(root, target_fd, conn);
        break;
    case INVITE_TO_GROUP:
        printf("邀请加入群聊\n");
        handleInviteToGroup(root, target_fd, conn);
        break;
    case SEND_GROUP_MES:
        printf("发送群聊消息\n");
        handleSendGroupMessage(jsonData, root, target_fd, conn);
        break;
    case LEAVE_GROUP:
        printf("退出群聊\n");
        handleLeaveGroup(root, target_fd, conn);
        break;
    case CREATE_GROUP:
        printf("创建群聊\n");
        handleCreateGroup(root, target_fd, conn);
        break;
    case GET_GROUP_NOTIFICATIONS:
        printf("获取群聊通知\n");
        handleGetGroupNotifications(root, target_fd, conn);
        break;
    case AGREE_JOIN_GROUP:
        printf("同意加入群聊\n");
        handleAgreeJoinGroup(root, target_fd, conn);
        break;
    case REFUSE_JOIN_GROUP:
        printf("拒绝加入群聊\n");
        handleRefuseJoinGroup(root, target_fd, conn);
        break;
    case SIGN_OUT:
        printf("退出登录\n");
        handleSignOut(root, target_fd, conn);
        break;
    default:
        printf("无效/未知的包!\n");
        break;
    }
    root = NULL;
}
// 注册账号（昵称，密码）——返回账号
void handleSignUp(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonNickname = cJSON_GetObjectItem(root, "nickname");
    cJSON *jsonPassword = cJSON_GetObjectItem(root, "password");

    char sql_str[1000];
    sprintf(sql_str, "insert into user(password,nickname) values('%s','%s')", jsonPassword->valuestring, jsonNickname->valuestring);
    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {
        // 查询账号
        strcpy(sql_str, "select LAST_INSERT_ID()");
        mysql_real_query(conn, sql_str, strlen(sql_str));
        MYSQL_RES *r_set;
        r_set = mysql_store_result(conn);
        MYSQL_ROW rec = mysql_fetch_row(r_set);
        int userAccount = atoi(rec[0]);
        // 发送账号
        cJSON *send = cJSON_CreateObject();
        cJSON_AddNumberToObject(send, "type", SEND_ACCOUNT);
        cJSON_AddNumberToObject(send, "userAccount", userAccount);
        char *json_string = cJSON_Print(send);
        mySend(json_string, target_fd);

        printf("acccount:%d\n", userAccount);
        cJSON_Delete(send);
        mysql_free_result(r_set);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("注册失败"), target_fd);
        printf("error!\n");
    }
    cJSON_Delete(root);
}
// 登录(账号，密码)——返回用户信息(昵称)
void handleSignIn(cJSON *root, int target_fd, MYSQL *conn)
{

    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonPassword = cJSON_GetObjectItem(root, "password");

    char sql_str[1000];
    sprintf(sql_str, "select nickname from user where account = %d AND password = '%s'", jsonUserAccount->valueint, jsonPassword->valuestring);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    MYSQL_RES *r_set = NULL;
    r_set = mysql_store_result(conn);
    if (mysql_num_rows(r_set))
    {
        printf("不为空\n");
        MYSQL_ROW rec = mysql_fetch_row(r_set);
        cJSON *send = cJSON_CreateObject();
        cJSON_AddNumberToObject(send, "type", REPLY_SIGN_IN);
        cJSON_AddStringToObject(send, "nickname", rec[0]);

        char *json_string = cJSON_Print(send);
        mySend(json_string, target_fd);
        cJSON_Delete(send);
        mysql_free_result(r_set);
        r_set = NULL;
        // 将登录状态设为1
        sprintf(sql_str, "update user set is_online=1 where account=%d", jsonUserAccount->valueint);
        ret = mysql_real_query(conn, sql_str, strlen(sql_str)); // 偷懒，不检查ret了

        // 并保存账号至在线用户表
        sprintf(sql_str, "insert into online_user(account, socket_fd) values (%d,%d)", jsonUserAccount->valueint, target_fd);
        puts(sql_str);
        ret = mysql_real_query(conn, sql_str, strlen(sql_str)); // 偷懒，不检查ret了
        // 返回错误码
    }
    else // 查询不为空
    {
        mySend(generateERRORJSON("登录失败"), target_fd);
    }

    mysql_free_result(r_set);
    cJSON_Delete(root);
}
// 注销账号
void handleDeleteAccount(cJSON *root, int target_fd, MYSQL *conn)
{

    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");

    char sql_str[1000];
    sprintf(sql_str, "delete  from user where account = %d ", jsonUserAccount->valueint);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {
        // 返回OK
        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("注销失败"), target_fd);
    }
    cJSON_Delete(root);
}
// 获取联系人列表（本人账号）——联系人列表
void handleReturnContacts(cJSON *root, int target_fd, MYSQL *conn)
{

    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");

    char sql_str[1000];
    sprintf(sql_str, "select account,nickname,is_online from user where account in (select friend_account from contact where user_account = %d ) order by account", jsonUserAccount->valueint);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    MYSQL_RES *r_set = NULL;
    r_set = mysql_store_result(conn);

    if (mysql_num_rows(r_set))
    { // 初始化待发送json对象
        cJSON *send_data = cJSON_CreateObject();
        cJSON_AddNumberToObject(send_data, "type", REPLY_GET_CONTACTS);

        // 字段数
        int num = mysql_num_fields(r_set);
        // 行数
        int rows = mysql_num_rows(r_set);
        // 创建json数组
        cJSON *friends = cJSON_CreateArray();
        for (int i = 0; i < rows; i++)
        {

            // 从结果集合中取一行
            MYSQL_ROW rec = mysql_fetch_row(r_set);
            if (rows < 0)
            {
                break;
            }
            cJSON *item = cJSON_CreateObject();
            // 往数组项添加数据，然后将数组项添加进数组
            cJSON_AddNumberToObject(item, "account", atoi(rec[0]));
            cJSON_AddStringToObject(item, "nickname", rec[1]);
            cJSON_AddNumberToObject(item, "isOnline", atoi(rec[2]));

            cJSON_AddItemToArray(friends, item);
        }
        cJSON_AddItemToObject(send_data, "friends", friends);
        char *json_string = cJSON_Print(send_data);
        printf("json_string:%s\n", json_string);
        mySend(json_string, target_fd);
    }
    else // 查询不为空
    {
        // 返回错误码
        mySend(generateERRORJSON("请先添加联系人"), target_fd);
    }

    mysql_free_result(r_set);
    cJSON_Delete(root);
}
// 删除好友
void handleRemoveContact(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonTargetAccount = cJSON_GetObjectItem(root, "targetAccount");

    char sql_str[1000];
    sprintf(sql_str, "delete from contact where user_account= %d AND friend_account=%d", jsonUserAccount->valueint, jsonTargetAccount->valueint);
    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {
        // 返回正确
        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("删除失败"), target_fd);
    }
    cJSON_Delete(root);
}
// 申请获取单聊聊天记录
void handlePrivateChat(cJSON *root, int target_fd, MYSQL *conn)
{

    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonTargetAccount = cJSON_GetObjectItem(root, "targetAccount");

    char sql_str[1000];
    sprintf(sql_str, "SELECT user.nickname, single_message.send_time, single_message.content FROM single_message INNER JOIN user ON user.account = single_message.send_account WHERE (single_message.send_account = %d AND single_message.receive_account = %d) OR (single_message.send_account = %d AND single_message.receive_account = %d) ORDER BY single_message.send_time", jsonUserAccount->valueint, jsonTargetAccount->valueint, jsonTargetAccount->valueint, jsonUserAccount->valueint);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    MYSQL_RES *r_set;
    r_set = mysql_store_result(conn);
    if (mysql_num_rows(r_set)) // 查询不为空
    {
        // 初始化待发送json对象
        cJSON *send_data = cJSON_CreateObject();
        cJSON_AddNumberToObject(send_data, "type", REPLY_PRIVATE_CHAT);

        // 字段数
        int num = mysql_num_fields(r_set);
        // 行数
        int rows = mysql_num_rows(r_set);
        printf("num:%d  row:%d\n", num, rows);
        // 创建json数组
        cJSON *messages = cJSON_CreateArray();
        for (int i = 0; i < rows; i++)
        {

            // 从结果集合中取一行
            MYSQL_ROW rec = mysql_fetch_row(r_set);
            if (rows < 0)
            {
                break;
            }
            cJSON *item = cJSON_CreateObject();
            // 往数组项添加数据，然后将数组项添加进数组
            cJSON_AddStringToObject(item, "nickname", rec[0]);
            cJSON_AddStringToObject(item, "sendTime", rec[1]);
            cJSON_AddStringToObject(item, "content", rec[2]);
            cJSON_AddItemToArray(messages, item);
        }

        cJSON_AddItemToObject(send_data, "messages", messages);
        char *json_string = cJSON_Print(send_data);
        printf("json_string:%s\n", json_string);
        mySend(json_string, target_fd);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("暂无历史消息"), target_fd);
    }

    mysql_free_result(r_set);
    cJSON_Delete(root);
}
// 申请进行单聊(新开一个连接用来接收)
void handlePrivateChatRequst(cJSON *root, int target_fd, MYSQL *conn)
{

    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");

    char sql_str[1000];
    sprintf(sql_str, "UPDATE online_user SET private_chat_socket_fd=%d WHERE account =%d", target_fd, jsonUserAccount->valueint);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));

    if (ret == 0)
    {
        mySend(generateOKJSON(), target_fd);
    }
    else
    { // 返回错误码
        mySend(generateERRORJSON("暂无历史消息"), target_fd);
    }

    cJSON_Delete(root);
}
// 发送单聊消息（本人账号，目标账号，消息）
void handleSendMessage(char *jsonData, cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonTargetAccount = cJSON_GetObjectItem(root, "targetAccount");
    cJSON *jsonContent = cJSON_GetObjectItem(root, "content");
    cJSON *jsonSendTime = cJSON_GetObjectItem(root, "sendTime");

    // 看看对方在不在线
    char sql_str[1000];

    sprintf(sql_str, "select private_chat_socket_fd from online_user where account=%d", jsonTargetAccount->valueint);
    puts(sql_str);
    mysql_real_query(conn, sql_str, strlen(sql_str)); // 偷个懒，不检查返回值了
    MYSQL_RES *r_set;
    r_set = mysql_store_result(conn);
    if (mysql_num_rows(r_set))
    {
        // 结果不为空，
        MYSQL_ROW rec = mysql_fetch_row(r_set);
        // 转发给目标用户
        int len = strlen(jsonData);
        int network_int = htonl(len);
        write(atoi(rec[0]), &network_int, sizeof(int));
        write(atoi(rec[0]), jsonData, len);
        puts(jsonData);
        printf("转发成功\n");
    }
    else
    {
        printf("获取结果集失败或无结果: %s\n", mysql_error(conn));
    }

    // 保存至数据库
    sprintf(sql_str, "insert into single_message(send_account,receive_account,content,send_time) values (%d,%d,'%s','%s')", jsonUserAccount->valueint, jsonTargetAccount->valueint, jsonContent->valuestring, jsonSendTime->valuestring);
    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {
        // 返回正确
        mySend(generateOKJSON(), target_fd);
        printf("ok\n");
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("发送失败"), target_fd);
        printf("error!\n");
    }
    cJSON_Delete(root);
}
// 申请添加好友——return OK
void handleAddContact(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonTargetAccount = cJSON_GetObjectItem(root, "targetAccount");
    char sql_str[1000];
    MYSQL_RES *result = NULL;
    // 查一下好友表，看看是否对好友发好友申请，如果是直接返回错误码
    sprintf(sql_str, "SELECT * from contact WHERE user_account=%d AND friend_account = %d", jsonUserAccount->valueint, jsonTargetAccount->valueint);
    puts(sql_str);
    mysql_real_query(conn, sql_str, strlen(sql_str));
    result = mysql_store_result(conn);
    // 如果好友已存在，直接返回
    if (mysql_num_rows(result))
    {
        mySend(generateERRORJSON("好友已存在"), target_fd);
        mysql_free_result(result);
        cJSON_Delete(root);
        return;
    }

    mysql_free_result(result);
    // 好友不存在，先查一遍好友申请表，看对方有没有发好友申请
    sprintf(sql_str, "select *from friend_request where request_account=%d AND target_account =%d", jsonTargetAccount->valueint, jsonUserAccount->valueint);
    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    result = mysql_store_result(conn);
    puts(sql_str);
    if (ret != 0)
    {
        mySend(generateERRORJSON("出错了，请重新添加"), target_fd);
    }
    else
    {
        if (mysql_num_rows(result)) // 结果存在，则自动同意申请
        {
            mysql_free_result(result);
            result = NULL;
            // 从申请表里删除记录
            sprintf(sql_str, "delete from friend_request where request_account=%d", jsonTargetAccount->valueint);
            puts(sql_str);
            ret = mysql_real_query(conn, sql_str, strlen(sql_str));
            if (ret == 0)
            {
                // 添加进好友表
                sprintf(sql_str, "insert into contact(user_account,friend_account) values (%d,%d),(%d,%d)", jsonTargetAccount->valueint, jsonUserAccount->valueint, jsonUserAccount->valueint, jsonTargetAccount->valueint);
                puts(sql_str);
                ret = mysql_real_query(conn, sql_str, strlen(sql_str));
                if (ret == 0)
                {
                    mySend(generateERRORJSON("成功添加好友"), target_fd);
                }
                else
                {
                    mySend(generateERRORJSON("添加好友失败"), target_fd);
                }
            }
            else // 发送错误码
            {
                mySend(generateERRORJSON("同意申请失败"), target_fd);
            }
        }
        else // 结果不存在，则正常发送好友申请
        {
            mysql_free_result(result);
            result = NULL;
            sprintf(sql_str, "insert into friend_request(request_account,target_account,time) values (%d,%d,now())", jsonUserAccount->valueint, jsonTargetAccount->valueint);
            ret = mysql_real_query(conn, sql_str, strlen(sql_str));
            if (ret == 0)
            {
                mySend(generateOKJSON(), target_fd);
            }
            else
            {
                if (mysql_errno(conn) == 1062)
                {
                    mySend(generateERRORJSON("请勿重复提交申请"), target_fd);
                }
                else
                {
                    mySend(generateERRORJSON("发送好友申请失败"), target_fd);
                }
            }
        }
    }

    cJSON_Delete(root);
}
// 获取好友验证消息表（本人账号）———返回验证消息表
// 偷懒，只显示给别人发的好友申请，不显示别人给自己发的
void handleGetContactNotifications(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");

    char sql_str[1000];
    sprintf(sql_str, "select user.nickname,friend_request.request_account,friend_request.time from friend_request  INNER JOIN user on  friend_request.request_account=user.account AND target_account=%d ORDER BY time", jsonUserAccount->valueint);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    MYSQL_RES *r_set = NULL;
    r_set = mysql_store_result(conn);

    if (mysql_num_rows(r_set)) // 查询不为空
    {
        // 初始化待发送json对象
        cJSON *send_data = cJSON_CreateObject();
        cJSON_AddNumberToObject(send_data, "type", REPLY_GET_CONTACT_NOTIFICATIONS);

        // 字段数
        int num = mysql_num_fields(r_set);
        // 行数
        int rows = mysql_num_rows(r_set);
        // 创建json数组
        cJSON *requests = cJSON_CreateArray();
        for (int i = 0; i < rows; i++)
        {

            // 从结果集合中取一行
            MYSQL_ROW rec = mysql_fetch_row(r_set);
            if (rows < 0)
            {
                break;
            }
            cJSON *item = cJSON_CreateObject();
            // 往数组项添加数据，然后将数组项添加进数组
            cJSON_AddStringToObject(item, "nickname", rec[0]);
            cJSON_AddNumberToObject(item, "targetAccount", atoi(rec[1]));
            cJSON_AddStringToObject(item, "time", rec[2]);

            cJSON_AddItemToArray(requests, item);
        }
        cJSON_AddItemToObject(send_data, "requests", requests);
        char *json_string = cJSON_Print(send_data);
        printf("json_string:%s\n", json_string);
        mySend(json_string, target_fd);
    }
    else
    { // 返回错误码
        mySend(generateERRORJSON("暂无好友验证消息"), target_fd);
    }

    mysql_free_result(r_set);
    cJSON_Delete(root);
}
// 同意好友申请（本人账号,目标账号）————return ok
void handleAgreeAddContact(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonTargetAccount = cJSON_GetObjectItem(root, "targetAccount");
    char sql_str[1000];

    sprintf(sql_str, "delete from friend_request where target_account=%d AND request_account=%d", jsonUserAccount->valueint, jsonTargetAccount->valueint);
    puts(sql_str);
    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret != 0)
    {
        mySend(generateERRORJSON("同意好友申请失败"), target_fd);
        cJSON_Delete(root);
        return;
    }
    sprintf(sql_str, "insert into contact(user_account,friend_account) values (%d,%d),(%d,%d)", jsonTargetAccount->valueint, jsonUserAccount->valueint, jsonUserAccount->valueint, jsonTargetAccount->valueint);
    puts(sql_str);
    ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {

        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        if (mysql_errno(conn) == 1062)
        {
            mySend(generateERRORJSON("好友已存在"), target_fd);
        }
        else
        {
            mySend(generateERRORJSON("添加好友失败"), target_fd);
        }
    }
    cJSON_Delete(root);
}
// 拒绝好友申请（本人账号,目标账号）———return ok
void handleRefuseAddContact(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonTargetAccount = cJSON_GetObjectItem(root, "targetAccount");

    char sql_str[1000];
    sprintf(sql_str, "delete from friend_request where target_account=%d AND request_account=%d", jsonUserAccount->valueint, jsonTargetAccount->valueint);
    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {
        // 返回正确
        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("拒绝好友申请失败"), target_fd);
    }
    cJSON_Delete(root);
}
// 获取群列表（本人账号）————返回群列表
void handleGetGroups(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");

    char sql_str[1000];
    sprintf(sql_str, "SELECT `group`.name, user_group.group_account FROM user_group INNER JOIN `group` ON `group`.account = user_group.group_account AND  user_group.user_account =%d", jsonUserAccount->valueint);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    MYSQL_RES *r_set = NULL;
    r_set = mysql_store_result(conn);

    if (mysql_num_rows(r_set)) // 查询不为空
    {
        // 初始化待发送json对象
        cJSON *send_data = cJSON_CreateObject();
        cJSON_AddNumberToObject(send_data, "type", REPLY_GET_GROUPS);

        // 字段数
        int num = mysql_num_fields(r_set);
        // 行数
        int rows = mysql_num_rows(r_set);
        // 创建json数组
        cJSON *groups = cJSON_CreateArray();
        for (int i = 0; i < rows; i++)
        {

            // 从结果集合中取一行
            MYSQL_ROW rec = mysql_fetch_row(r_set);
            if (rows < 0)
            {
                break;
            }
            cJSON *item = cJSON_CreateObject();
            // 往数组项添加数据，然后将数组项添加进数组
            cJSON_AddStringToObject(item, "name", rec[0]);
            cJSON_AddNumberToObject(item, "group_account", atoi(rec[1]));

            cJSON_AddItemToArray(groups, item);
        }
        cJSON_AddItemToObject(send_data, "groups", groups);
        char *json_string = cJSON_Print(send_data);
        printf("json_string:%s\n", json_string);
        mySend(json_string, target_fd);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("请先加群"), target_fd);
    }

    mysql_free_result(r_set);
    cJSON_Delete(root);
}
// 申请获取群聊聊天记录（群ID）———返回群聊消息记录
void handleGroupChat(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonGroupAccount = cJSON_GetObjectItem(root, "groupAccount");

    char sql_str[1000];
    sprintf(sql_str, "select user.nickname,group_message.content,group_message.send_time FROM group_message INNER JOIN user ON user.account=group_message.send_account WHERE group_message.group_account=%d", jsonGroupAccount->valueint);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    MYSQL_RES *r_set;
    r_set = mysql_store_result(conn);

    if (mysql_num_rows(r_set)) // 查询不为空
    {
        // 初始化待发送json对象
        cJSON *send_data = cJSON_CreateObject();
        cJSON_AddNumberToObject(send_data, "type", REPLY_GROUP_CHAT);

        // 字段数
        int num = mysql_num_fields(r_set);
        // 行数
        int rows = mysql_num_rows(r_set);
        // 创建json数组
        cJSON *messages = cJSON_CreateArray();
        for (int i = 0; i < rows; i++)
        {

            // 从结果集合中取一行
            MYSQL_ROW rec = mysql_fetch_row(r_set);
            if (rows < 0)
            {
                break;
            }
            cJSON *item = cJSON_CreateObject();
            // 往数组项添加数据，然后将数组项添加进数组
            cJSON_AddStringToObject(item, "nickName", rec[0]);
            cJSON_AddStringToObject(item, "content", rec[1]);
            cJSON_AddStringToObject(item, "sendTime", rec[2]);

            cJSON_AddItemToArray(messages, item);
        }
        cJSON_AddItemToObject(send_data, "messages", messages);
        char *json_string = cJSON_Print(send_data);
        printf("json_string:%s\n", json_string);
        mySend(json_string, target_fd);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("获取群聊历史消息失败"), target_fd);
    }

    mysql_free_result(r_set);
    cJSON_Delete(root);
}
// 申请邀请好友加入群聊（账号，好友账号，群ID）———return ok
void handleInviteToGroup(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonTargetAccount = cJSON_GetObjectItem(root, "targetAccount");
    cJSON *jsonGroupAccount = cJSON_GetObjectItem(root, "groupAccount");
    char sql_str[1000];

    // 先查一遍，对方是否已在群聊中
    sprintf(sql_str, " select *from user_group where user_account=%d AND group_account =%d", jsonTargetAccount->valueint, jsonGroupAccount->valueint);
    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    puts(sql_str);
    MYSQL_RES *result = NULL;
    result = mysql_store_result(conn);
    if (ret != 0)
    {
        mySend(generateERRORJSON("出错了，请重新邀请"), target_fd);
    }
    else
    {
        if (mysql_num_rows(result)) // 结果存在
        {
            mySend(generateERRORJSON("对方已在群聊中"), target_fd);
        }
        else // 结果不存在，说明好友不在群里中
        {
            mysql_free_result(result);
            result = NULL;
            sprintf(sql_str, "insert INTO  group_invitation(invite_user_account, target_user_account, target_group_account, time) VALUES (%d,%d,%d,now())", jsonUserAccount->valueint, jsonTargetAccount->valueint, jsonGroupAccount->valueint);
            ret = mysql_real_query(conn, sql_str, strlen(sql_str));
            if (ret != 0)
            {
                if (mysql_errno(conn) == 1062)
                {
                    mySend(generateERRORJSON("请勿重复发送申请"), target_fd);
                }
                else
                {
                    mySend(generateERRORJSON("邀请失败"), target_fd);
                }
            }
            else
            {
                mySend(generateOKJSON(), target_fd);
            }
        }
    }
    mysql_free_result(result);
    cJSON_Delete(root);
}
// 申请发送群聊消息（账号,群ID,消息）———return ok
void handleSendGroupMessage(char *jsonData, cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonGroupAccount = cJSON_GetObjectItem(root, "groupAccount");
    cJSON *jsonContent = cJSON_GetObjectItem(root, "content");
    cJSON *jsonSendTime = cJSON_GetObjectItem(root, "sendTime");

    // 转发给正在群聊的群成员
    char sql_str[1000];

    sprintf(sql_str, "select group_chat_socket_fd from online_user where account IN (SELECT user_account FROM user_group WHERE group_account=%d) AND group_chat_socket_fd !=0 AND account!=%d", jsonGroupAccount->valueint, jsonUserAccount->valueint);
    puts(sql_str);
    mysql_real_query(conn, sql_str, strlen(sql_str)); // 偷个懒，不检查返回值了
    MYSQL_RES *r_set = NULL;
    r_set = mysql_store_result(conn);
    if (r_set)
    {
        int len = 0;
        len = mysql_num_rows(r_set);
        if (len > 0)
        {
            // 逐个转发
            for (int i = 0; i < len; i++)
            {
                // 结果不为空，
                MYSQL_ROW rec = mysql_fetch_row(r_set);
                // 转发给目标用户
                int len = strlen(jsonData);
                int network_int = htonl(len);
                write(atoi(rec[0]), &network_int, sizeof(int));
                write(atoi(rec[0]), jsonData, len);
                puts(jsonData);
            }
            printf("全部转发成功\n");
        }
        else
        {
            printf("获取结果集失败或无结果: %s\n", mysql_error(conn));
        }
    }

    // 保存至数据库
    sprintf(sql_str, "insert into group_message(send_account,group_account,send_time,content) values (%d,%d,'%s','%s')", jsonUserAccount->valueint, jsonGroupAccount->valueint, jsonSendTime->valuestring, jsonContent->valuestring);
    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {
        // 返回正确
        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("发送失败"), target_fd);
    }
    cJSON_Delete(root);
}
// 申请进行群聊(新开一个连接用来接收)
void handleGroupChatRequst(cJSON *root, int target_fd, MYSQL *conn)
{

    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");

    char sql_str[1000];
    sprintf(sql_str, "UPDATE online_user SET group_chat_socket_fd=%d WHERE account =%d", target_fd, jsonUserAccount->valueint);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));

    if (ret == 0)
    {
        mySend(generateOKJSON(), target_fd);
    }
    else
    { // 返回错误码
        mySend(generateERRORJSON("创建群聊链接失败"), target_fd);
    }

    cJSON_Delete(root);
}

// 申请退出群聊（账号,群ID）———return ok
void handleLeaveGroup(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonGroupAccount = cJSON_GetObjectItem(root, "groupAccount");

    char sql_str[1000];
    sprintf(sql_str, "DELETE FROM user_group WHERE user_account=%d AND group_account=%d", jsonUserAccount->valueint, jsonGroupAccount->valueint);
    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {
        // 返回正确
        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("发送失败"), target_fd);
    }
    cJSON_Delete(root);
}
// 申请创建群聊（群名，用户账户，账号数组）——return ok
void handleCreateGroup(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonGroupName = cJSON_GetObjectItem(root, "groupName");
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonMembers = cJSON_GetObjectItem(root, "members");
    int len = cJSON_GetArraySize(jsonMembers);

    char sql_str[1000];
    // 通过事务处理多条插入语句
    mysql_autocommit(conn, 0);
    // 创建群
    sprintf(sql_str, "insert into `group`( name, create_time,  owner_account) VALUES ('%s',now(),%d)", jsonGroupName->valuestring, jsonUserAccount->valueint);
    puts(sql_str);
    mysql_real_query(conn, sql_str, strlen(sql_str));

    // 获取新群的群账号
    sprintf(sql_str, "select account from `group` WHERE owner_account=%d order by create_time DESC", jsonUserAccount->valueint);
    puts(sql_str);
    mysql_real_query(conn, sql_str, strlen(sql_str));
    MYSQL_RES *r_set;
    r_set = mysql_store_result(conn);
    MYSQL_ROW rec = mysql_fetch_row(r_set);
    int groupAccount = atoi(rec[0]);
    mysql_free_result(r_set);
    // 将创建者添加进user_group表
    sprintf(sql_str, "insert into user_group(user_account, group_account) VALUES (%d,%d)", jsonUserAccount->valueint, groupAccount);
    mysql_real_query(conn, sql_str, strlen(sql_str));
    // 逐一发送邀请
    for (int i = 0; i < len; i++)
    {
        cJSON *data = cJSON_GetArrayItem(jsonMembers, i);
        sprintf(sql_str, "insert INTO  group_invitation(invite_user_account, target_user_account, target_group_account, time) VALUES (%d,%d,%d,now())", jsonUserAccount->valueint, data->valueint, groupAccount);
        puts(sql_str);
        mysql_real_query(conn, sql_str, strlen(sql_str));
    }
    // 提交
    if (mysql_commit(conn) == 0)
    {
        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        mySend(generateERRORJSON("创建失败"), target_fd);
    }

    cJSON_Delete(root);
}
// 获取群聊验证消息表（本人账号）———返回验证消息表
// 偷个懒，只获取别人邀请该用户加群的验证消息
void handleGetGroupNotifications(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");

    char sql_str[1000];
    sprintf(sql_str, "SELECT group_invitation.id,group_invitation.invite_user_account, user.nickname, group_invitation.target_group_account,`group`.name,group_invitation.time FROM group_invitation INNER JOIN user ON user.account=group_invitation.invite_user_account AND target_user_account=%d INNER JOIN  `group` ON `group`.account=group_invitation.target_group_account ORDER BY group_invitation.time DESC", jsonUserAccount->valueint);

    puts(sql_str);

    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    MYSQL_RES *r_set;
    r_set = mysql_store_result(conn);

    if (mysql_num_rows(r_set)) // 查询不为空
    {
        // 初始化待发送json对象
        cJSON *send_data = cJSON_CreateObject();
        cJSON_AddNumberToObject(send_data, "type", REPLY_GET_GROUP_NOTIFICATIONS);

        // 字段数
        int num = mysql_num_fields(r_set);
        // 行数
        int rows = mysql_num_rows(r_set);
        // 创建json数组
        cJSON *invitations = cJSON_CreateArray();
        if (rows > 0)
        {
            for (int i = 0; i < rows; i++)
            {

                // 从结果集合中取一行
                MYSQL_ROW rec = mysql_fetch_row(r_set);

                cJSON *item = cJSON_CreateObject();
                // 往数组项添加数据，然后将数组项添加进数组
                cJSON_AddNumberToObject(item, "inviteId", atoi(rec[0]));           // 邀请编号
                cJSON_AddNumberToObject(item, "inviteUserAccount", atoi(rec[1]));  // 邀请人账号
                cJSON_AddStringToObject(item, "inviteUserName", rec[2]);           // 邀请人昵称
                cJSON_AddNumberToObject(item, "targetGroupAccount", atoi(rec[3])); // 群号
                cJSON_AddStringToObject(item, "targetGroupName", rec[4]);          // 群名
                cJSON_AddStringToObject(item, "time", rec[5]);                     // 时间

                cJSON_AddItemToArray(invitations, item);
            }
        }
        cJSON_AddItemToObject(send_data, "invitations", invitations);
        char *json_string = cJSON_Print(send_data);
        printf("json_string:%s\n", json_string);
        mySend(json_string, target_fd);
    }
    else
    {
        // 返回错误码
        mySend(generateERRORJSON("暂无群验证消息"), target_fd);
    }

    mysql_free_result(r_set);
    cJSON_Delete(root);
}
// 同意加入群聊（邀请编号）————return ok
void handleAgreeJoinGroup(cJSON *root, int target_fd, MYSQL *conn)
{

    cJSON *jsonInviteId = cJSON_GetObjectItem(root, "inviteId");
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");
    cJSON *jsonGroupAccount = cJSON_GetObjectItem(root, "groupAccount");

    char sql_str[1000];

    sprintf(sql_str, "delete from group_invitation WHERE id=%d", jsonInviteId->valueint);
    puts(sql_str);
    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret != 0)
    {
        mySend(generateERRORJSON("同意邀请失败"), target_fd);
        cJSON_Delete(root);
        return;
    }
    sprintf(sql_str, "insert into user_group(user_account, group_account) VALUES (%d,%d)", jsonUserAccount->valueint, jsonGroupAccount->valueint);
    puts(sql_str);
    ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {

        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        if (mysql_errno(conn) == 1062)
        {
            mySend(generateERRORJSON("已经在群里了"), target_fd);
        }
        else
        {
            mySend(generateERRORJSON("加入群聊失败"), target_fd);
        }
    }
    cJSON_Delete(root);
}
// 拒绝加入群聊（邀请人账号，用户账户，群ID）————return ok
void handleRefuseJoinGroup(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonInviteId = cJSON_GetObjectItem(root, "inviteId");

    char sql_str[1000];

    sprintf(sql_str, "delete from group_invitation WHERE id=%d", jsonInviteId->valueint);
    puts(sql_str);
    int ret = mysql_real_query(conn, sql_str, strlen(sql_str));
    if (ret == 0)
    {
        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        mySend(generateERRORJSON("拒绝邀请失败"), target_fd);
    }

    cJSON_Delete(root);
}

// 退出登录
void handleSignOut(cJSON *root, int target_fd, MYSQL *conn)
{
    cJSON *jsonUserAccount = cJSON_GetObjectItem(root, "userAccount");

    // 将该用户的在线状态置为0（离线）
    char sql_str[1000];
    sprintf(sql_str, "update user set is_online=0 where account=%d", jsonUserAccount->valueint);
    int ret1 = mysql_real_query(conn, sql_str, strlen(sql_str)); // 偷个懒，不检查返回值了

    // 从在线用户表中删除
    sprintf(sql_str, "delete from online_user where socket_fd=%d", target_fd);
    int ret2 = mysql_real_query(conn, sql_str, strlen(sql_str)); // 偷个懒，不检查返回值了
    if (ret1 == 0 && ret2 == 0)
    {
        mySend(generateOKJSON(), target_fd);
    }
    else
    {
        mySend(generateERRORJSON("退出登录失败"), target_fd);
    }

    cJSON_Delete(root);
}