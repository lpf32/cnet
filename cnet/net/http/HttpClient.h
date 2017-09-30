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
class HttpClent : boost::noncopyable
{
public:
    typedef boost::function<void (HttpRequest *)> HttpConnectionCallback;
    typedef boost::function<void (HttpResponse *)> HttpMessageCallback;

    HttpClent(EventLoop* loop, const InetAddress& serverAddr, const string& name);

    ~HttpClent();

    void setHttpConnectionCallback(const HttpConnectionCallback& cb)
    {
        httpConnectionCallback_ = cb;
    }

    void setHttpMessageCallbackconst (HttpMessageCallback& cb)
    {
        httpMessageCallback_ = cb;
    }

    
private:
    void connect()
    {
        client_.connect();
    }

    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf,
                   Timestamp receiveTime);
    TcpClient client_;
    HttpConnectionCallback httpConnectionCallback_;
    HttpMessageCallback httpMessageCallback_;
    TcpConnectionPtr conn_;
};
}
}

#endif //CNET_HTTPCLIENT_H
