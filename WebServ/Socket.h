#ifndef FILESERV_SOCKET_H
#define FILESERV_SOCKET_H

#include <string>


class Buffer;
class InetAddr;

class Socket{
public: 
    Socket(int fd):sockfd_(fd),state_(kdislistened){}
    Socket():sockfd_(0),state_(kdislistened){}
    ~Socket();
    void createSocket();
    void bind(const std::string address, const std::string port);
    void bind(const InetAddr& addr);
    void listen();
    int accept(struct sockaddr_in* addr);
    int getSockFd(){return sockfd_;}

    void setReuseAddr(bool on);
    void setReuseport(bool on);
    void noTcpDelay(bool on);
    void setTcpKeepalive(bool on);

    //size_t Send(const Buffer& buffer, size_t len);
    //size_t Send(const void* data, size_t len);

    size_t Recv(Buffer& buffer, size_t len);
    size_t Recv(void* buffer, size_t len);

    int getSockError();

    void shutdown();
    void close();

private: 
    int sockfd_;
    enum state{klistening, kdislistened, kdislistening, };
    state state_;
    void setState(state st)
    {
        state_ = st;
    }
};

#endif // !FILESERV_SOCKET_H