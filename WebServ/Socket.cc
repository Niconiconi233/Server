#include "Socket.h"
#include "Logging/base/Logging.h"
#include "Utils.h"
#include "InetAddr.h"

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
    return listenfd;
}

void Socket::setReuseAddr(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
}

void Socket::setReuseport(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof opt);
}

void Socket::noTcpDelay(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof opt);
}

void Socket::setTcpKeepalive(bool on)
{
    int opt = on ? 1 : 0;
    ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof opt);
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

