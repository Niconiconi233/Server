#include "TcpServer.h"
#include "Acceptor.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Logging/base/Logging.h"
#include "EventLoopThreadPool.h"
#include "Utils.h"

void defaultConnectionCallback(const TcpConnectionPtr& conn)
{
  LOG_TRACE << "in defaultConnectionCallback " << conn->peerAddress().toIpPort();
}

void defaultMessageCallback(const TcpConnectionPtr& conn, Buffer* buf)
{
  LOG_TRACE << "in defaultMessageCallback " << conn->peerAddress().toIpPort()<<" total data is " << buf->readableBytes();
  ::write(0, buf->peek(), buf->readableBytes());
}

TcpServer::TcpServer(EventLoop* loop, InetAddr& servaddr)
    :loop_(loop),
    acceptorPtr_(new Acceptor(loop, servaddr)),
    eventLoopThread_(new EventLoopThreadPool(loop_)),
    connectionCallback_(defaultConnectionCallback),
    messageCallback_(defaultMessageCallback)
{
    acceptorPtr_->setNewConnCallback(std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));
}

TcpServer::~TcpServer()
{
  for(auto& i : connLists_)
  {
    TcpConnectionPtr ptr(i.second);
    i.second.reset();
    ptr->getLoop()->runInLoop(std::bind(&TcpConnection::connectDestroyed, ptr));
  }
  eventLoopThread_.reset();
}

void TcpServer::start()
{
  eventLoopThread_->start();
  loop_->runInLoop(std::bind(&Acceptor::listen, acceptorPtr_.get()));
}

void TcpServer::setThreadNumber(int number)
{
  eventLoopThread_->setThreadNumber(number);
}

void TcpServer::newConnection(int sockfd, const InetAddr& peeraddr)
{
  loop_->assertInLoop();
  LOG_TRACE << "new connection comes from " << peeraddr.toIpPort();
  InetAddr localaddr(net::getLocalAddr(sockfd));
  EventLoop* loop = eventLoopThread_->getNextLoop();
  TcpConnectionPtr ptr(new TcpConnection(loop, sockfd, localaddr, peeraddr));
  connLists_[sockfd] = ptr;//first use shared_ptr count is 1
  ptr->setConnectionCallback(connectionCallback_);
  ptr->setMessageCallback(messageCallback_);
  ptr->setWriteCompleteCallback(writeCompleteCallback_);
  ptr->setCloseCallback(std::bind(&TcpServer::removeConnection, this, std::placeholders::_1));
  loop_->runInLoop(std::bind(&TcpConnection::connectEstablished, ptr));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
  // FIXME: unsafe
  loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, std::move(conn)));

}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
  loop_->assertInLoop();
  LOG_DEBUG << "TcpServer::removeConnectionInLoop fd = " << conn->getFd();
  size_t n = connLists_.erase(conn->getFd());//use_cout -1
  (void)n;
  assert(n == 1);
  EventLoop* ioLoop = conn->getLoop();
  ioLoop->queueInLoop(
      std::bind(&TcpConnection::connectDestroyed, conn));
}






