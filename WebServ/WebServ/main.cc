#include "HttpServer.h"
#include "../EventLoop.h"
#include "../Logging/base/ConfigReader.h"

int main()
{
    ConfigReader reader("ServerConfig.conf");
    EventLoop loop;
    HttpServer server(reader.getAsString("ip"), reader.getAsString("port"), &loop, 2);
    server.enableGzip(reader.getAsBool("gzip"));
    server.init();
    loop.loop();
    return 0;
}