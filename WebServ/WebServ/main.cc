#include "HttpServer.h"
#include "../EventLoop.h"
#include "../Logging/base/ConfigReader.h"
#include "../Logging/base/SingLeton.h"

int main()
{
    //ConfigReader reader("ServerConfig.conf");
    auto& reader = SingLeton<ConfigReader>::instance();
    EventLoop loop;
    HttpServer server(reader->getAsString("ip"), reader->getAsString("port"), &loop, 0);
    server.enableGzip(reader->getAsBool("gzip"));
    server.init();
    loop.loop();
    return 0;
}
