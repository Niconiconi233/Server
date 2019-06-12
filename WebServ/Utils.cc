#include "Utils.h"
#include "Buffer.h"



struct sockaddr* net::sockaddr_cast(struct sockaddr_in* addr)
{
  return static_cast<struct sockaddr*>(net::implicit_cast<void*>(addr));
}


void net::setNonBlock(int fd)
{
    int val = ::fcntl(fd, F_GETFL, 0);
    ::fcntl(fd, F_SETFL, val | O_NONBLOCK);
}

void net::memZero(void* data, size_t len)
{
    memset(data, 0, len);
}

int net::getSocketError(int sockfd)
{
  int optval;
  socklen_t optlen = static_cast<socklen_t>(sizeof optval);

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0)
  {
    return errno;
  }
  else
  {
    return optval;
  }
}

void net::bind(int sockfd, const struct sockaddr_in* addr)
{
    int ret = ::bind(sockfd, (sockaddr*)addr, sizeof *addr);
    if(ret != 0)
    {
      LOG_ERROR << "bind failed error is " << strerror(errno);
      abort(); 
    }
    LOG_LOG << "bind in sockfd = " << sockfd;
}

struct sockaddr_in net::getLocalAddr(int sockfd)
{
  struct sockaddr_in localaddr;
  net::memZero(&localaddr, sizeof localaddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
  if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
  {
    LOG_ERROR << "sockets::getLocalAddr";
  }
  return localaddr;
}

struct sockaddr_in net::getPeerAddr(int sockfd)
{
  struct sockaddr_in peeraddr;
  net::memZero(&peeraddr, sizeof peeraddr);
  socklen_t addrlen = static_cast<socklen_t>(sizeof peeraddr);
  if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
  {
    LOG_ERROR << "sockets::getPeerAddr";
  }
  return peeraddr;
}

int net::createSocketOrDie()
{
    int sockfd = ::socket(AF_INET, SOCK_STREAM, 0);
    net::setNonBlock(sockfd);
    LOG_LOG << "create sockfd " << sockfd;
    assert(sockfd > 0);
    return sockfd;
}

void net::listenOrDie(int sockfd)
{
    int ret = ::listen(sockfd, SOMAXCONN);
    assert(ret == 0);
    LOG_LOG << "start listenin on " << sockfd;
}

int net::acceptOrDie(int sockfd, struct sockaddr_in* addr)
{
   socklen_t len = sizeof addr;
    int listenfd = ::accept4(sockfd, (sockaddr*)addr, &len, SOCK_CLOEXEC | SOCK_NONBLOCK);
    LOG_LOG << "new connection listenfd = " << listenfd;
    assert(listenfd > 0);
    return listenfd;
}

void net::shutdown(int sockfd)
{
    ::shutdown(sockfd, SHUT_WR);
    LOG_LOG << "socket::shutdown " << sockfd;
}

void net::myclose(int sockfd)
{
  ::close(sockfd);
}

//必须在调用中修正偏移量
size_t net::Send(int sockfd, const Buffer& buffer, size_t len)
{
  size_t nsend = ::send(sockfd, buffer.peek(), buffer.writableBytes(), 0);
  return nsend;
}

size_t net::Send(int sockfd, const void* data, size_t len)
{
  size_t nsend = ::send(sockfd, data, len, 0);
  return nsend;
}
//必须在调用中修正偏移量
size_t net::Recv(int sockfd, Buffer& buffer, size_t len)
{
  size_t nrecv = ::recv(sockfd, buffer.beginWrite(), buffer.writableBytes(), 0);
  return nrecv;
}

size_t net::Recv(int sockfd, void* buffer, size_t len)
{
  size_t nrecv = ::recv(sockfd, buffer, len, 0);
  return nrecv;
}