#include "InetAddr.h"
#include "Utils.h"

#include <arpa/inet.h>
#include <stdio.h>

InetAddr::InetAddr(const std::string addr, const std::string port)
    :addr_(addr),
    port_(port)
{
    net::memZero(&address_, sizeof address_);
    address_.sin_family = AF_INET;
    address_.sin_port = htons(atoi(port.c_str()));
    inet_pton(AF_INET, addr.c_str(), &address_.sin_addr);
}

InetAddr::InetAddr(const InetAddr& addr)
{
    address_ = addr.address_;
    addr_ = addr.addr_;
    port_ = addr.port_;
}

InetAddr::InetAddr(struct sockaddr_in addr)
{
    address_ = addr;
    char buf[INET6_ADDRSTRLEN];
    inet_ntop(AF_INET, &address_.sin_addr, buf, sizeof buf);
    addr_ = buf;
    char port[6];
    snprintf(port, sizeof port, "%u", ntohs(addr.sin_port));
    port_ = port;
}


InetAddr::InetAddr(const std::string port)
{
    addr_ = "0.0.0.0";
    port_ = port;
    net::memZero(&address_, sizeof address_);
    address_.sin_family = AF_INET;
    address_.sin_port = htons(atoi(port.c_str()));
    address_.sin_addr.s_addr = htonl(INADDR_ANY);
}


InetAddr::InetAddr(int port)
{
    addr_ = "0.0.0.0";
    char buf[32];
    snprintf(buf, sizeof buf, "%d", port);
    port_ = buf;
    net::memZero(&address_, sizeof address_);
    address_.sin_family = AF_INET;
    address_.sin_port = htons(port);
    address_.sin_addr.s_addr = htonl(INADDR_ANY);
}

sockaddr* InetAddr::toSockaddr()
{
    return net::sockaddr_cast(&address_);
}

std::string InetAddr::toIpPort() const
{
    std::string res;
    res.append(addr_);
    res.append(" : ");
    res.append(port_);
    return res;
}

const std::string& InetAddr::toIp() const
{
    return addr_;
}

const  std::string& InetAddr::toPort() const
{
    return port_;
}