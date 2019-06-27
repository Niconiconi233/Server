#include "Socket.h"

#include "Utils.h"
#include "InetAddr.h"
#include "Logging/base/Logging.h"
#include "Logging/base/SingLeton.h"
#include "Logging/base/ConfigReader.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>

#include <cassert>
#include <cerrno>
#include <cstring>
#include <cstdlib>

Socket::~Socket()
{
    net::myclose(sockfd_);
    assert(state_ == kdislistened);
}

void Socket::createSocket()
{
    assert(sockfd_ == 0);
    sockfd_ = net::createSocketOrDie();
}

void Socket::close()
{
    assert(sockfd_ != 0);
    net::myclose(sockfd_);
}

void Socket::shutdown()
{
    assert(sockfd_ != 0);
    net::shutdown(sockfd_);
}

void Socket::bind(const std::string address, const std::string port)
{
    assert(sockfd_ != 0);
    InetAddr addr(address, port);
    net::bind(sockfd_, addr.getAddr());
}

void Socket::bind(const InetAddr& address)
{
    assert(sockfd_ != 0);
    net::bind(sockfd_, address.getAddr());
}
void Socket::listen()
{
    assert(sockfd_ != 0);
    net::listenOrDie(sockfd_);
}

int Socket::accept(struct sockaddr_in* addr)
{
    int listenfd = net::acceptOrDie(sockfd_, addr);
    if(listenfd < 0)
    {
        LOG_ERROR << "accept error errno = " << errno;
    }
    return listenfd;
}

void Socket::setReuseAddr(bool on)
{
    int opt = on ? 1 : 0;
    if(::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt) == -1)
    {
        char buf[32] = {0};
        LOG_ERROR << "Socket::setReuseAddr failed errno is " << strerror_r(errno, buf, 32);
    }
}

void Socket::setReuseport(bool on)
{
    int opt = on ? 1 : 0;
    if(::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof opt) == -1)
    {
        char buf[32] = {0};
        LOG_ERROR << "Socket::setReusePort failed errno is " << strerror_r(errno, buf, 32);
    }
}

void Socket::noTcpDelay(bool on)
{
    int opt = on ? 1 : 0;
    if(::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof opt) == -1)
    {
        char buf[32] = {0};
        LOG_ERROR << "Socket::noTcpDelay failed errno is " << strerror_r(errno, buf, 32);
    }
}

void Socket::setTcpKeepalive(bool on)
{
    int opt = on ? 1 : 0;
    if(::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof opt) == -1)
    {
        char buf[32] = {0};
        LOG_ERROR << "Socket::setTcpKeepAlive failed errno is " << strerror_r(errno, buf, 32);
    }
    auto& ptr = SingLeton<ConfigReader>::instance();
    int interval = ptr->getAsInt("keepdile");
    if(::setsockopt(sockfd_, IPPROTO_TCP, TCP_KEEPIDLE, &interval, sizeof interval) == -1)
    {
        char buf[32] = {0};
        LOG_ERROR << "Socket::setTcpKeepAlive Tcp_keepIdle failed " << strerror_r(errno, buf, 32);
    }
    interval = ptr->getAsInt("keepintvl");
    if(::setsockopt(sockfd_, IPPROTO_TCP, TCP_KEEPINTVL, &interval, sizeof interval) == -1)
    {
        char buf[32] = {0};
        LOG_ERROR << "Socket::setTcpKeepAlive Tcp_keepIntvl failed " << strerror_r(errno, buf, 32);
    }
    interval = ptr->getAsInt("keepcnt");
    if(::setsockopt(sockfd_, IPPROTO_TCP, TCP_KEEPCNT, &interval, sizeof interval) == -1)
    {
        char buf[32] = {0};
        LOG_ERROR << "Socket::setTcpKeepAlive Tcp_keepcnt failed " << strerror_r(errno, buf, sizeof buf);
    }

}
/*
size_t Socket::Send(const Buffer& buffer, size_t len)
{
    size_t nsend = net::Send(sockfd_, buffer, len);
    return nsend;
}

size_t Socket::Send(const void* data, size_t len)
{
    size_t nsend = net::Send(sockfd_, data, len);
    return nsend;
}

size_t Socket::Recv(Buffer& buffer, size_t len)
{
    size_t nrecv = net::Recv(sockfd_, buffer, len);
    return nrecv;
}

size_t Socket::Recv(void* buffer, size_t len)
{
    size_t nrecv = net::Recv(sockfd_, buffer, len);
    return nrecv;
}*/

int Socket::getSockError()
{
    return net::getSocketError(sockfd_);
}

