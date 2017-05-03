#ifndef CNET_POLLPOLLER_H
#define CNET_POLLPOLLER_H

#include <cnet/net/Poller.h>
#include <vector>

struct pollfd;

namespace cnet
{
namespace net
{

    /*!
     *  IO multiplexing with poll(2)
     */
class PollPoller : public Poller
{
public:
    PollPoller(EventLoop* loop);
    virtual ~PollPoller();

    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels);
    virtual void updateChannel(Channel *channel);
    virtual void removeChannel(Channel *channel);

private:
    void fillActiveChannels(int numEvents, ChannelList* activeChannels) const;

    typedef std::vector<struct pollfd> PollFdList;
    PollFdList pollfds_;
};
}
}

#endif //CNET_POLLPOLLER_H
