#include "coroutine.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ucontext.h>

#define STACK_SIZE (1024 * 1024)
#define DEFAULT_COROUTINE 16

struct coroutine;
//调度器
struct schedule {
    char stack[STACK_SIZE];
    ucontext_t main;
    //协程数量
    int nco;
    //容量
    int cap;
    int running;
    //协程双重指针
    struct coroutine** co;
};

struct coroutine {
    coroutine_func func;
    //TODO:ud arg
    void* ud;
    ucontext_t ctx;
    struct schedule* sch;
    //容量
    ptrdiff_t cap;
    //携程大小
    ptrdiff_t size;
    int status;
    //协程栈
    char* stack;
};
//协程初始化
struct coroutine* _co_new(struct schedule* S, coroutine_func func, void* ud)
{
    struct coroutine* co = (struct coroutine*)malloc(sizeof(*co));
    co->func = func;
    co->ud = ud;
    co->sch = S;
    co->cap = 0;

    co->size = 0;
    co->status = COROUTINE_READY;
    //初始栈为空
    co->stack = NULL;
    return co;
}
//释放协程栈内存，释放携程
void _co_delete(struct coroutine* co)
{
    free(co->stack);
    free(co);
}
//开启携程调度器
struct schedule* coroutine_open(void)
{
    //动态创建调度器
    struct schedule* S = (struct schedule*)malloc(sizeof(*S));
    //调度器管理的协程数量是0
    S->nco = 0;
    //容量默认是１６
    S->cap = DEFAULT_COROUTINE;
    //协程运行标志
    S->running = -1;
    //创建容量个携程内存,存容量个协程指针
    S->co = (struct coroutine**)malloc(sizeof(struct coroutine*) * S->cap);
    //初始化置０
    memset(S->co, 0, sizeof(struct coroutine*) * S->cap);
    //返回调度器
    return S;
}
//关闭协程调度器
void coroutine_close(struct schedule* S)
{
    int i;
    //销毁协程调度器的所有协程
    for (i = 0; i < S->cap; i++) {
        struct coroutine* co = S->co[i];
        if (co) {
            _co_delete(co);
        }
    }
    //销毁协程双重指针
    free(S->co);
    //防止悬空指针
    S->co = NULL;
    //释放调度器
    free(S);
}
//创建一个协程
/**
 * @arg     func :　执行协程的回调函数
 * @arg     S    :  调度器
 * ＠return  id  :　返回创建的协程的序号
 * */
int coroutine_new(struct schedule* S, coroutine_func func, void* ud)
{
    //协程初始化
    struct coroutine* co = _co_new(S, func, ud);
    printf("----debug----%d ------- %d", sizeof(struct coroutine*), __LINE__);
    //调度器管理的协程数大于容量
    if (S->nco >= S->cap) {
        int id = S->cap;
        //协程空间扩容2倍（存的是协程指针）
        S->co = (struct coroutine**)realloc(S->co, S->cap * 2 * sizeof(struct coroutine*));
        //新扩容的协程空间置0
        memset(S->co + S->cap, 0, sizeof(struct coroutine*) * S->cap);
        //添加新创建的协程
        S->co[S->cap] = co;
        //容量＊２
        S->cap *= 2;
        //调度器管理的协程数＋１
        ++S->nco;
        //返回创建协程的序号
        return id;
    }
    //调度器开辟的协程空间还没有装满
    else {
        int i;
        //找到空的内存把新创建的协程放到里面
        for (i = 0; i < S->cap; i++) {
            int id = (i + S->nco) % S->cap;
            if (S->co[id] == NULL) {
                S->co[id] = co;
                ++S->nco;
                //返回创建的协程的序号
                return id;
            }
        }
    }
    //其他情况不存在
    assert(0);
    return -1;
}

static void mainfunc(uint32_t low32, uint32_t hi32)
{
    uintptr_t ptr = (uintptr_t)low32 | ((uintptr_t)hi32 << 32);
    struct schedule* S = (struct schedule*)ptr;
    int id = S->running;
    struct coroutine* C = S->co[id];
    C->func(S, C->ud);
    _co_delete(C);
    S->co[id] = NULL;
    --S->nco;
    S->running = -1;
}

//唤醒协程
void coroutine_resume(struct schedule* S, int id)
{
    //只能唤醒没有运行的协程
    assert(S->running == -1);
    assert(id >= 0 && id < S->cap);
    struct coroutine* C = S->co[id];
    if (C == NULL)
        return;
    int status = C->status;
    //判断协程状态
    switch (status) {
    // 协程准备好的状态
    case COROUTINE_READY: {
        getcontext(&C->ctx);
        // 分配栈空间
        C->ctx.uc_stack.ss_sp = S->stack;
        // 栈空间大小
        C->ctx.uc_stack.ss_size = STACK_SIZE;
        //　协程执行完切换到主协程
        C->ctx.uc_link = &S->main;
        S->running = id;
        // 状态改变为在执行状态
        C->status = COROUTINE_RUNNING;
        //
        uintptr_t ptr = (uintptr_t)S;
        //　执行协程
        makecontext(&C->ctx, (void (*)(void))mainfunc, 2, (uint32_t)ptr, (uint32_t)(ptr >> 32));
        //切换上下文到协程
        swapcontext(&S->main, &C->ctx);
        break;
    }
        // 获取要执行的协程的上下文

    //挂起协程状态
    case COROUTINE_SUSPEND: {
        //把协程的栈放在栈后面
        memcpy(S->stack + STACK_SIZE - C->size, C->stack, C->size);
        S->running = id;
        C->status = COROUTINE_RUNNING;
        swapcontext(&S->main, &C->ctx);
        break;
    }

    default:
        assert(0);
    }
}

static void _save_stack(struct coroutine* C, char* top)
{
    char dummy = 0;
    assert(top - &dummy <= STACK_SIZE);
    //调度器分配栈大小不够，重新分配
    if (C->cap < top - &dummy) {
        free(C->stack);
        C->cap = top - &dummy;
        C->stack = (char*)malloc(C->cap);
    }
    C->size = top - &dummy;
    memcpy(C->stack, &dummy, C->size);
}
//挂起协程
void coroutine_yield(struct schedule* S)
{
    int id = S->running;
    //协程必须处于执行态
    assert(id >= 0);
    struct coroutine* C = S->co[id];
    //协程内存位于调度器管理的栈中
    assert((char*)&C > S->stack);
    _save_stack(C, S->stack + STACK_SIZE);
    //状态变成挂起状态
    C->status = COROUTINE_SUSPEND;

    S->running = -1;
    swapcontext(&C->ctx, &S->main);
}
//查询某个协程的状态
int coroutine_status(struct schedule* S, int id)
{
    assert(id >= 0 && id < S->cap);
    if (S->co[id] == NULL) {
        return COROUTINE_DEAD;
    }
    return S->co[id]->status;
}
//返回正在执行协程的Id
//线程的同一时刻只能存在一个在执行的协程
int coroutine_running(struct schedule* S)
{
    return S->running;
}
