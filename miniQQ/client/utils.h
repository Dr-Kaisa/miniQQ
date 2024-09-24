#ifndef __UTILS_H__
#define __UTILS_H__

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#endif

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

void get_now_time(char *timeString);
long int get_file_size(FILE *fp);

#endif