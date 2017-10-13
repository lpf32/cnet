#include <cnet/net/http/HttpClient.h>

#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/http/HttpResponse.h>
#include <cnet/net/http/HttpContext.h>
#include <cnet/net/EventLoop.h>

#include <cnet/base/Logging.h>

#include <boost/bind.hpp>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

using namespace cnet;
using namespace cnet::net;

HttpClient::HttpClient(EventLoop *loop,
                       const string &name,
                       HttpRequest* request)
    : request_(request),
      response_(true),
      client_(loop, request_->getServerAddr(), name)
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
        LOG_INFO << "Client connected, Http Version: " << request_->schemeString();
        conn->setTcpNoDelay(true);

        Buffer buf;
        request_->appendToBuffer(&buf);
        conn->send(&buf);
    }
}

void HttpClient::onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp receiveTime)
{
    if (request_->scheme() == HttpRequest::kHttps)
    {
        //TODO https
    }
    HttpResponse req(true);
    req.setBody(string(buf->peek(), buf->readableBytes()));
    buf->retrieveAll();
    httpMessageCallback_(&req);
}


HttpClient* HttpClient::get(EventLoop *loop, const string &url)
{
    HttpRequest *request = HttpRequest::parseURL(url, HttpRequest::kGet);

    HttpClient* client = new HttpClient(loop, "httpclient-get", request);
    return client;
}

HttpClient* HttpClient::post(EventLoop *loop, const string &url)
{
    HttpRequest *request = HttpRequest::parseURL(url, HttpRequest::kPost);

    HttpClient* client = new HttpClient(loop, "httpclient-post", request);
    return client;
}