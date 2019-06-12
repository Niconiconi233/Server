#ifndef FILESERV_UTILS_H
#define FILESERV_UTILS_H

#include "Logging/base/Logging.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <cassert>
#include <cerrno>

class Buffer;

namespace net{

template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);


void setNonBlock(int fd);

void memZero(void* data, size_t len);

int getSocketError(int sockfd);

struct sockaddr_in getLocalAddr(int sockfd);

struct sockaddr_in getPeerAddr(int sockfd);

int createSocketOrDie();

void bind(int sockfd, const struct sockaddr_in* addr);

void listenOrDie(int sockfd);

int acceptOrDie(int sockfd, struct sockaddr_in* addr);

void shutdown(int sockfd);

void myclose(int sockfd);

size_t Send(int sockfd, const Buffer& buffer, size_t len);

size_t Send(int sockfd, const void* data, size_t len);

size_t Recv(int sockfd, Buffer& buffer, size_t len);

size_t Recv(int sockfd, void* buffer, size_t len);

}//namespace net

#endif // !FILESERV_UTILS_H