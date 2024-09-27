#include "../common/myThread.h"
#include "myService.h"
#define MAX_ONLINE_USER 1000

// extern thread_pool_t pool;
int main()
{

    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    init_socket(listen_fd);
    printf("listen_fd:%d\n", listen_fd);
    // 链接数据库
    MYSQL conn;
    // 通过数据库IP地址，用户进行远程连接（注意是数据库本身用来连接的IP，而不是请求方的IP，和'IP'@'root'中的IP无关
    mysql_ini_and_link(&conn, "localhost", "root");

    // 创建 epoll 实例
    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1)
    {
        perror("epoll_create");
        return -1;
    }

    // 监听可读事件
    struct epoll_event event, events[MAX_ONLINE_USER];
    memset(events, 0, sizeof(events));
    event.events = EPOLLIN;
    event.data.fd = listen_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event) == -1)
    {
        perror("epoll_ctl");
        close(epoll_fd);
        return -1;
    }

    // 初始化线程池
    // thread_pool_init();
    // while (1)
    char jsonData[1000];
    while (1)
    {
        int nfds = epoll_wait(epoll_fd, events, MAX_ONLINE_USER, -1); // 等待事件
        if (nfds == -1)
        {
            perror("epoll_wait");
            break;
        }
        // 处理事件
        for (int i = 0; i < nfds; ++i)
        {
            int target_fd = events[i].data.fd;
            if (target_fd == listen_fd)
            {

                handleConnection(epoll_fd, listen_fd);
            }
            else if (events[i].events == EPOLLIN)
            {
                int ret = myRecv(jsonData, target_fd);
                if (ret == -1) // 没读到数据，说明客户端断开连接
                {
                    printf("处理断开连接");
                    handleDisconnect(epoll_fd, events[i].data.fd, &conn);
                }
                else
                {
                    printf("处理消息");
                    handleMessage(jsonData, target_fd, &conn);
                }
            }
        }
    }
    // 清理工作
    mysql_close(&conn);
    return 0;
}