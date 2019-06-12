#ifndef FILESERV_EVENTLOOPTHREAD_H
#define FILESERV_EVENTLOOPTHREAD_H

#include "Logging/base/Thread.h"
#include "Logging/base/MutexLock.h"
#include "Logging/base/Condition.h"

class EventLoop;

class EventLoopThread
{
public: 
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* start();


private: 
    void threadFunc();
    Thread thread_;
    EventLoop* loop_;
    MutexLock mutex_;
    Condition cond_;
    
};

#endif // !FILESERV_EVENTLOOPTHREAD_H
