/*
    Copyright 2018, simba wei.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "threadpool.h"
#include "iostream"
using namespace std;
ThreadPool::ThreadPool(int _maxThreads, unsigned int _waitSeconds)
    : maxThreads(_maxThreads), counter(0), idle(0),
      waitSeconds(_waitSeconds), quit(false){}

//entrance for thread
//It is a consumer thread
void* thread_routine(void *args)
{
    pthread_detach(pthread_self());
    printf("*thread 0x%lx is starting...\n", (unsigned long)pthread_self());
    ThreadPool *pool = (ThreadPool *)args;

    while (true)
    {
        bool timeout = false;

        pool->ready.lock();
        //当处于等待的时候, 则说明空闲的线程多了一个
        ++ pool->idle;

        //pool->ready中的条件变量有三个作用:
        // 1.等待任务队列中有任务到来
        // 2.等待线程池销毁通知
        // 3.确保当等待超时的时候, 能够将线程销毁(线程退出)
        while (pool->taskQueue.empty() && pool->quit == false)
        {
            printf("thread 0x%lx is waiting...\n", (unsigned long)pthread_self());
            //等待waitSeconds
            if (0 != pool->ready.timedwait(pool->waitSeconds))
            {
                //如果等待超时
                printf("thread 0x%lx is wait timeout ...\n", (unsigned long)pthread_self());
                timeout = true;
                //break出循环, 继续向下执行, 会执行到下面第1个if处
                break;
            }
        }
        //条件成熟(当等待结束), 线程开始执行任务或者是线程销毁, 则说明空闲线程又少了一个
        -- pool->idle;

        // 状态3.如果等待超时(一般此时任务队列已经空了)
        if (timeout == true && pool->taskQueue.empty())
        {
            -- pool->counter; //current num of threads -1
            //解锁然后跳出循环, 直接销毁线程(退出线程)
            pool->ready.unlock();
            break;
        }

        // 状态2.如果是等待到了线程的销毁通知, 且任务都执行完毕了
        if (pool->quit == true && pool->taskQueue.empty())
        {
            -- pool->counter;
            //如果没有线程了, 则给线程池发送通知
            //告诉线程池, 池中已经没有线程了
            if (pool->counter == 0)
                pool->ready.signal();
            //解锁然后跳出循环
            pool->ready.unlock();
            break;
        }

        // 状态1.如果是有任务了, 则执行任务
        if (!(pool->taskQueue.empty()))
        {
            //从队头取出任务进行处理
            ThreadPool::task_t *t = pool->taskQueue.front();
            pool->taskQueue.pop();

            //执行任务需要一定的时间
            //解锁以便于其他的生产者可以继续生产任务, 其他的消费者也可以消费任务
            pool->ready.unlock();
            //处理任务
            t->run(t->args1,t->args2,t->args3,t->args4,t->args5,t->args6);
            delete t;
        }
    }

    //跳出循环之后, 打印退出信息, 然后销毁线程
    printf("thread 0x%lx is exiting...\n", (unsigned long)pthread_self());
    pthread_exit(NULL);
}

//线程启动函数
void ThreadPool::startTask()
{
    // 如果有等待线程, 则唤醒其中一个, 让它来执行任务
    if (idle > 0)
        ready.signal();
    // 没有等待线程, 而且当前先线程总数尚未达到阈值, 我们就需要创建一个新的线程
    else if (counter < maxThreads)
    {
        pthread_t tid;
        pthread_create(&tid, NULL, thread_routine, this);
        ++ counter;
    }
}

void ThreadPool::addTask(callback_t run, void *args1, void *args2,
                         void* args3, void* args4, void* args5, void* args6)
{
    /** 1. 生成任务并将任务添加到"任务队列"队尾 **/
    task_t *newTask = new task_t();
    newTask->run = run;
    newTask->args1 = args1;
    newTask->args2 = args2;
    newTask->args3 = args3;
    newTask->args4 = args4;
    newTask->args5 = args5;
    newTask->args6 = args6;

    ready.lock();   //注意需要使用互斥量保护共享变量
    taskQueue.push(newTask);

    /** 2. 让线程开始执行任务 **/
    startTask();
    ready.unlock();//解锁以使任务开始执行
}

ThreadPool::~ThreadPool()
{
    //如果已经调用过了, 则直接返回
    if (quit == true)
        return;

    ready.lock();
    quit = true;
    if (counter > 0)
    {
        //对于处于等待状态, 则给他们发送通知,
        //这些处于等待状态的线程, 则会接收到通知,
        //然后直接退出
        if (idle > 0)
            ready.broadcast();

        //对于正处于执行任务的线程, 他们接收不到这些通知,
        //则需要等待他们执行完任务
        while (counter > 0)
            ready.wait();
    }
    ready.unlock();
}
