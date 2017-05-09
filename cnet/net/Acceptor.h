#ifndef CNET_ACCEPTOR_H
#define CNET_ACCEPTOR_H

#include <boost/function.hpp>

#include <cnet/net/Channel.h>
#include <cnet/net/Socket.h>
#include <cnet/base/noncopyable.h>

namespace cnet
{
namespace net
{

class EventLoop;
class InetAddress;

/*!
 *  Acceptor of incoming TCP connections.
 */

class Acceptor : public noncopyable
{
public:
    typedef boost::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

    Acceptor(EventLoop *loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb)
    { newConnectionCallback_ = cb; }

    bool listening() const { return listenning_; }

    void listen();

private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
    int idleFd_;

};
}
}

#endif //CNET_ACCEPTOR_H
