# 一、功能和模块说明

## 登录模块

- 登录——输入账号密码，待服务器验证成功后进入主界面
- 注册账号——输入昵称，密码，注册成功后服务器返回账号，并自动登录



## 联系人模块

- 我的好友——显示好友列表，选着某个好友后可以进行进一步操作，如发消息，删除等
  - 发消息（单聊）——获取历史消息记录，实时发送和接收消息
  - 删除好友
- 好友验证——显示好友验证消息，选着某个验证消息后可以进一步操作，如同意，拒绝等
  - 同意好友申请——将用户添加到联系人列表
  - 拒绝好友申请
- 添加好友——输入目标账号后将向其发送一条好友申请



## 群模块

- 我的群聊——显示我加入的所有群聊，选着某个群聊后可进行进一步操作，如退出，发消息，邀请好友加入
  - 发送群聊消息——获取群历史聊天记录，并实时发送和接收消息
  - 邀请加群——输入目标账号后可向其发送加群邀请
  - 退出群聊

- 群通知——显示所有对用户发出的加群邀请，选中某个邀请后可进行进一步操作，如同意，拒绝
  - 同意加群邀请
  - 拒绝邀请
- 创建群聊——输入群名和要邀请的人后，将自动创建并加入一个群，并对所有邀请的人发送邀请





# 二、网络通信

#### 账号服务模块

- 申请登录（账号，密码）——返回其他账号信息（昵称）
- 申请注册（昵称，账号，密码——返回账号；
- 申请注销（账号）——return ok



#### 联系人模块

##### 我的好友

- 获取联系人列表（本人账号）——账号表3（账号）——返回联系人列表（账号，昵称，在线状态）
  - 申请删除好友（本人账号,目标账号——return ok
  - 申请进行单聊（本人账号,目标账号）——返回单聊消息记录
    - 申请发送单聊消息（本人账号，目标账号，消息）——return ok

##### 添加好友

- 发送好友申请（本人账号,目标账号）
  - 如果对方已发送——return ERROR
  - 没有则发送——return OK


##### 好友通知

- 获取好友验证消息表（本人账号）——返回验证消息表

  - 同意好友申请（本人账号,目标账号）——return ok

  - 拒绝好友申请（本人账号,目标账号）——return ok

#### 群模块

##### 我的群聊

- 获取群列表（本人账号）——返回群列表
  - 申请进行群聊（群ID）——返回群聊消息记录
    - 持续监听新群聊消息（账号,群ID,消息）——群表12——return ok
    - 申请发送群聊消息（账号,群ID,消息）——return ok
  - 申请邀请好友加入群聊（账号，好友账号，群ID）——return ok
  - 申请退出群聊（账号,群ID）——return ok

##### 创建群聊

- 申请创建群聊（群名，用户账户，(成员账号1,成员账号2...)）——return ok

##### 群通知

- 获取群聊验证消息表（本人账号）——返回验证消息表
  - 同意加入群聊（账号，群ID）——return ok
  - 拒绝加入群聊——return ok





# 三、系统架构

## 1. 系统总体架构

本系统采用典型的 C/S 架构，由客户端和服务器组成。客户端用于用户的消息收发，服务器负责处理客户端的请求和消息转发。架构分为以下几个层次：

- **网络层**：负责客户端与服务器之间的通信，基于 TCP 协议实现消息的可靠传输。
- **数据库层**：存储用户信息、聊天记录等持久性数据，采用 MySQL 数据库管理。
- **业务逻辑层**：处理客户端的请求逻辑，包括用户登录、消息发送与接收、好友管理、群组管理等。
- **用户接口层**：客户端与用户的交互部分，使用简单的终端界面。

## 2. 网络层设计

### 2.1 网络协议选择

网络层采用 **TCP 协议**，确保消息传输的可靠性。TCP 是面向连接的协议，能够保证数据的完整性和顺序性，适用于聊天软件需要的实时、稳定的消息通信需求。

- **IP:**本地IP，localhost（127.0.0.0）
- **端口号**：服务器监听特定的端口（如 `8080`），等待客户端连接。
- **传输方式**：所有的消息传输都基于 TCP，采用 **同步阻塞模型**，服务器可以通过 I/O 复用机制（**epoll**）处理多个客户端连接。

### 2.2 网络数据包设计

为了确保客户端与服务器之间的消息通信结构清晰，json数据格式发包：

```
{
	type,
	其他数据项
}
```

- **type**：定义不同的包类型

  - ```SIGN_IN = 1```,   // 请求登录

  - ```REPLY_SIGN_IN,```  // 服务端回应登录请求

  - ``` SIGN_UP```,     // 请求注册

  - ```SEND_ACCOUNT```,  // 服务端完成注册，并返回新账号给客户端

  - ```DELETE_ACCOUNT```, // 请求删除账号

  - ```GET_CONTACTS```,     // 获取联系人列表

  - ```REPLY_GET_CONTACTS```,  // 服务器端返回联系人列表

  - ```REMOVE_CONTACT```,    // 客户端申请删除好友

  - ```PRIVATE_CHAT```,     // 客户端申请发起单聊

  - ```REQUEST_PRIVATE_CHAT```, // 客户端建立单聊专用TCP通道后，请求将通道保存至数据库

  - ```REPLY_PRIVATE_CHAT```,  // 服务端同意单聊请求，并将历史聊天记录返回

  - ```SEND_PRIVATE_MES```,   // 客户端通过专用TCP通道发送的单聊消息

  - ````ADD_CONTACT````, // 客户端申请添加好友

  - ```GET_CONTACT_NOTIFICATIONS```,    // 客户端申请获取好友通知

  - ```REPLY_GET_CONTACT_NOTIFICATIONS```, // 服务端同意申请并返回好友通知

  - ```AGREE_ADD_CONTACT```,        // 客户端申请同意好友申请

  - ```REFUSE_ADD_CONTACT,```        // 客户端申请拒绝好友申请

  - ```GET_GROUPS```,     // 客户端申请获取群聊列表

  - ```REPLY_GET_GROUPS```,  // 服务端同意申请，并返回群聊

  - ```GROUP_CHAT```,     // 客户端申请进行群聊

  - ```REQUEST_GROUP_CHAT```, // 客户端建立群聊专用TCP通道后，申请将通道保存至数据库

  - ```REPLY_GROUP_CHAT```,  // 服务端同意群聊请求，并将群历史消息返回

  - ```INVITE_TO_GROUP```,   // 客户端申请向好友发起加群邀请

  - ```SEND_GROUP_MES```,   // 客户端申请发送群聊消息

  - ```LEAVE_GROUP```,     // 客户端申请退出群聊

  - ```CREATE_GROUP```, // 客户端申请创建群聊

  - ```GET_GROUP_NOTIFICATIONS```,    // 客户端申请获取群通知

  - ```REPLY_GET_GROUP_NOTIFICATIONS```, // 服务端同意申请，并返回群通知

  - ```AGREE_JOIN_GROUP```,        // 客户端申请同意加群邀请

  - ```REFUSE_JOIN_GROUP```,       // 客户端申请拒绝加群邀请

  - ```SIGN_OUT```, // 客户端申请退出登录状态

  - ```OK```,   // 服务端同意申请（通用）

  - ```ERROR```, // 客户端返回错误信息

- **sender_id**：消息发送者的用户 ID。

- **receiver_id**：消息接收者的用户 ID；如果为 0，则表示发送群消息。

- **group_id**：群组 ID；如果是私聊消息，值为 0。

- **message**：消息内容，长度为 1024 字节。

- **timestamp**：消息发送的时间戳，记录消息发送的时间。

同时，为了避免粘包现象，在每个包发送前先发送一个小包，告诉接收方下一个包的大小

```c
void mySend(char *send_string, int target_fd)
{
    int len = strlen(send_string);
    int network_int = htonl(len);
    write(target_fd, &network_int, sizeof(int));
    write(target_fd, send_string, len);
    free(send_string);
}
```

同理，收包前先收个小包，知道下一个包的大小

```c
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
```



### 2.3 客户端与服务器通信流程

1. **客户端连接服务器**：客户端发起 TCP 连接请求，服务器监听并接受该连接。
2. **消息传递**：客户端向服务器发送封装好的 `JSON` 数据包，服务器根据 `type` 进行对应的处理，转发给接收方或存储到数据库中。
3. **消息推送**：服务器接收到消息后，如果目标客户端在线，立即向目标客户端转发。然后消息将存储到数据库中，用户下次登录时再推送。

### 2.4 服务器多客户端处理机制

为了实现服务器处理多个客户端的连接需求，采用以下方案：

- **I/O 复用**：使用  `epoll`  I/O 复用技术同时处理多个客户端连接，降低线程切换的开销。

服务器通过一个主循环不断监听来自客户端的连接请求，并根据数据包类型进行相应处理。



## 3. 数据库层设计

### 3.1 数据库选型

数据库采用 **MySQL** 关系型数据库进行持久化存储。MySQL 提供了良好的查询性能和事务支持，适合聊天软件需要的高频率数据读写和一致性要求。

### 3.2 数据表设计

为满足系统的需求，主要设计以下几张表：



##### 3.2.1 用户表 （user）

用来存储用户实体的信息

```mysql
CREATE TABLE user
(
    account   int AUTO_INCREMENT COMMENT '账号，且是主键'
        PRIMARY KEY,
    password  char(40)          NULL COMMENT '密码',
    nickname  char(40)          NULL COMMENT '昵称',
    is_online tinyint DEFAULT 0 NULL COMMENT '0为不在线，1为在线'
);
```



##### 3.2.2 群表 （group）

用来存储群实体的信息

```mysql
CREATE TABLE `group`
(
    account       int AUTO_INCREMENT COMMENT '群号'
        PRIMARY KEY,
    name          char(30)     NOT NULL COMMENT '群名',
    create_time   char(30)     NULL COMMENT '创建时间',
    notice        varchar(200) NULL COMMENT '群公告，群通知',
    owner_account int          NOT NULL,
    CONSTRAINT owner___fk
        FOREIGN KEY (owner_account) REFERENCES user (account)
);
```



##### 3.2.3 用户-群表 (user_group)

用户表和群表的中间表，用来表示用户和群的关系

```mysql
CREATE TABLE user_group
(
    user_account  int NOT NULL COMMENT '用户账户',
    group_account int NOT NULL COMMENT '群账号',
    CONSTRAINT user_group_uk
        UNIQUE (user_account, group_account),
    CONSTRAINT group_fk
        FOREIGN KEY (group_account) REFERENCES `group` (account)
            ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT user_fk
        FOREIGN KEY (user_account) REFERENCES user (account)
            ON UPDATE CASCADE ON DELETE CASCADE
)
    COMMENT '用户-群表，包括所有用户和群的关系';
```



##### 3.2.4 联系人表 （contact）

用来存储所有用户的好友关系

```mysql
CREATE TABLE contact
(
    user_account   int NOT NULL COMMENT '用户账号',
    friend_account int NOT NULL COMMENT '好友账号',
    CONSTRAINT contact_uk1
        UNIQUE (user_account, friend_account),
    CONSTRAINT contact_uk2
        UNIQUE (friend_account, user_account),
    CONSTRAINT contact_fk
        FOREIGN KEY (user_account) REFERENCES user (account)
            ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT friend_fk
        FOREIGN KEY (friend_account) REFERENCES user (account)
            ON UPDATE CASCADE ON DELETE CASCADE
);
```



##### 3.2.5 好友申请表 (friend_request)

存储所有好友申请信息

```mysql
CREATE TABLE friend_request
(
    request_account int      NOT NULL COMMENT '申请人账号（外键）',
    target_account  int      NOT NULL COMMENT '目标账号（外键）',
    time            char(30) NULL COMMENT '申请时间',
    CONSTRAINT friend_request_pk
        UNIQUE (request_account, target_account),
    CONSTRAINT friend_request_pk_2
        UNIQUE (target_account, request_account),
    CONSTRAINT request___fk
        FOREIGN KEY (request_account) REFERENCES user (account),
    CONSTRAINT target_fk
        FOREIGN KEY (target_account) REFERENCES user (account)
);
```



##### 3.2.6 群聊邀请表 (group_invitation)

存储所有群聊验证消息

```mysql
CREATE TABLE group_invitation
(
    invite_user_account  int      NOT NULL COMMENT '邀请人账号（外键',
    target_user_account  int      NOT NULL COMMENT '受邀人账号（外键）',
    target_group_account int      NOT NULL COMMENT '目标群账号（外键）',
    time                 char(30) NULL COMMENT '邀请时间',
    id                   int AUTO_INCREMENT COMMENT '编号'
        PRIMARY KEY,
    CONSTRAINT group_invitation_pk
        UNIQUE (invite_user_account, target_user_account, target_group_account),
    CONSTRAINT invite_user_fk
        FOREIGN KEY (invite_user_account) REFERENCES user (account)
            ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT target_group_fk
        FOREIGN KEY (target_group_account) REFERENCES `group` (account)
            ON UPDATE CASCADE ON DELETE CASCADE,
    CONSTRAINT target_user_fk
        FOREIGN KEY (target_user_account) REFERENCES user (account)
            ON UPDATE CASCADE ON DELETE CASCADE
)
    COMMENT '群聊验证消息表';
```





##### 3.2.7 单聊消息表 (single_message)

存储所有单聊消息

```mysql
CREATE TABLE single_message
(
    id              int AUTO_INCREMENT COMMENT '消息编号'
        PRIMARY KEY,
    send_account    int          NOT NULL COMMENT '发送人账号',
    receive_account int          NOT NULL COMMENT '接收人账号',
    send_time       char(30)     NULL COMMENT '消息发送时间',
    content         varchar(100) NULL COMMENT '消息内容'
)
```



##### 3.2.8 群聊消息表 (group_message)

存储所有群聊消息

```mysql
CREATE TABLE group_message
(
    id            int AUTO_INCREMENT COMMENT '消息编号'
        PRIMARY KEY,
    send_account  int          NOT NULL COMMENT '发送人账号',
    group_account int          NOT NULL COMMENT '群账号',
    send_time     char(30)     NULL COMMENT '消息发送时间',
    content       varchar(100) NULL COMMENT '消息内容'
);
```



##### 3.2.9 在线用户表 (online_user)

存储在内存中的表，存储当前所有已登录的在线用户

```mysql
CREATE TABLE online_user
(
    account                int AUTO_INCREMENT COMMENT '账号，且是主键'
        PRIMARY KEY,
    socket_fd              int           NOT NULL,
    private_chat_socket_fd int DEFAULT 0 NULL,
    group_chat_socket_fd   int DEFAULT 0 NULL
)
    ENGINE = MEMORY;
```





### 3.3 数据库交互流程

1. **用户登录**：用户登录时，服务器从 `user` 表中验证用户身份，并更新 `online_status`。
2. **消息发送**：当用户发送消息时，服务器会将消息存储到 `single_message` 或```group_message```表
3. **离线消息**：用户重新上线时，服务器会从 `single_message` 或```group_message```表中查询未送达的消息，并推送给用户。











# 四、环境与部署

##### 环境

- Linux
- mysql(8.0以上)

##### 依赖库

- ```-lmysqlclient```

##### 部署

仅支持本地部署

- IP：localhost
- 端口：8080

```sh
make client
make server

#必须先启动服务端
cd server
./main

#然后启动客户端
cd client
./main
```













# 五、更新计划

- 优化代码，检查返回值，减少崩溃概率

- 完善好友通知功能，即显示别人给用户发的申请，也显示用户发送的好友申请，并显示申请状态
- 完善群通知功能，既显示其他用户发来的群邀请，也显示用户发送的群邀请，并显示邀请状态
- 优化服务端性能，采用线程池加I\O复用结合的方式处理连接























































































































