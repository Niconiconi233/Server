#include "TcpConnection.h"

#include "Logging/base/Logging.h"
#include "Utils.h"
#include "Socket.h"
#include "EventLoop.h"

TcpConnection::TcpConnection(EventLoop* loop, int fd, const InetAddr& localaddr, const InetAddr& peeraddr)
    :loop_(loop),
    socketPtr_(new Socket(fd)),
    channelPtr_(new Channel(loop, fd)),
    localAddr_(localaddr),
    peerAddr_(peeraddr),
    state_(kdisconnecting)
    //highWaterMark_(64* 1024 * 1024)
{
    channelPtr_->setReadCallback(std::bind(&TcpConnection::handleRead, this));
    channelPtr_->setCloseCallback(std::bind(&TcpConnection::handleClose, this)),
    channelPtr_->setErrorCallback(std::bind(&TcpConnection::handleError, this));
    channelPtr_->setWriteCallback(std::bind(&TcpConnection::handleWrite, this));
    socketPtr_->setTcpKeepalive(true);
    socketPtr_->noTcpDelay(true);
    LOG_TRACE << "TcpConnection created fd = " << fd << " peeraddr = " << peerAddr_.toIpPort() << "in loop " << loop;
}

TcpConnection::~TcpConnection()
{
    assert(state_ == kdisconnected);
    LOG_DEBUG <<"~TcpConnection "<<channelPtr_.use_count();
}

void TcpConnection::shutdown()
{
    setState(kdisconnecting);
    loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
}

void TcpConnection::shutdownInLoop()
{
    if(!channelPtr_->isWriting()){
        socketPtr_->shutdown();
        setState(kdisconnected);
    }
}

void TcpConnection::Send(const std::string& str)
{
  SendInLoop(str.c_str(), str.length());
}

void TcpConnection::Send(const void* data, int len)
{
    SendInLoop(data, len);
}

void TcpConnection::Send(Buffer& buffer)
{
    SendInLoop(buffer.peek(), buffer.readableBytes());
    buffer.retrieveAll();
}

void TcpConnection::SendInLoop(const void* data, size_t len)
{
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  size_t remaining = len;
  bool faultError = false;
  if (state_ == kdisconnected)
  {
    LOG_WARN << "disconnected, give up writing";
    return;
  }
  // if no thing in output queue, try writing directly
  if (!channelPtr_->isWriting() && outputBuffer_.readableBytes() == 0)
  {
    nwrote = net::Send(channelPtr_->getFd(), data, len);
    LOG_TRACE <<"TcpConnection::sendInLoop send " << nwrote << " bytes data to fd " << channelPtr_->getFd();
    if (nwrote >= 0)
    {
      remaining = len - nwrote;
      if (remaining == 0 && writeCompleteCallback_)
      {
        loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));
      }
    }
    else // nwrote < 0
    {
      nwrote = 0;
      if (errno != EWOULDBLOCK)
      {
        LOG_ERROR << "TcpConnection::sendInLoop";
        if (errno == EPIPE || errno == ECONNRESET) 
        {
          faultError = true;
        }
      }
    }
  }

  assert(remaining <= len);
  if (!faultError && remaining > 0)
  {
    /*size_t oldLen = outputBuffer_.readableBytes();
    if (oldLen + remaining >= highWaterMark_
        && oldLen < highWaterMark_
        && highWaterMarkCallback_)
    {
      loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
    }*/
    outputBuffer_.append(static_cast<const char*>(data)+nwrote, remaining);
    if (!channelPtr_->isWriting())
    {
      channelPtr_->enableWrite();
    }
  }
}

// called when TcpServer accepts a new connection
void TcpConnection::connectEstablished()
{
    loop_->assertInLoop();
    setState(kconnected);
    channelPtr_->enableRead();
    {
        connectionCallback_(shared_from_this());
    }

    LOG_LOG << "Tcpconnection::connectEstablished fd = " << getFd();
}
  // called when TcpServer has removed me from its map
void TcpConnection::connectDestroyed()
  {
      {
      loop_->assertInLoop();
      if(state_ == kconnected)
      {
          setState(kdisconnected);
          channelPtr_->disableAll();

          connectionCallback_(shared_from_this());//通知作用
      }
      channelPtr_->remove();
      LOG_TRACE << "Tcpconnection::connectDestroyed fd = " << getFd();
      }
  }


void TcpConnection::handleRead()
{
    loop_->assertInLoop();
    assert(state_ == kconnected);
    int saved_errno = 0;
    size_t nread = inputBuffer_.readFd(channelPtr_->getFd(), &saved_errno);
    if(nread > 0)
    {
        messageCallback_(shared_from_this(), &inputBuffer_);
    }else if(nread == 0)
        closeCallback_(shared_from_this());
    else
    {
        LOG_ERROR << "TcpConnection::handleRead";
        handleError();
    }
}

void TcpConnection::handleClose()
{
    loop_->assertInLoop();
    assert(state_ == kconnected || state_ == kdisconnecting);
    channelPtr_->disableAll();
    TcpConnectionPtr ptr(shared_from_this());//use_count = 2
    closeCallback_(ptr);
    LOG_DEBUG << "TcpConnection::handleClose sockfd = " << channelPtr_->getFd();
}

void TcpConnection::handleWrite()
{
    loop_->assertInLoop();
    if(channelPtr_->isWriting())
    {
        int nsend = net::Send(channelPtr_->getFd(), outputBuffer_.peek(), outputBuffer_.readableBytes());
        if(nsend > 0)
        {
            outputBuffer_.retrieve(nsend);
            if(outputBuffer_.readableBytes() == 0)
                if(writeCompleteCallback_)
                    writeCompleteCallback_(shared_from_this());
            if(state_ == kdisconnecting)
                shutdownInLoop();
        }else
        {
            LOG_ERROR << "TcpConnection::handleWrite " << strerror(errno);
        }
    }else
    {
        LOG_ERROR << "Connection fd = " << channelPtr_->getFd()
              << " is down, no more writing";
    }
}


void TcpConnection::handleError()
{
    int ret = socketPtr_->getSockError();
    LOG_ERROR << "TcpConnection::handleError errno = " << ret;
}


