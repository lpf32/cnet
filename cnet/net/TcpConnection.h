#ifndef CNET_TCPCONNECTION_H
#define CNET_TCPCONNECTION_H

#include <cnet/base/StringPiece.h>
#include <cnet/base/Types.h>
#include <cnet/base/noncopyable.h>
#include <cnet/net/Callbacks.h>
#include <cnet/net/Buffer.h>
#include <cnet/net/InetAddress.h>

#include <boost/any.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

struct tcp_info;

namespace cnet
{
namespace net
{

class Channel;
class EventLoop;
class Socket;

/*!
 *  TCP connection, for both client and server usage.
 *
 *  This is an interface class, so don't expose too much details.
 */

class TcpConnection: public noncopyable,
                     public boost::enable_shared_from_this<TcpConnection>

{
public:
    /*!
     *  Constructs a TcpConnection with a connected sockfd
     *
     *  User should not create this object.
     */
    TcpConnection(EventLoop* loop,
                const string& name,
                int sockfd,
                const InetAddress& localAddr,
                const InetAddress& peerAddr);
    ~TcpConnection();

    EventLoop* getLoop() const { return loop_; }
    const string& name() const { return name_; }
    const InetAddress& localAddress() const { return localAddr_; }
    const InetAddress& peerAddress() const { return peerAddr_; }
    bool connected() const { return state_ == kConnected; }
    bool disconnected() const { return state_ == kDisconnecting; }

    // return true if success.
    bool getTcpInfo(struct tcp_info*) const;
    string getTcpInfoString() const;

    void send(const void* message, int len);
    void send(const StringPiece& message);

    void send(Buffer* message);
    void shutdown(); // NOT thread safe, no simultaneous calling
    void forceClose();
    void forceCloseWithDelay(double seconds);
    void setTcpNoDelay(bool on);

    void startRead();
    void stopRead();
    bool isReading() const { return reading_; } // NOT thread safe, may race with start/stopReadInLoop

    void setContext(const boost::any& context)
    { context_ = context; }

    const boost::any& getContext() const
    { return context_; }

    boost::any* getMutableContext()
    { return &context_; }

    void setConnectionCallback(const ConnectionCallback& cb)
    { connectionCallback_ = cb; }

    void setMessageCallback(const MessageCallback& cb)
    { messageCallback_ = cb; }

    void setWriteCompleteCallback(const WriteCompleteCallback& cb)
    { writeCompleteCallback_ = cb; }

    void setHighWaterMarkCallback(const HighWaterMarkCallback& cb, size_t highWaterMark)
    { highWaterMarkCallback_ = cb; highWaterMark_ = highWaterMark; }

    // Advanced interface
    Buffer* inputBuffer()
    { return &inputBuffer_; }

    Buffer* outputBuffer()
    { return &outpuBuffer_; }

    void setCloseCallback(const CloseCallback& cb)
    { closeCallback_ = cb; }

    // called when TcpServer accepts a new connection
    void connectEstablished();  // should be called only once
    // called when TcpServer has removed me from its map
    void connectDestroyed(); // should be called only once

private:
    enum StateE {kDisconnected, kConnecting, kConnected, kDisconnecting };
    void handleRead(Timestamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInLoop(const StringPiece& message);
    void sendInLoop(const void* message, size_t len);
    void shutdownInLoop();
    void forceCloseInLoop();
    void setState(StateE s) { state_ = s; }
    const char* stateToString() const;
    void startReadInLoop();
    void stopReadInLoop();

private:
    EventLoop *loop_;
    const string name_;
    StateE state_; // FIXME: use atomic variable
    bool reading_;
    boost::scoped_ptr<Socket> socket_;
    boost::scoped_ptr<Channel> channel_;
    const InetAddress localAddr_;
    const InetAddress peerAddr_;
    ConnectionCallback connectionCallback_;
    MessageCallback messageCallback_;
    WriteCompleteCallback writeCompleteCallback_;
    HighWaterMarkCallback highWaterMarkCallback_;
    CloseCallback closeCallback_;
    size_t highWaterMark_;
    Buffer inputBuffer_;
    Buffer outpuBuffer_;  // FIXME: use list<Buffer> as output buffer.
    boost::any context_;
};
}
}

#endif //CNET_TCPCONNECTION_H
