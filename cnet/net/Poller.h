#ifndef CNET_POLLER_H
#define CNET_POLLER_H

#include <map>
#include <vector>

#include <cnet/base/noncopyable.h>
#include <cnet/base/Timestamp.h>
#include <cnet/net/EventLoop.h>

namespace cnet
{
namespace net
{
class Channel;

class Poller : public noncopyable
{
public:
    typedef std::vector<Channel*> ChannelList;

    Poller(EventLoop* loop);
    virtual ~Poller();

    /*!
     * Polls the I/O events.
     * Must be called in the loop thread.
     * @param timeoutMs
     * @param activeChannels
     * @return
     */
    virtual Timestamp poll(int timeoutMs, ChannelList* activeChannels) = 0;

    /*!
     *  Changes the interested I/O events.
     *  Must be called in the loop thread.
     */
    virtual void updateChannel(Channel *channel) = 0;

    /*!
     *  Remove the channel, when it destructs.
     *  Must be called in the loop thread.
     */
    virtual void removeChannel(Channel *channel) = 0;

    virtual bool hasChannel(Channel* channel) const;

    static Poller* newDefaultPoller(EventLoop* loop);

    void assertInLoopThread() const
    {
        ownerLoop_->assertInLoopThread();
    }

protected:
    typedef std::map<int, Channel*> ChannelMap;
    ChannelMap channels_;

private:
    EventLoop *ownerLoop_;
};
}
}

#endif //CNET_POLLER_H
