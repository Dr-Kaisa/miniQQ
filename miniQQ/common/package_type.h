#ifndef __PACKAGE_TYPE_H__
#define __PACKAGE_TYPE_H__

enum TYPE
{
    SIGN_IN = 1,    // 请求登录
    REPLY_SIGN_IN,  // 服务端回应登录请求
    SIGN_UP,        // 请求注册
    SEND_ACCOUNT,   // 服务端完成注册，并返回新账号给客户端
    DELETE_ACCOUNT, // 请求删除账号

    // 我的好友
    GET_CONTACTS,         // 获取联系人列表
    REPLY_GET_CONTACTS,   // 服务器端返回联系人列表
    REMOVE_CONTACT,       // 客户端申请删除好友
    PRIVATE_CHAT,         // 客户端申请发起单聊
    REQUEST_PRIVATE_CHAT, // 客户端建立单聊专用TCP通道后，请求将通道保存至数据库
    REPLY_PRIVATE_CHAT,   // 服务端同意单聊请求，并将历史聊天记录返回
    SEND_PRIVATE_MES,     // 客户端通过专用TCP通道发送的单聊消息
    // 添加好友
    ADD_CONTACT, // 客户端申请添加好友
    // 好友通知
    GET_CONTACT_NOTIFICATIONS,       // 客户端申请获取好友通知
    REPLY_GET_CONTACT_NOTIFICATIONS, // 服务端同意申请并返回好友通知
    AGREE_ADD_CONTACT,               // 客户端申请同意好友申请
    REFUSE_ADD_CONTACT,              // 客户端申请拒绝好友申请
    // 我的群聊
    GET_GROUPS,         // 客户端申请获取群聊列表
    REPLY_GET_GROUPS,   // 服务端同意申请，并返回群聊
    GROUP_CHAT,         // 客户端申请进行群聊
    REQUEST_GROUP_CHAT, // 客户端建立群聊专用TCP通道后，申请将通道保存至数据库
    REPLY_GROUP_CHAT,   // 服务端同意群聊请求，并将群历史消息返回
    INVITE_TO_GROUP,    // 客户端申请向好友发起加群邀请
    SEND_GROUP_MES,     // 客户端申请发送群聊消息
    LEAVE_GROUP,        // 客户端申请退出群聊
    // 创建群聊
    CREATE_GROUP, // 客户端申请创建群聊

    // 群聊通知
    GET_GROUP_NOTIFICATIONS,       // 客户端申请获取群通知
    REPLY_GET_GROUP_NOTIFICATIONS, // 服务端同意申请，并返回群通知
    AGREE_JOIN_GROUP,              // 客户端申请同意加群邀请
    REFUSE_JOIN_GROUP,             // 客户端申请拒绝加群邀请

    // 退出登录
    SIGN_OUT, // 客户端申请退出登录状态

    OK,    // 服务端同意申请（通用）
    ERROR, // 客户端返回错误信息

};

#endif