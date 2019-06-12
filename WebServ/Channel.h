#ifndef FILESERV_CHANNEL_H
#define FILESERV_CHANNEL_H

#include "Logging/base/noncopyable.h"

#include <sys/types.h>
#include <memory>
#include <functional>
#include <sys/epoll.h>

class EventLoop;

class Channel : noncopyable, public std::enable_shared_from_this<Channel>
{
private: 
    typedef std::function<void()> CallBack;
    EventLoop* loop_;
    int fd_;
    __uint32_t events_;
    __uint32_t revents_;

    CallBack readcallback_;
    CallBack writecallback_;
    CallBack errcallback_;
    CallBack closecallback_;
    
public: 
    Channel(EventLoop* loop, int fd):loop_(loop),fd_(fd),events_(0),revents_(0){}
    void remove();
    ~Channel();

    void setReadCallback(CallBack&& cb)
    {
        readcallback_ = cb;
    }

    void setWriteCallback(CallBack&& cb)
    {
        writecallback_ = cb;
    }

    void setErrorCallback(CallBack&& cb)
    {
        errcallback_ = cb;
    }

    void setCloseCallback(CallBack&& cb)
    {
        closecallback_ = cb;
    }

    void handleEvents();

    int getFd()
    {
        return fd_;
    }

    void enableRead()
    {
        events_ |= EPOLLIN;
        events_ |= EPOLLRDHUP;
        updateChannel();
    }

    void enableWrite()
    {
        events_ |= EPOLLOUT;
        events_ |= EPOLLRDHUP;
        updateChannel();
    }

    void disableRead()
    {
        events_ &= ~EPOLLIN;
        updateChannel();
    }

    void disableWrite()
    {
        events_ &= ~EPOLLOUT;
        updateChannel();
    }

    void disableAll()
    {
        events_ = 0;
        updateChannel();
    }

    void updateChannel();

    __uint32_t getEvents()
    {
        return events_;
    }

    void setRevents(__uint32_t event)
    {
        revents_ = event;
    }

    void setEvents(__uint32_t event)
    {
        events_ = event;
    }

  bool isWriting() const { return events_ & EPOLLOUT; }
  bool isReading() const { return events_ & EPOLLIN; }
};

typedef std::shared_ptr<Channel> ChannelPtr;

#endif // !FILESERV_CHANNEL_H
