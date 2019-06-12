#include "EventLoop.h"
#include "Channel.h"

#include <iostream>
#include <functional>
#include <unistd.h>

void onread()
{
    std::cout<<"on read"<<std::endl;
    char buf[64];
    ::read(0, buf, sizeof buf);
}

void onclose()
{
    std::cout<<"onclose"<<std::endl;
}


int main()
{
    EventLoop loop;
    ChannelPtr ptr(new Channel(&loop, 0));
    ptr->enableRead();
    ptr->setReadCallback(std::bind(&onread));
    ptr->setCloseCallback(std::bind(&onclose));
    loop.addChannel(ptr);
    loop.loop();
    
}
