#include "Socket.h"
#include "Logging/base/Logging.h"
#include <iostream>
#include <unistd.h>

const char header[] = "HTTP/1.1 200 OK\r\nContent-Length:26289\r\nContent-Type:html/text\r\nServer:WDNMD\r\n\r\n";

int main()
{
    Socket socket;
    socket.createSocketOrDie();
    socket.setReuseAddr(1);
    socket.bind("127.0.0.1", "10086");
    socket.listen();
    socket.accept();
    socket.send(header, sizeof header);
    socket.send("1.log");
    socket.shutdown();
    sleep(3);
    return 0;
}
