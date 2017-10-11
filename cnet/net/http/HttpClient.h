#ifndef CNET_HTTPCLIENT_H
#define CNET_HTTPCLIENT_H

#include <cnet/net/TcpClient.h>
#include <boost/core/noncopyable.hpp>

#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/http/HttpResponse.h>

namespace cnet
{
namespace net
{

/**
*  A simple Http Client
*/
class HttpRequest;
class HttpResponse;
class HttpClient : boost::noncopyable
{
public:
    typedef boost::function<void (HttpRequest *)> HttpConnectionCallback;
    typedef boost::function<void (HttpResponse *)> HttpMessageCallback;

    HttpClient(EventLoop* loop, const string& name, HttpRequest* request);

    ~HttpClient();

    void setHttpConnectionCallback(const HttpConnectionCallback& cb)
    {
        httpConnectionCallback_ = cb;
    }

    void setHttpMessageCallback (const HttpMessageCallback& cb)
    {
        httpMessageCallback_ = cb;
    }

    void connect()
    {
        client_.connect();
    }

    static HttpClient* post(EventLoop *loop, const string& url);

    static HttpClient* get(EventLoop *loop, const string& url);
    
private:
    
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf,
                   Timestamp receiveTime);
    void onResponse(const TcpConnectionPtr&, const HttpResponse&);

    HttpRequest *request_;
    HttpResponse response_;
    TcpClient client_;
    HttpConnectionCallback httpConnectionCallback_;
    HttpMessageCallback httpMessageCallback_;
};
}
}

#endif //CNET_HTTPCLIENT_H
