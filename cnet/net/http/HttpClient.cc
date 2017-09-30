#include <cnet/net/http/HttpClient.h>

#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/http/HttpResponse.h>
#include <cnet/net/http/HttpContext.h>

#include <cnet/base/Logging.h>

#include <boost/bind.hpp>

using namespace cnet;
using namespace cnet::net;

HttpClent::HttpClent(EventLoop *loop,
                     const InetAddress &serverAddr,
                     const string &name)
    : client_(loop, serverAddr, name)
{
    client_.setConnectionCallback(boost::bind(&HttpClent::onConnection, this, _1));
    client_.setMessageCallback(boost::bind(&HttpClent::onMessage, this, _1, _2, _3));
}

HttpClent::~HttpClent()
{
}

void HttpClent::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        LOG_INFO << "Client ";
        conn->setTcpNoDelay(true);

        HttpRequest req;
        httpConnectionCallback_(&req);
        Buffer buf;
        req.appendToBuffer(&buf);
        conn->send(&buf);
    }
}

void HttpClent::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
    httpMessageCallback_(&buf);
}
