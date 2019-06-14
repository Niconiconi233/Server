#include "TimerQueue.h"

#include <sys/time.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include "EventLoop.h"
#include "Channel.h"
#include "Logging/base/Logging.h"

namespace
{
    int createTimerFd()
    {
        int fd = ::timerfd_create(CLOCK_REALTIME, TFD_CLOEXEC | TFD_NONBLOCK);
        if(fd < 0)
            LOG_ERROR << "TimerQueue::createTimerFd failed ";
        return fd;
    }

    void readTimerFd(int timerfd)
    {
        uint64_t howmany;
        ssize_t n = ::read(timerfd, &howmany, sizeof howmany);
        LOG_TRACE << "TimerQueue::handleRead " << howmany;
        if(n != sizeof howmany)
            LOG_ERROR << "TimerQueue::handleRead reads " << n << " bytes instead of 8";
    }

    struct timespec timerNow()
    {
        //struct timeval tv;
        //gettimeofday(&tv, 0);
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        return spec;
    }

    void resetTimerFd(int timerfd, itimerspec newValue)
    {
        struct itimerspec oldValue;
        memset(&oldValue, 0, sizeof oldValue);
        struct timespec now = timerNow();
        if(now.tv_sec > newValue.it_value.tv_sec)//重置时间
        {
            newValue.it_value.tv_sec = now.tv_sec + 2;
        }
        LOG_DEBUG << newValue.it_value.tv_sec << " now is " << now.tv_sec;
        int ret = ::timerfd_settime(timerfd, TFD_TIMER_ABSTIME, &newValue, &oldValue);
        if(ret)
        {
            LOG_ERROR << "timerfd_settime";
        }
    }
}

TimerItem::TimerItem(int sec, TimerCallback callback, bool lp)
    :loop(lp),
    cb(std::move(callback)),
    sec(sec)
{
    memset(&ispec, 0, sizeof ispec);
    struct timespec now = timerNow();
    ispec.it_value.tv_sec = now.tv_sec + sec;
    ispec.it_interval.tv_nsec = now.tv_nsec;
}

bool timerCmp(const TimerItem& lhs, const TimerItem& rhs)
{
    return lhs.ispec.it_value.tv_sec > rhs.ispec.it_value.tv_sec || lhs.ispec.it_value.tv_nsec > rhs.ispec.it_value.tv_nsec;
}

TimerQueue::TimerQueue(EventLoop* loop)
    :loop_(loop),
    timerfd_(createTimerFd()),
    channelPtr_(new Channel(loop, timerfd_)),
    heap_(std::bind(&timerCmp, std::placeholders::_1, std::placeholders::_2))
{
    channelPtr_->setReadCallback(std::bind(&TimerQueue::handleRead, this));
    channelPtr_->enableRead();
}

TimerQueue::~TimerQueue()
{
    channelPtr_->disableAll();
    channelPtr_->remove();
    ::close(timerfd_);
}

void TimerQueue::addTimer(int sec, TimerCallback cb, bool lp)
{
    TimerItem item(sec, std::move(cb), lp);
    loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, item));
}

void TimerQueue::addTimerInLoop(TimerItem& item)
{
    heap_.insert(item);
    if(now_.sec == 0)
    {
        now_ = heap_.getTop();
        resetTimerFd(timerfd_, now_.ispec);
    }
    else
    {
        TimerItem& tmp = heap_.getTop();
        if(!(now_ == tmp))
        {
            now_ = tmp;
            resetTimerFd(timerfd_, now_.ispec);
        }
    }
}

void TimerQueue::handleRead()
{
    loop_->assertInLoopThread();
    readTimerFd(timerfd_);
    struct timespec now = timerNow();
    while(heap_.getTop().ispec.it_value.tv_sec <= now.tv_sec + 1 && !heap_.isEmpty())
    {
        LOG_DEBUG << heap_.getTop().ispec.it_value.tv_sec;
        cancelTimer_.emplace_back(heap_.getMin());
    }
    if(cancelTimer_.size() > 0)
    {
        for(const auto& i : cancelTimer_)
            i.cb();
        loop_->runInLoop(std::bind(&TimerQueue::readdTimerInLoop, this));
    }
}

void TimerQueue::readdTimerInLoop()
{
    for(auto it = cancelTimer_.begin(); it != cancelTimer_.end(); ++it)
    {
        if(it->loop)
        {
            addTimer(it->sec, it->cb, it->loop);
        }
    }
    if(!heap_.isEmpty())//如果没有添加新的时间，刷新时间
    {
        now_ = heap_.getTop();
        resetTimerFd(timerfd_, now_.ispec);
    }
    cancelTimer_.clear();
}