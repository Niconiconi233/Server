#include "EventLoop.h"
#include "Channel.h"
#include "Logging/base/Logging.h"

namespace{
    #include <unistd.h>
    #include <sys/syscall.h>
    #include <sys/eventfd.h>
    #include <signal.h>
    int getThreadId()
    {
        return syscall(SYS_gettid);
    }

    int createFd()
    {
        int fd = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
        if(fd < 0)
        {
                LOG_ERROR << "Epoll createFd failed ";
                abort();
        }
        LOG_LOG << "create eventfd fd = " << fd;
        return fd;

    }
#pragma GCC diagnostic ignored "-Wold-style-cast"
class IgnoreSigPipe
{
 public:
  IgnoreSigPipe()
  {
    ::signal(SIGPIPE, SIG_IGN);
    //LOG_LOG << "ignoreSigPipe";
  }
};
#pragma GCC diagnostic error "-Wold-style-cast"

IgnoreSigPipe initObj;
}

__thread int threadid_;

EventLoop::EventLoop()
    :EpollPtr_(new Epoller()),
    looping_(false),
    wakeUpFd_(createFd()),
    wakeupChannel_(new Channel(this, wakeUpFd_)),
    mutex_(),
    timerQueuePtr_(new TimerQueue(this))
{
    threadid_ = getThreadId();
    wakeupChannel_->enableRead();
    LOG_LOG << "EventLoop::EventLoop created int thread " << threadid_;
}

EventLoop::~EventLoop()
{
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeUpFd_);
    EpollPtr_.reset();
    timerQueuePtr_.reset();
}

void EventLoop::loop()
{
    LOG_LOG << "EventLoop::loop in thread " << threadid_;
    looping_ = true;
    while(looping_)
    {
        std::vector<ChannelPtr> lists = EpollPtr_->poll();

        for(auto it = lists.begin(); it != lists.end(); ++it)
            (*it)->handleEvents();

        doPendingFunc();
    }
}

void EventLoop::quit()
{
    if(assertInLoop())
    {
        looping_ = false;
        wakeup();
    }
}

void EventLoop::runInLoop(Functor func)
{
    if(assertInLoop())
        func();
    else
    {
        queueInLoop(std::move(func));
    }
    
}

void EventLoop::queueInLoop(Functor func)
{
    {
        MutexLockGurard lock(mutex_);
        FuncList_.push_back(func);
    }
    if(!assertInLoop())
        wakeup();
}

void EventLoop::doPendingFunc()
{
    std::vector<Functor> list;
    {
        MutexLockGurard lock(mutex_);
        list.swap(FuncList_);
    }
    if(list.size() > 0)
        LOG_LOG << "EventLoop::doPendingFunc list size is " << list.size();
    for(const auto& i : list)
    {
        i();
    }
}

bool EventLoop::assertInLoop()
{
    return threadid_ == getThreadId();
}

void EventLoop::assertInLoopThread()
{
    if(!assertInLoop())
        LOG_ERROR << "EventLoop::assertInLoopThread not in loop ";

}

void EventLoop::updateChannel(const ChannelPtr& channel)
{
    EpollPtr_->modEvent(channel);
}

void EventLoop::removeChannel(const ChannelPtr& channel)
{
    EpollPtr_->delEvent(channel);
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = ::write(wakeUpFd_, &one, sizeof one);
    if(n != sizeof one)
    {
        LOG_ERROR << "EventLoop::wakeUp() send " << n << " bytes instead of 8";
    }
}


void EventLoop::handleRead()
{
  uint64_t one = 1;
  ssize_t n = ::read(wakeUpFd_, &one, sizeof one);
  if (n != sizeof one)
  {
    LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
  }
}

void EventLoop::runAt(int sec, TimerCallback cb)
{
    timerQueuePtr_->addTimer(sec, std::move(cb), false);
}

void EventLoop::runEveryN(int sec, TimerCallback cb)
{
    timerQueuePtr_->addTimer(sec, std::move(cb), true);
}

