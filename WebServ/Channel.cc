#include "Channel.h"
#include "Epoller.h"
#include "EventLoop.h"
#include "Logging/base/Logging.h"

#include <sys/epoll.h>

Channel::~Channel()
{
    LOG_LOG << "~Channel fd = " << fd_;
}

void Channel::handleEvents()
{
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
    {
        closecallback_();
    }
    if (revents_ & EPOLLERR)
    {
        if (errcallback_) errcallback_();
        return;
    }
    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
    {
        readcallback_();
    }
    if (revents_ & EPOLLOUT)
    {
        writecallback_();
    }
}

void Channel::updateChannel()
{
    loop_->updateChannel(shared_from_this());//2
}

void Channel::remove()
{
    loop_->removeChannel(shared_from_this());
}