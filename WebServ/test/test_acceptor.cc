#include "Acceptor.h"
#include "EventLoop.h"
#include "InetAddr.h"
#include <iostream>
#include <unistd.h>
#include <functional>

void newConnbcb(int fd, const InetAddr& addr)
{
    std::cout<<"fd = " << fd << "address is " << addr.toIpPort()<<std::endl;

}


int main()
{
    EventLoop loop;
    InetAddr servaddr("10086");
    Acceptor acc(&loop, servaddr);
    acc.listen();
    acc.setNewConnCallback(std::bind(&newConnbcb, std::placeholders::_1, std::placeholders::_2));
    loop.loop();
    return 0;
}