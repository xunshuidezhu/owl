#ifndef TIMER_H
#define TIMER_H

#include <functional>

typedef std::function<void(void*)> callback;

class Timer {
public:
    Timer();

    virtual ~Timer() = 0;
};

Timer::~Timer()
{
}

class HeapTimer : public Timer {
public:
    callback cb;
public:
    HeapTimer();

    virtual ~HeapTimer() override;
};

class TimerContainer {
public:
    
    virtual void addTimer(Timer*) = 0;

    virtual bool haveTimer() = 0;

    virtual void doCallback(callback c) = 0;
};

class HeapTimerContainer : public TimerContainer {
public:
    virtual void addTimer(Timer* timer) override;

    virtual bool haveTimer() override;

    void doCallback(callback cb);
};
#endif