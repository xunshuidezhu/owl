
/*****************************************************************
 * 计时器测试代码，
 * 1)sleep 2 s, 定时1s,触发超时函数
 * 2)定时5000ms，sleep 2s，不触发超时函数
 * ***************************************************************/
#include "timer.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>

void cb(ClientData* data)
{
    std::cout << "time out " << std::endl;
}

int main()
{
    HeapTimerContainer heap(1);
    HeapTimer* timer = new HeapTimer(5000);
    timer->setCallback(cb);
    heap.add_timer(timer);
    sleep(2);
    heap.tick();
    struct timeval time;

    struct timeval now;
    struct timeval now1;
    gettimeofday(&now, 0);
    gettimeofday(&now1, 0);
    heap.top(time);
    std::cout << (time.tv_sec - now.tv_sec) * 1000 << std::endl;
    return 0;
}