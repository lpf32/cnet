#include <cnet/net/Socket.h>

#include <cnet/base/Logging.h>
#include <cnet/net/InetAddress.h>
#include <cnet/net/SocketsOps.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string.h>
#include <stdio.h>

using namespace cnet;
using namespace cnet::net;

Socket::~Socket()
{
    sockets::close(sockfd_);
}

bool Socket::getTcpInfo(struct tcp_info *tcpi) const
{
    socklen_t len = sizeof(*tcpi);
    bzero(tcpi, len);
    return ::getsockopt(sockfd_, SOL_TCP, TCP_INFO, tcpi, &len) == 0;
}

bool Socket::getTcpInfoString(char *buf, int len) const
{
    struct tcp_info tcpi;
    bool ok = getTcpInfo(&tcpi);
    if (ok)
    {
        snprintf(buf, len, "unrecovered=%u "
            "rto=%u ato=%u snd_mss=%u rcv_mss=%u "
            "lost=%u retrans=%u rtt=%u rttvar=%u "
            "sshthresh=%u cwnd=%u total_retrans=%u",
        tcpi.tcpi_retransmits,  // Number of unrecovered [RTO] timeouts
        tcpi.tcpi_rto,          // Retransmit timeout in usec
        tcpi.tcpi_ato,
        tcpi.tcpi_snd_mss,
        tcpi.tcpi_rcv_mss,
        tcpi.tcpi_lost,          // Lost packets
        tcpi.tcpi_retrans,       // Retransmitted packets out
        tcpi.tcpi_rtt,           // Smoothed round trip time in usec
        tcpi.tcpi_rttvar,        // Medium deviation
        tcpi.tcpi_snd_ssthresh,
        tcpi.tcpi_snd_cwnd,
        tcpi.tcpi_total_retrans); // Total retransmits for entire connection
    }
    return ok;
}

void Socket::bindAddress(const InetAddress &addr)
{
    sockets::bindOrdie(sockfd_, addr.getSockAddr());
}

void Socket::listen()
{
    sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress *peeraddr)
{
    struct sockaddr_in6 addr;
    bzero(&addr, sizeof addr);
    int connfd = sockets::accept(sockfd_,&addr);
    if (connfd >= 0)
    {
        peeraddr->setSockAddrInet6(addr);
    }
    return connfd;
}

void Socket::shutdwonWrite()
{
    sockets::shutdownWrite(sockfd_);
}

void Socket::setTcpNoDelay(bool on)
{
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, IPPROTO_TCP, TCP_NODELAY,
                &optval, static_cast<socklen_t >(sizeof optval)) < 0)
        LOG_SYSERR << "Socket::setTcpNoDelay, on: " << (on ? "true": "false");

}

void Socket::setReuseAddr(bool on)
{
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
                     &optval, static_cast<socklen_t >(sizeof optval)) < 0)
        LOG_SYSERR << "Socket::setTcpNoDelay" << (on ? "true": "false");
}

void Socket::setReusePort(bool on)
{
#ifdef SO_REUSEPORT
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEPORT,
                     &optval, static_cast<socklen_t >(sizeof optval)) < 0)
        LOG_SYSERR << "Socket::setTcpNoDelay" << (on ? "true": "false");
#else
    LOG_ERROR << "SO_REUSEPORT is not supported."
#endif
}

void Socket::setKeepAlive(bool on)
{
    int optval = on ? 1 : 0;
    if (::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
                     &optval, static_cast<socklen_t >(sizeof optval)) < 0)
        LOG_SYSERR << "Socket::setKeepAlive" << (on ? "true": "false");
}