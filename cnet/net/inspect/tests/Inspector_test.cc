#include <cnet/net/inspect/Inspector.h>
#include <cnet/net/EventLoop.h>
#include <cnet/net/EventLoopThread.h>

using namespace cnet;
using namespace cnet::net;

int main()
{
    EventLoop loop;
    EventLoopThread t;
    Inspector ins(t.startLoop(), InetAddress(12345), "test");
    loop.loop();
}