#ifndef FILESERV_TCPSERVER_H
#define FILESERV_TCPSERVER_H

#include "Logging/base/noncopyable.h"
#include "Channel.h"
#include "Callbacks.h"

#include <string>
#include <map>
#include <memory>
#include <functional>

class Acceptor;
class TcpConnection;
class EventLoopThreadPool;
class Buffer;
class InetAddr;

class TcpServer : noncopyable
{
    typedef std::map<int, TcpConnectionPtr> ConnLists;
public: 
    TcpServer(EventLoop* loop, InetAddr& servaddr);
    TcpServer(const std::string port);
    ~TcpServer();

    void start();

    void setThreadNumber(int number);

    /// Set connection callback.
    /// Not thread safe.
    void setConnectionCallback(const ConnectionCallback& cb)
     { connectionCallback_ = cb; }

    /// Set message callback.
    /// Not thread safe.
    void setMessageCallback(const MessageCallback& cb)
     { messageCallback_ = cb; }

    /// Set write complete callback.
    /// Not thread safe.
    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

private: 
    /// Not thread safe, but in loop
    void newConnection(int sockfd, const InetAddr& peerAddr);
    /// Thread safe.
    void removeConnection(const TcpConnectionPtr& conn);
    /// Not thread safe, but in loop
    void removeConnectionInLoop(const TcpConnectionPtr& conn);

    EventLoop* loop_;
    std::unique_ptr<Acceptor> acceptorPtr_;
    std::shared_ptr<EventLoopThreadPool> eventLoopThread_;
    ConnLists connLists_;

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
};

#endif // !FILESERV_TCPSERVER_H