#include "menu.h"
extern pthread_mutex_t mutex;
int main()
{
    boot();
    User this;
    memset(&this, 0, sizeof(User));
    // 建立连接
    int socket_fd = init_socket("192.168.174.100", 8080);

    // 保存连接信息
    this.socket_fd = socket_fd;
    // 显示登录页面
    show_login_menu(&this);
    // 关闭连接
    close(socket_fd);

    return 0;
}