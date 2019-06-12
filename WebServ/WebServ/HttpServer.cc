#include "HttpServer.h"

#include "../Buffer.h"
#include "../TcpServer.h"
#include "../EventLoop.h"
#include "../InetAddr.h"
#include "../Logging/base/Logging.h"
#include "HttpSession.h"

#include <functional>

HttpServer::HttpServer(const std::string ip, const std::string port, EventLoop* loop, int threadNumber)
    :loop_(loop),
    sessionId_(0),
    ip_(ip),
    port_(port),
    enableGzip_(false),
    threadNumber_(threadNumber)
{

}

HttpServer::~HttpServer()
{
    tcpServer_.reset();
    for(auto& i : sessionLists_)
        i.second.reset();
}

void HttpServer::init()
{
    InetAddr servaddr(ip_, port_);
    tcpServer_.reset(new TcpServer(loop_, servaddr));
    tcpServer_->setThreadNumber(threadNumber_);
    tcpServer_->setConnectionCallback(std::bind(&HttpServer::onConnect, this, std::placeholders::_1));
    tcpServer_->start();
}

void HttpServer::onConnect(const TcpConnectionPtr& ptr)
{
    std::shared_ptr<HttpSession> session(new HttpSession(ptr, sessionId_.load()));
    if(enableGzip_)
        session->enableGzip(true);
    ++sessionId_;
    //lock
    sessionLists_[ptr->getFd()] = session;
    ptr->setMessageCallback(std::bind(&HttpSession::onMessage, session.get(), std::placeholders::_1, std::placeholders::_2));
    LOG_LOG << "new connection from " << ptr->peerAddress().toIpPort();
}
