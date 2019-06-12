#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"
#include "EventLoop.h"
#include "Logging/base/Logging.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop* loop)
    :baseLoop_(loop),
    started_(false),
    numThread_(0),
    nextLoop_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::start()
{
    baseLoop_->assertInLoopThread();
    started_ = true;
    LOG_LOG << "EventLoopThreadPool " << numThread_;
    for(int i = 0; i < numThread_; ++i)
    {
        EventLoopThread* thread = new EventLoopThread();
        threadLists_.emplace_back(thread);
        loopLists_.emplace_back(thread->start());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    EventLoop* loop = baseLoop_;
    if(!loopLists_.empty())
    {
        loop = loopLists_[nextLoop_];
        if(++nextLoop_ >= loopLists_.size())
            nextLoop_ = 0;
    }
    return loop;
}
