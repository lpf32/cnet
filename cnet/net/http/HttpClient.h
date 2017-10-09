#ifndef CNET_HTTPCLIENT_H
#define CNET_HTTPCLIENT_H

#include <cnet/net/TcpClient.h>
#include <boost/core/noncopyable.hpp>

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

    HttpClient(EventLoop* loop, const InetAddress& serverAddr, const string& name, const string& host);

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

    void post(const string& url)
    {

    }

    void get(const string& url)
    {

    }

    static HttpClient* getHttpClient(EventLoop *loop, const string &host, const string& name);
    
private:
    
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf,
                   Timestamp receiveTime);
    void onResponse(const TcpConnectionPtr&, const HttpResponse&);

    TcpClient client_;
    string host_;
    HttpConnectionCallback httpConnectionCallback_;
    HttpMessageCallback httpMessageCallback_;
};
}
}

#endif //CNET_HTTPCLIENT_H
