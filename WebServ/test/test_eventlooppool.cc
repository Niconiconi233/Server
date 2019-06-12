#include "../EventLoopThreadPool.h"
#include "../EventLoop.h"
#include <iostream>

int main()
{
    EventLoop loop;
    EventLoopThreadPool pool(&loop);
    pool.setThreadNumber(2);
    pool.start();
    loop.loop();
    return 0;
}
