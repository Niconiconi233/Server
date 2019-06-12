#ifndef FILESERV_ACCEPTOR_H
#define FILESERV_ACCEPTOR_H

#include "Channel.h"
#include "Socket.h"

#include <functional>

class EventLoop;
class InetAddr;

class Acceptor : noncopyable
{
    typedef std::function<void (int, const InetAddr&)> newConnCallback;
public: 
    Acceptor(EventLoop* loop, InetAddr& addr);
    ~Acceptor();

    void setNewConnCallback(const newConnCallback&& cb)
    {
        newConnCallback_ = cb;
    }

    void listen();


private: 
    EventLoop* loop_;
    Socket acceptSocket_;
    ChannelPtr acceptChannel_;
    void handleRead();
    newConnCallback newConnCallback_;

};

#endif // !FILESERV_ACCEPTOR_H