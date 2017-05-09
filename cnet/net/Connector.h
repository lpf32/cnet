#ifndef CNET_CONNECTOR_H
#define CNET_CONNECTOR_H

#include <cnet/net/InetAddress.h>
#include <cnet/base/noncopyable.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

namespace cnet
{
namespace net
{
class Channel;
class EventLoop;

class Connector : public noncopyable,
                  public boost::enable_shared_from_this<Connector>
{
public:
    typedef boost::function<void (int sockfd)> NewConnectionCallback;

    Connector(EventLoop *loop, const InetAddress& serverAddr);
    ~Connector();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

    void start();  // can be called in any thread
    void restart(); // can be called in any thread
    void stop();  // can be called in any thread

    const InetAddress& serverAddress() const { return serverAddr_; }
private:
    enum States {kDisconnected, kConnecting, kConnected };
    static const int kMaxRetryDelayMs;
    static const int kInitRetryDelayMs;

    void setState(States s) { state_ = s;}
    void startInLoop();
    void stopInLoop();
    void connect();
    void connecting(int sockfd);
    void handleWrite();
    void handleError();
    void retry(int sockfd);
    int removeAndResetChannel();
    void resetChannel();

    EventLoop *loop_;
    InetAddress serverAddr_;
    bool connect_; // atomic
    States state_; // FIXME: use atomic variable
    boost::scoped_ptr<Channel> channel_;
    NewConnectionCallback newConnectionCallback_;
    int retryDelayMs_;
};
}
}

#endif //CNET_CONNECTOR_H
