#ifndef CNET_INETADDRESS_H
#define CNET_INETADDRESS_H

#include <cnet/base/copyable.h>
#include <cnet/base/StringPiece.h>

#include <netinet/in.h>

namespace cnet
{
namespace net
{
namespace sockets
{
const struct sockaddr* sockaddr_cast(const struct sockaddr_in6*);
}

/*!
 *  Wrapper of sockaddr_in
 *
 *  This is an POD interface class
 */
class InetAddress : public copyable
{
public:
    /*!
     *  Constructs an endpoint with given port number.
     *  Moustly used in TcpServer listening.
     *
     */

    explicit InetAddress(uint16_t port = 0, bool loopbackOnly = false, bool ipv6 = false);

    /*!
     *  Constructs an endpoint with given ip and port
     * @param ip should be "1.2.3.5"
     */
    InetAddress(StringArg ip, uint16_t port, bool ipv6 = false);

    /*!
     *  Constructs an endpoint with given struct
     *  Moustly used when accepting new connections
     * @param addr
     */
    explicit InetAddress(const struct sockaddr_in& addr)
        : addr_(addr)
    { }

    explicit InetAddress(const struct sockaddr_in6& addr6)
        : addr6_(addr6)
    { }

    sa_family_t family() const { return addr_.sin_family; }
    string toIp() const;
    string toIpPort() const;
    uint16_t toPort() const;

    const struct sockaddr* getSockAddr() const { return sockets::sockaddr_cast(&addr6_); }
    void setSockAddrInet6(const struct sockaddr_in6& addr6) { addr6_ = addr6; }

    uint32_t ipNetEndian() const;
    uint16_t portNetEndian() const { return addr_.sin_port; }

    // resovle hostname to IP address, not changing port or sin_family
    // return true on sucess.
    // thread safe
    static bool resolve(StringArg hostname, InetAddress* result);

private:
    union
    {
        struct sockaddr_in addr_;
        struct sockaddr_in6 addr6_;
    };
};
}
}

#endif //CNET_INETADDRESS_H
