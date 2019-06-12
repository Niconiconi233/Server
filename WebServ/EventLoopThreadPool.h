#ifndef FILESERV_EVENTLOOPTHREADPOOL_H
#define FILESERV_EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool(EventLoop* loop);
    ~EventLoopThreadPool();

    EventLoopThreadPool(const EventLoopThreadPool&) = delete;
    EventLoopThreadPool& operator=(const EventLoopThreadPool&) = delete;

    void setThreadNumber(int number)
    {
        numThread_ = number;
    }
    void start();

    EventLoop* getNextLoop();

private: 
    std::vector<EventLoop*> loopLists_;
    std::vector<std::unique_ptr<EventLoopThread>> threadLists_;
    EventLoop* baseLoop_;
    bool started_;
    int numThread_;
    size_t nextLoop_;
};

#endif // !FILESERV_EVENTLOOPTHREADPOOL_H