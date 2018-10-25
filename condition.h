/*
    Copyright 2018. simba wei

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

#ifndef CONDITION_H
#define CONDITION_H

#include "pthread.h"
#include <time.h>

class Condition
{
public:
    Condition(const pthread_mutexattr_t *mutexAttr = NULL,
              const pthread_condattr_t  *condAttr = NULL);
    ~Condition();
    //条件变量函数
    int signal();
    int broadcast();
    int wait();
    int timedwait(int seconds);

    //互斥量函数
    int lock();
    int trylock();
    int unlock();

private:
    //pthread_mutex_t： 互斥锁，多线程中对共享变量的包保护
    //pthread_cond_t： 线程间同步，一般和pthread_mutex_t一起使用，以防止出现逻辑错误，即如果单独使用条件变量，某些情况下（条件变量前后出现对共享变量的读写）会出现问题
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_cond;
};

#endif // CONDITION_H
