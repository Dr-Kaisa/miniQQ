#include "myDB.h"

void mysql_ini_and_link(MYSQL *conn, char *IP, char *user)
{
    // 初始化数据库
    mysql_init(conn);
    // 链接数据库
    MYSQL *ret = mysql_real_connect(conn, IP, user, "1", "miniQQ", 3306, NULL, 0);
    if (ret == NULL)
    {
        printf("连接数据库失败: %s\n", mysql_error(conn));
        exit(0);
    }
}