#ifndef FILESERV_EPOLLER_H
#define FILESERV_EPOLLER_H

#include "Logging/base/noncopyable.h"
#include "Channel.h"
#include <map>
#include <vector>
#include <memory>

struct epoll_event;
class Channel;

class Epoller : noncopyable{
const int initSize = 64;
const int EPOLLWAIT_TIME = 500;
public: 
    Epoller();
    ~Epoller();

    void modEvent(const ChannelPtr& channel);
    void delEvent(const ChannelPtr& channel);

    std::vector<ChannelPtr> poll();

private: 
    typedef std::vector<epoll_event> EventLists;
    typedef std::map<int, ChannelPtr> ItemsLists;

    std::vector<ChannelPtr> fileVector(EventLists& lists, int len);
    EventLists eventlists_;
    ItemsLists itemlists_;

    int epollid_;
};



#endif // !FILESERV_EPOLLER_H