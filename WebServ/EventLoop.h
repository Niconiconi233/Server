#ifndef FILESERV_EVENTLOOP_H
#define FILESERV_EVENTLOOP_H

#include "Logging/base/noncopyable.h"
#include "Epoller.h"
#include "Logging/base/MutexLock.h"

#include <functional>
#include <vector>
#include <memory>

class EventLoop : noncopyable
{
    typedef std::function<void()> Functor;
public: 
    EventLoop();
    void loop();
    void quit();
    void runInLoop(Functor func);
    bool assertInLoop();
    void assertInLoopThread();
    void queueInLoop(Functor func);

    void updateChannel(const ChannelPtr& channel);
    void removeChannel(const ChannelPtr& channel);

private: 
    std::unique_ptr<Epoller> EpollPtr_;
    std::vector<Functor> FuncList_;
    bool looping_;
    int wakeUpFd_;
    ChannelPtr wakeupChannel;
    MutexLock mutex_;
    
    void wakeup();
    void doPendingFunc();
    void handleRead();
};

#endif // !EVENTLOOP_H
