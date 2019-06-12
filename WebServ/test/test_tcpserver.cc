#include "TcpServer.h"
#include "InetAddr.h"
#include "EventLoop.h"
#include "TcpConnection.h"
#include <unistd.h>
/*
    实现了回射服务器
 */

void newConncb(const TcpConnectionPtr& ptr)
{
    char buf[] = "welcome to my server~\n";
    ptr->Send(buf, sizeof buf);
}

void newMesscb(const TcpConnectionPtr& ptr, Buffer* buff)
{
    ptr->Send(buff->peek(), buff->readableBytes());
    buff->retrieveAll();
}

int main()
{
    EventLoop loop;
    InetAddr addr("10086");
    TcpServer server(&loop, addr);
    server.setConnectionCallback(std::bind(newConncb, std::placeholders::_1));
    server.setMessageCallback(std::bind(&newMesscb, std::placeholders::_1, std::placeholders::_2));
    server.start();
    loop.loop();
    return 0;
}