#ifndef FILESERV_TIMERQUEUE_H
#define FILESERV_TIMERQUEUE_H

#include <functional>
#include <vector>

#include "TimerHeap.hpp"
#include "Channel.h"

using TimerCallback = std::function<void()>;

class EventLoop;

struct TimerItem
{
    bool loop;//是否重复
    itimerspec ispec;
    TimerCallback cb;
    int sec;
    TimerItem():loop(false),cb(NULL),sec(0){}
    TimerItem(int sec, TimerCallback callback, bool lp);
    bool operator==(const TimerItem& rhs)
    {
        return loop == rhs.loop && sec == rhs.sec && ispec.it_value.tv_sec == ispec.it_value.tv_sec;
    }
};

bool timerCmp(const TimerItem& lhs, const TimerItem& rhs);

class TimerQueue
{
public: 
    TimerQueue(EventLoop* loop);
    ~TimerQueue();

    TimerQueue(const TimerQueue&) = delete;
    TimerQueue& operator=(const TimerQueue&) = delete;

    void addTimer(int sec, TimerCallback cb, bool lp);

private: 
    void readdTimerInLoop();
    void handleRead();
    void addTimerInLoop(TimerItem& item);
    
private: 
    EventLoop* loop_;
    int timerfd_;
    ChannelPtr channelPtr_;
    TimerHeap<TimerItem> heap_;
    std::vector<TimerItem> cancelTimer_;
    TimerCallback callback_;
    TimerItem now_;
};

#endif // !FILESERV_TIMERQUEUE_H