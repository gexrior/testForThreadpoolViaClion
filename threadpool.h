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

#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "condition.h"
#include <stdio.h>
#include <stdlib.h>
#include <queue>

void* thread_routine(void *args);

class ThreadPool
{
    friend void* thread_routine(void *args);
private:
    // type of callback function
    typedef void *(*callback_t)(void *,void *,void *,void *,void *,void *);
    // structure of task
    struct task_t
    {
        callback_t run; //callback function
        void* args1; //args of callback function
        void* args2;
        void* args3;
        void* args4;
        void* args5;
        void* args6;
    };
public:
    ThreadPool(int _maxThreads = 36, unsigned int _waitSeconds = 1000);
    ~ThreadPool();
    //interface for adding tasks
    void addTask(callback_t run, void *args1, void *args2,
                 void* args3, void* args4, void* args5, void* args6);
    bool is_finish()
    {
        if(quit == true && taskQueue.empty())
        {
            return true;
        }
        else
        {
            return false;
        }
    }
private:
    void startTask();
private:
    Condition ready;     // ready for task queue or destroying thread pool
    std::queue<task_t *> taskQueue; //task queue

    unsigned int maxThreads;  //max thread num
    unsigned int counter;  //current num of threads
    unsigned int idle;     //num of idle threads
    unsigned int waitSeconds;  //wait time for thread
    bool quit;  //tag of destroying thread pool
};

#endif // THREADPOOL_H
