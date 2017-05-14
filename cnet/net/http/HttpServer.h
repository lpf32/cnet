#ifndef CNET_HTTPSERVER_H
#define CNET_HTTPSERVER_H

#include <cnet/net/TcpServer.h>
#include <cnet/base/noncopyable.h>

namespace cnet
{
namespace net
{

class HttpRequest;
class HttpResponse;

/*!
 *  A simple embeddable HTTP server designed for report status of a program.
 *  It is not fully HTTP 1.1 compliant server, but provides minimum features
 *  that can communicate with HttpClient and web browser.
 *  It is synchronous, just like Java Servlet.
 *
 */
class HttpServer : public noncopyable
{
public:
    typedef boost::function<void (const HttpRequest&, HttpResponse*)> HttpCallback;

    HttpServer(EventLoop *loop,
                const InetAddress& listenAddr,
                const string& name,
                TcpServer::Option option = TcpServer::kNoReusePort);

    ~HttpServer();

    EventLoop* getLoop() const { return server_.getLoop(); }

    /// Not thread safe, callback be registered before calling start()
    void setHttpCallback(const HttpCallback& cb)
    { httpCallback_ = cb; }

    void setThreadNum(int numThreads)
    { server_.setThreadNum(numThreads); }

    void start();

private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn, Buffer *buf, Timestamp receiveTime);
    void onRequest(const TcpConnectionPtr&, const HttpRequest&);

    TcpServer server_;
    HttpCallback httpCallback_;
};
}
}

#endif //CNET_HTTPSERVER_H
