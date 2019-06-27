#ifndef FILESERV_EVENTLOOP_H
#define FILESERV_EVENTLOOP_H

#include "Logging/base/noncopyable.h"
#include "Epoller.h"
#include "TimerQueue.h"
#include "Logging/base/MutexLock.h"

#include <functional>
#include <vector>
#include <memory>

class EventLoop : noncopyable
{
    typedef std::function<void()> Functor;
public: 
    EventLoop();
    ~EventLoop();
    void loop();
    void quit();
    void runInLoop(Functor func);
    bool assertInLoop();
    void assertInLoopThread();
    void queueInLoop(Functor func);

    void updateChannel(const ChannelPtr& channel);
    void removeChannel(const ChannelPtr& channel);

    void runAt(int sec, TimerCallback cb);
    void runEveryN(int sec, TimerCallback cb);

private: 
    std::unique_ptr<Epoller> EpollPtr_;
    std::vector<Functor> FuncList_;
    bool looping_;
    int wakeUpFd_;
    ChannelPtr wakeupChannel_;
    MutexLock mutex_;
    std::unique_ptr<TimerQueue> timerQueuePtr_;
    const int threadId_;
    
    void wakeup();
    void doPendingFunc();
    void handleRead();
};

#endif // !EVENTLOOP_H
