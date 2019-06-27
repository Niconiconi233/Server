#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddr.h"
#include "Utils.h"

#include "Logging/base/Logging.h"

Acceptor::Acceptor(EventLoop* loop, InetAddr& addr)
    :loop_(loop),
    acceptSocket_(net::createSocketOrDie()),
    acceptChannel_(new Channel(loop, acceptSocket_.getSockFd()))
{
    //acceptSocket_.setReuseAddr(1);
    acceptSocket_.setReuseport(1);
    acceptSocket_.bind(addr);
    acceptChannel_->setReadCallback(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
    acceptChannel_->disableAll();
    acceptChannel_->remove();
    acceptSocket_.close();
}

void Acceptor::listen()
{
    loop_->assertInLoop();
    acceptSocket_.listen();
    acceptChannel_->enableRead();
}

void Acceptor::handleRead()
{
    struct sockaddr_in addr;
    int connfd = acceptSocket_.accept(&addr);
    if(connfd > 0)
    {
        if(net::SelfConnection(connfd))//判断自连接
        {
            net::myclose(connfd);
            return;
        }
        InetAddr peerAddr(addr);
        if(newConnCallback_)
        {
            newConnCallback_(connfd, peerAddr);
        }
    }else
    {
        LOG_ERROR << "Acceptor::handleRead error listenfd = " << connfd;
        ::close(connfd);
    }
}