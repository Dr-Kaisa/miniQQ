#include "myThread.h"

thread_pool_t pool;
// 工作线程执行的函数
void *thread_worker(void *arg)
{
    while (1)
    {
        task_t task;
        pthread_mutex_lock(&pool.lock);

        // 阻塞等待任务
        while (pool.queue_size == 0)
        {
            pthread_cond_wait(&pool.notify, &pool.lock);
        }

        // 取出任务
        task.function = pool.queue[pool.head].function;
        task.arg = pool.queue[pool.head].arg;
        pool.head = (pool.head + 1) % MAX_QUEUE;
        pool.queue_size--;

        pthread_mutex_unlock(&pool.lock);

        // 执行任务
        (*(task.function))(task.arg);
    }
    return NULL;
}

// 线程池初始化
void thread_pool_init()
{
    pool.queue_size = 0;
    pool.head = 0;
    pool.tail = 0;
    pthread_mutex_init(&pool.lock, NULL);
    pthread_cond_init(&pool.notify, NULL);

    // 创建工作线程
    for (int i = 0; i < MAX_THREADS; i++)
    {
        pthread_create(&pool.threads[i], NULL, thread_worker, NULL);
    }
}

// 向线程池提交任务
void thread_pool_add_task(void (*function)(void *), void *arg)
{
    pthread_mutex_lock(&pool.lock);

    // 将任务添加到队列
    pool.queue[pool.tail].function = function;
    pool.queue[pool.tail].arg = arg;
    pool.tail = (pool.tail + 1) % MAX_QUEUE;
    pool.queue_size++;

    pthread_cond_signal(&pool.notify); // 唤醒一个等待的工作线程
    pthread_mutex_unlock(&pool.lock);
}
