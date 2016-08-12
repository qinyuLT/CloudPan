/*************************************************************************
	> File Name: tPool.h
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月24日 星期日 16时53分08秒
 ************************************************************************/
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <pthread.h>

//任务节点
typedef struct tpool_work
{
    void *            (*routine)(void*);//任务函数
    void              *arg;             //传入任务函数的参数
    struct tpool_work *next;
}tpool_work_t;

typedef struct tpool 
{
    int              shudown;          //线程池是否销毁
    int              max_thr_num;      //最大线程数
    pthread_t        *thr_id;          //线程ID组数
    tpool_work_t     *queue_head;
    tpool_work_t     *queue_tail;
    pthread_mutex_t  queue_lock;
    pthread_cond_t   queue_ready;
}tpool_t;

//创建线程池
int  createPool(int max_thr_num);
//销毁线程池
void destroyPool();
//向线程池添加任务
int  addWorkToPool(void*(*routine)(void*),void *arg);


#endif
