#include <iostream>
#include <fstream>
#include "threadpool.h"
void *Query(void* args1, void* args2, void* args3,
            void* args4, void* args5, void* args6)
{
    std::cout << "hello" << std::endl;
}

int main() {
    std::cout << "Hello, World!" << std::endl;
    ThreadPool pool(4);
    pool.addTask(Query,NULL,NULL,NULL,NULL,NULL,NULL);
    return 0;
}