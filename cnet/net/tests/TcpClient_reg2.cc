#include <cnet/base/Logging.h>
#include <cnet/base/Thread.h>
#include <cnet/net/EventLoop.h>
#include <cnet/net/TcpClient.h>

#include <boost/bind.hpp>

using namespace cnet;
using namespace cnet::net;

void threadFunc(EventLoop* loop)
{
    InetAddress serverAddr("127.0.0.1", 22);
    TcpClient client(loop, serverAddr, "TcpClient");
    client.connect();

    CurrentThread::sleepUsec(1000*1000);

    // client destructs when connected.
}

int main()
{
    Logger::setLogLevel(Logger::DEBUG);

    EventLoop loop;
    loop.runAfter(100.0, boost::bind(&EventLoop::quit, &loop));
    Thread thr(boost::bind(threadFunc, &loop));
    thr.start();
    loop.loop();
}
