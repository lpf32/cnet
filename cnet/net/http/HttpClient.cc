#include <cnet/net/http/HttpClient.h>

#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/http/HttpResponse.h>
#include <cnet/net/http/HttpContext.h>
#include <cnet/net/EventLoop.h>

#include <cnet/base/Logging.h>

#include <boost/bind.hpp>

using namespace cnet;
using namespace cnet::net;

HttpClient::HttpClient(EventLoop *loop,
                     const InetAddress &serverAddr,
                     const string &name,
                     const string &host)
    : client_(loop, serverAddr, name),
      host_(host)
{
    client_.setConnectionCallback(boost::bind(&HttpClient::onConnection, this, _1));
    client_.setMessageCallback(boost::bind(&HttpClient::onMessage, this, _1, _2, _3));
}

HttpClient::~HttpClient()
{
}

void HttpClient::onConnection(const TcpConnectionPtr &conn)
{
    if (conn->connected())
    {
        LOG_INFO << "Client connected";
        conn->setTcpNoDelay(true);

        HttpRequest req;
        req.addHeader("host", host_);
        httpConnectionCallback_(&req);
        Buffer buf;
        req.appendToBuffer(&buf);
        conn->send(&buf);
    }
}

void HttpClient::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
    HttpResponse req(true);
    req.setBody(string(buf->peek(), buf->readableBytes()));
    buf->retrieveAll();
    httpMessageCallback_(&req);
}


HttpClient* HttpClient::getHttpClient(EventLoop *loop, const string &host, const string& name)
{
    string::const_iterator colon = std::find(host.begin(), host.end(), ':');
    uint16_t port = 80;
    if (string(host.begin(), colon) == "http")
        port = 80;
    else if (string(host.begin(), colon) == "https")
        port = 443;
    assert(port == 80 || port == 443);

    InetAddress serverAddr(port);
    string hostname(colon+3,host.end());
    assert(InetAddress::resolve(hostname, &serverAddr));

    LOG_INFO << "connection to " << serverAddr.toIpPort();
    HttpClient* httpClient =  new HttpClient(loop, serverAddr, name, hostname);
    return httpClient;
}