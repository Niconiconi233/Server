#ifndef FILESERV_INETADDR_H
#define FILESERV_INETADDR_H

#include <netinet/in.h>
#include <string>

class InetAddr
{
public: 
    InetAddr(const std::string addr, const std::string port);
    InetAddr(const InetAddr& addr);
    InetAddr(const std::string port);
    InetAddr(int port);
    InetAddr(struct sockaddr_in addr);

    sockaddr* toSockaddr();
    const struct sockaddr_in* getAddr()const {return &address_;}
    struct sockaddr_in* getAddr(){return &address_;}

    std::string toIpPort() const;
    const std::string& toIp() const;
    const std::string& toPort() const;



private: 
    struct sockaddr_in address_;
    std::string addr_, port_;

};

#endif // !FILESERV_INETADDR_H
