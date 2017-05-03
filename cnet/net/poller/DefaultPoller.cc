#include <cnet/net/Poller.h>
#include <cnet/net/poller/PollPoller.h>
#include <cnet/net/poller/EPollPoller.h>

#include <stdlib.h>

using namespace cnet::net;

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    if (::getenv("CNET_USE_POLL"))
    {
        return new PollPoller(loop);
    }
    else
    {
        return new EPollPoller(loop);
    }
}
