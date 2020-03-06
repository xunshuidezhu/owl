#include <bits/stdc++.h>
#include <functional>
#include <iostream>
#include <pthread.h>
#include <set>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace std;

int fd;

void* readMsg(void* arg)
{
    unsigned long long buffer;
    read(fd, &buffer, 8);
    printf("%d", buffer);
}

void* writeMsg(void* arg)
{
    unsigned long long buffer = 31;
    write(fd, &buffer, 8);
}

// Definition for singly-linked list.
struct ListNode {
    int val;
    ListNode* next;
    ListNode(int x)
        : val(x)
        , next(NULL)
    {
    }
};

int main()
{   
    ifstream fin();

    fd = eventfd(0, 0);

    //    printf("%d", n);
    pthread_t tid1, tid2;
    pthread_create(&tid2, nullptr, readMsg, 0);
    sleep(2);
    pthread_create(&tid1, nullptr, writeMsg, 0);
    pthread_join(tid1, nullptr);
    pthread_join(tid2, nullptr);
    std::set<int> s;
    s.insert(2);
    s.insert(2);
    std::cout << std::endl;
    std::cout << s.count(2) << std::endl;
}