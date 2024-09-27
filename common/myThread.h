#ifndef __MYTHREAD_H__
#define __MYTHREAD_H__

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// 最大线程数
#define MAX_THREADS 4
// 最大任务数
#define MAX_QUEUE 10

typedef struct
{
    void (*function)(void *); // 函数指针，指向需要处理的任务
    void *arg;                // 任务的参数
} task_t;

typedef struct
{
    pthread_t threads[MAX_THREADS];
    task_t queue[MAX_QUEUE];
    int queue_size;
    int head;
    int tail;
    pthread_mutex_t lock;
    pthread_cond_t notify;
} thread_pool_t;

void *thread_worker(void *arg);
void thread_pool_init();
void thread_pool_add_task(void (*function)(void *), void *arg);

#endif