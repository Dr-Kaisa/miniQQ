#ifndef __MYDB_H__
#define __MYDB_H__

#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
void mysql_ini_and_link(MYSQL *conn, char *IP, char *user);
#endif