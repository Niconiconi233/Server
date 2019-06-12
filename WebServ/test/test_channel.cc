#include "Channel.h"

#include <iostream>
#include <functional>
#include <vector>
#include <memory>
#include <unistd.h>
#include "Epoller.h"
#include "Socket.h"

using namespace std;

void onread(int fd)
{
    cout<<"read"<<endl;
    char buf[64] = {0};
    size_t n = ::read(fd, buf, sizeof buf);
    cout<<buf;
}

void onwrite(int fd)
{
    cout<<"write"<<endl;
}

void onclose(int fd)
{
    cout<<"close"<<endl;
}

void onerr(int fd)
{
    cout<<"err"<<endl;
}

int main()
{
    Epoller poll;
    Socket socket(&poll);
    socket.createSocketOrDie();
    socket.bind("127.0.0.1", "10086");
    socket.listen();
    ChannelPtr ptr(new Channel(socket.getSockFd()));
    ptr->enableRead();
    ptr->setReadCallback(std::bind(&Socket::accept, &socket, 1));
    poll.addEvent(ptr);
    while(true)
    {
        std::vector<ChannelPtr> ve = poll.poll();
        for(auto it = ve.begin(); it != ve.end(); ++it)
            (*it)->handleEvents();
    }
    return 0;

}