#include <cnet/base/Logging.h>
#include <cnet/net/EventLoopThread.h>
#include <cnet/net/TcpClient.h>

#include <boost/bind.hpp>

using namespace cnet;
using namespace cnet::net;

int main()
{
    Logger::setLogLevel(Logger::DEBUG);

    EventLoopThread loopThread;
    {
        InetAddress serverAddr("127.0.0.1", 22); // should succeed
        TcpClient client(loopThread.startLoop(), serverAddr, "TcpClient");
        client.connect();
        CurrentThread::sleepUsec(500 * 1000);
        client.disconnect();
    }

    CurrentThread::sleepUsec(1000 * 1000);
}