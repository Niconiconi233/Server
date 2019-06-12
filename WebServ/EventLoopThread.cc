#include "EventLoopThread.h"

#include <functional>
#include <cassert>

#include "EventLoop.h"

EventLoopThread::EventLoopThread()
    :thread_(std::bind(&EventLoopThread::threadFunc, this)),
    loop_(nullptr),
    mutex_(),
    cond_(mutex_)
{

}

EventLoopThread::~EventLoopThread()
{
    if(loop_ != nullptr)
    {
        loop_->quit();
        thread_.join();
    }
}

EventLoop* EventLoopThread::start()
{
    assert(!thread_.started());
    thread_.start();
    EventLoop* loop = nullptr;
    {
        MutexLockGurard lock(mutex_);
        while(loop_ == nullptr)
        {
            cond_.wait();
        }
        loop = loop_;
    }
    return loop;
}

void EventLoopThread::threadFunc()
{
    EventLoop loop;
    {
        MutexLockGurard lock(mutex_);
        loop_ = &loop;
        cond_.notify();
    }
    loop.loop();
}