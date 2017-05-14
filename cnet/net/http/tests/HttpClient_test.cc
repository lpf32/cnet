#include <cnet/net/http/HttpServer.h>
#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/http/HttpResponse.h>
#include <cnet/net/EventLoop.h>
#include <cnet/base/Logging.h>

#include <iostream>
#include <map>
#include <cnet/net/TcpClient.h>

using namespace cnet;
using namespace cnet::net;


void onConnection(const TcpConnectionPtr& conn)
{
    Buffer buf;
    buf.append("GET / HTTP/1.1\r\n");
    buf.append("Host: www.example.com\r\n\r\n");
    conn->send(&buf);
}

void onMessage(const TcpConnectionPtr& conn, Buffer* buf, Timestamp time)
{
    printf("%s\n", buf->peek());
}

int main()
{
    EventLoop loop;
    TcpClient client(&loop, InetAddress("127.0.0.1", 8000), "Http client");
    client.setConnectionCallback(onConnection);
    client.setMessageCallback(onMessage);
    client.connect();
    loop.loop();
}
