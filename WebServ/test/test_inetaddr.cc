#include "InetAddr.h"
#include <iostream>

int main()
{
    InetAddr addr1(12);
    std::cout<<addr1.toIpPort()<<std::endl;
        InetAddr addr2("127.0.0.1", "123");
    std::cout<<addr2.toIpPort()<<std::endl;
        InetAddr addr3("123");
    std::cout<<addr3.toIpPort()<<std::endl;
}