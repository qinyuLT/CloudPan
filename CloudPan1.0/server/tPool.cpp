/*************************************************************************
	> File Name: tPool.cpp
	> Author: qinyu
	> Mail: qinyu.LT@gmail.com 
	> Created Time: 2016年04月24日 星期日 17时10分01秒
 ************************************************************************/
#include "tPool.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
//#include <iostream>
//using namespace std;

/*
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
*/
static tpool_t *tpool = NULL;

//工作线程函数，从任务链表取出任务并执行
static void *thread_routine(void *arg)
{
    tpool_work_t *work;

    while(1)
    {
        //任务列表为空 且 线程池未关闭：线程阻塞等待任务
        pthread_mutex_lock(&tpool->queue_lock);
        while(!tpool->queue_head && !tpool->shudown)
        {
            pthread_cond_wait(&tpool->queue_ready,&tpool->queue_lock);
        }

        //如果线程池关闭，退出
        if(tpool->shudown)
        {
            pthread_mutex_unlock(&tpool->queue_lock);
            pthread_exit(NULL);
        }

        //从任务列表取任务并执行
        work = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        pthread_mutex_unlock(&tpool->queue_lock);
        work->routine(work->arg);

        //线程完成，释放任务
        free(work->arg);
        free(work);
    }

    return NULL;
}

//创建线程池
int createPool(int max_thr_num)
{
    int i;
    tpool = (tpool_t*)calloc(1,sizeof(tpool_t));
    if(!tpool)
    {
        printf("%s: calloc tpool failed\n",__FUNCTION__);
        exit(1);
    }
    
    //初始化任务链表,互斥量，条件变量
    tpool->max_thr_num = max_thr_num;
    tpool->shudown = 0;
    tpool->queue_head = NULL;
    tpool->queue_tail = NULL;
    if(pthread_mutex_init(&tpool->queue_lock,NULL) != 0)
    {
        printf("%s :pthread_mutex_init failed,errno:%d,errno:%s\n",__FUNCTION__,errno,strerror(errno));
        exit(-1);
    }

    if(pthread_cond_init(&tpool->queue_ready,NULL) != 0)
    {
        printf("%s:pthread_cond_init failed,errno:%d,errno:%s\n",__FUNCTION__,errno,strerror(errno));
        exit(-1);
    }

    //创建工作线程
    tpool->thr_id = (pthread_t*)calloc(max_thr_num,sizeof(pthread_t));
    if(!tpool->thr_id)
    {
        printf("%s:calloc thr_id failed\n",__FUNCTION__);
        exit(1);
    }

    for(i=0;i<max_thr_num;++i)
    {
        if(pthread_create(&tpool->thr_id[i],NULL,thread_routine,NULL) != 0)
        {
            printf("%s:pthread_create failed,errno:%d,errno:%s\n",__FUNCTION__,errno,strerror(errno));
            exit(-1);
        }
    }
    return 0;
}

//销毁线程池
void destroyPool()
{
    int i;
    tpool_work_t *member;
    if(tpool->shudown)return;
    
    //关闭线程池开关
    tpool->shudown = 1;

    //唤醒所以阻塞的线程
    pthread_mutex_lock(&tpool->queue_lock);
    pthread_cond_broadcast(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);

    //回收结束线程的剩余资源
    for(i=0;i<tpool->max_thr_num;++i)
    {
        pthread_join(tpool->thr_id[i],NULL);
    }

    //释放threadID数组
    free(tpool->thr_id);

    //释放未完成的任务
    while(tpool->queue_head)
    {
        member = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        free(member->arg);
        free(member);
    }

    //销毁互斥量，条件变量
    pthread_mutex_destroy(&tpool->queue_lock);
    pthread_cond_destroy(&tpool->queue_ready);

    //释放线程池结构体
    free(tpool);
}

//向线程池添加任务
int  addWorkToPool(void*(*routine)(void*),void *arg)
{
    tpool_work_t *work;

    if(!routine)
    {
        printf("%s:Invalid argument\n",__FUNCTION__);
        return -1;
    }

    //work指向等待加入任务链表的任务
    work = (tpool_work_t*)malloc(sizeof(tpool_work_t));
    if(!work)
    {
        printf("%s:malloc failed\n",__FUNCTION__);
        return -1;
    }
    work->routine = routine;
    work->arg = arg;
    work->next = NULL;

    //将任务节点添加到任务链表
    pthread_mutex_lock(&tpool->queue_lock);
    if(!tpool->queue_head)
    {
        tpool->queue_head = work;
        tpool->queue_tail = work;
    }
    else
    {
        tpool->queue_tail->next = work;
        tpool->queue_tail = work;
    }

    //通知工作线程，有新任务添加
    pthread_cond_signal(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);
    return 0;
}
/*
int main()
{
    if(createPool(4))
    {
        printf("thread pool create failed!\n");
        exit(-1);
    }
    printf("-- Thread Pool Strart --\n");

    return 0;
}*/
