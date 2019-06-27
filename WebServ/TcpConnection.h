#ifndef FILESERV_TCPCONNECTION_H
#define FILESERV_TCPCONNECTION_H

#include "Logging/base/noncopyable.h"
#include "Buffer.h"
#include "InetAddr.h"
#include "Channel.h"
#include "Callbacks.h"

#include <memory>
#include <functional>

class EventLoop;
class Socket;

class TcpConnection : noncopyable, public std::enable_shared_from_this<TcpConnection>
{
public: 
    TcpConnection(EventLoop* loop, int fd, const InetAddr& localaddr, const InetAddr& peeraddr);
    ~TcpConnection();
    const InetAddr& localAddress(){return localAddr_;}
    const InetAddr& peerAddress(){return peerAddr_;}
    EventLoop* getLoop(){return loop_;}
    void shutdown();
    void Send(const void* data, int len);
    void Send(const std::string& str);
    void Send(Buffer& buffer);
    int getFd()
    {
        return channelPtr_->getFd();
    }

    Buffer& getInputBuffer()
    {
        return inputBuffer_;
    }

    Buffer& getOutputBuffer()
    {
        return outputBuffer_;
    }

  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback& cb)
  { writeCompleteCallback_ = cb; }

 /*void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
  { highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark; }*/

  void setCloseCallback(const CloseCallback& cb)
  {closeCallback_ = cb;}

    // called when TcpServer accepts a new connection
  void connectEstablished();   // should be called only once
  // called when TcpServer has removed me from its map
  void connectDestroyed();  // should be called only once

  bool connected() const {return state_ == kconnected;}

private: 
    enum state {kconnecting, kconnected, kdisconnecting, kdisconnected}; 

    EventLoop* loop_;
    std::unique_ptr<Socket> socketPtr_;
    std::shared_ptr<Channel> channelPtr_;
    
    const InetAddr localAddr_;
    const InetAddr peerAddr_;
    state state_;
    
    //size_t highWaterMark_;

    void setState(state st)
    {
        state_ = st;
    }
    void shutdownInLoop();
    void SendInLoop(const void* data, size_t len);
    void handleRead();
    void handleClose();
    void handleWrite();
    void handleError();

    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    //HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;

    Buffer inputBuffer_;
    Buffer outputBuffer_;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

#endif // !FILESERV_TCPCONNECTION_H