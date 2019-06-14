#ifndef FILESERV_HTTPSERVER_H
#define FILESERV_HTTPSERVER_H

#include <map>
#include <memory>
#include <atomic>

#include "../TcpConnection.h"

class HttpSession;
class EventLoop;
class Buffer;
class TcpServer;

class HttpServer
{
public: 
    HttpServer(const std::string ip, const std::string port, EventLoop*, int threadNumber = 0);
    ~HttpServer();

    HttpServer(const HttpServer&) = delete;
    HttpServer& operator=(const HttpServer&) = delete;

    void init();
    void enableGzip(bool on);

    void onConnect(const TcpConnectionPtr&);
   // void onMessage(TcpConnectionPtr&, Buffer*);

private: 
    EventLoop* loop_;
    std::unique_ptr<TcpServer> tcpServer_;
    std::atomic_int sessionId_;
    std::string ip_;
    std::string port_;
    bool Gzip_;
    int threadNumber_;
    std::map<int, std::shared_ptr<HttpSession>> sessionLists_;
};

#endif // !FILESERV_HTTPSERVER_H