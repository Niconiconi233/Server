#include "Epoller.h"

#include "Logging/base/Logging.h"

#include <sys/epoll.h>
#include <cassert>
#include <errno.h>


Epoller::Epoller():eventlists_(initSize)
{
    epollid_ = epoll_create1(EPOLL_CLOEXEC);
    if(epollid_ <= 0)
    {
        char buf[32] = {0};
        LOG_FATAL << "Epoller create epollid failed " << strerror_r(errno, buf, 32);
    }
}

void Epoller::modEvent(const ChannelPtr& channel)
{
    assert(channel != nullptr);
    int fd = channel->getFd();
    assert(fd >= 0);
    if(itemlists_.find(fd) == itemlists_.end())//add new event
    {
         struct epoll_event event;
        event.data.fd = fd;
        event.events = channel->getEvents();
        itemlists_[fd] = channel;
        if(epoll_ctl(epollid_, EPOLL_CTL_ADD, fd, &event) < 0)
        {
            LOG_ERROR << "Epoller::addevent failed in channel sockfd = " << fd << " " << strerror(errno);
            return;
        }
        LOG_TRACE << "Epoller::addEvent fd = " << fd <<  " event is " << event.events;
    }
    else{
        struct epoll_event event;
        event.data.fd = channel->getFd();
        event.events = channel->getEvents();
        if(epoll_ctl(epollid_, EPOLL_CTL_MOD, fd, &event) < 0)
        {
            LOG_ERROR << "Epoller::modevent failed in channel " << channel.get() << " event is " << event.events;
        }
        LOG_TRACE << "Epoller::modevent fd = " << fd << " new event is " << event.events;
    }
}

void Epoller::delEvent(const ChannelPtr& channel)
{
    assert(channel != nullptr);
    int fd = channel->getFd();
    assert(fd >= 0);
    assert(itemlists_.find(fd) != itemlists_.end());
    struct epoll_event event;
    event.data.fd = channel->getFd();
    event.events = channel->getEvents();
    if(epoll_ctl(epollid_, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        LOG_ERROR << "Epoller::delevent failed in channel " << channel.get() << " event is " << event.events;
    }
    itemlists_.erase(fd);//智能指针-1
    LOG_TRACE << "Epoller::delevent fd = " << channel->getFd();

}

std::vector<ChannelPtr> Epoller::poll()
{
    int n = epoll_wait(epollid_, eventlists_.data(), eventlists_.size(), EPOLLWAIT_TIME);
    if(n < 0)
    {
            LOG_ERROR << "Epoller::poll failed";
    }
    return fileVector(eventlists_, n);
}

std::vector<ChannelPtr> Epoller::fileVector(EventLists& list, int len)
{
    std::vector<ChannelPtr> activeLists;
    for(int i = 0; i < len; ++i)
    {
        ChannelPtr& ptr = itemlists_[list[i].data.fd];
        ptr->setRevents(list[i].events);
        activeLists.push_back(ptr);
    }
    return activeLists;
}

Epoller::~Epoller()
{
    for(auto it = itemlists_.begin(); it != itemlists_.end(); ++it)
        it->second.reset();
}
