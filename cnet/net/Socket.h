#ifndef CNET_SOCKET_H
#define CNET_SOCKET_H

#include <cnet/base/noncopyable.h>
#include <netinet/tcp.h>

// struct tcp_info is in <netinet/tcp.h>
struct tcp_info;

namespace cnet
{
namespace net
{

class InetAddress;

/*!
 *  Wrapper of socket file descriptor.
 *
 *  It close the sockfd when destructs.
 *  It's thread safe, all operations are delegated to OS
 */
class Socket: public noncopyable
{
public:
    explicit Socket(int sockfd)
        : sockfd_(sockfd)
    { }

    ~Socket();

    int fd() const { return sockfd_; }

    bool getTcpInfo(struct tcp_info*) const;
    bool getTcpInfoString(char *buf, int len) const;

    /// abort if address in use
    void bindAddress(const InetAddress& localaddr);

    ///abort if address in use
    void listen();

    /*!
     *  On success, return a non-negative integer that is
     *  a descriptor for the accepted socket, which has been
     *  set to non-blocking and close-on-exec. *perraddr is assigned.
     *  On error, -1 is returned, and *peeraddr is untouched.
     */
    int accept(InetAddress *peeraddr);

    void shutdwonWrite();

    /*!
     *  Enable/diable ICP_NODELAY (disable/enable Nagle's algorithm).
     */
    void setTcpNoDelay(bool on);

    /*!
     *  Enable/disable SO_REUSEADDR
     */
    void setReuseAddr(bool on);

    /*!
     *  Enable/disable SO_REUSEPORT
     */
    void setReusePort(bool on);

    /*!
     *  Enable/disable SO_KEEPALIVE
     */
    void setKeepAlive(bool on);

private:
    const int sockfd_;
};
}
}

#endif //CNET_SOCKET_H
