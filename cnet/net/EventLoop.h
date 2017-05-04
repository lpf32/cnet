#ifndef CNET_EVENTLOOP_H
#define CNET_EVENTLOOP_H

#include <vector>
#include <boost/any.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>

#include <cnet/base/noncopyable.h>
#include <cnet/base/Mutex.h>
#include <cnet/base/CurrentThread.h>
#include <cnet/base/Timestamp.h>
#include <cnet/net/Callbacks.h>
#include <cnet/net/TimerId.h>

namespace cnet
{
namespace net
{

class Channel;
class Poller;
class TimerQueue;

/*!
 *  Reactor, at most one per thread
 *
 *  This is an interface class, so don't expose too much details.
 */

class EventLoop : public noncopyable
{
public:
    typedef boost::function<void()> Functor;

    EventLoop();
    ~EventLoop();

    /*!
     *  Loops forever.
     *
     *  Must be called in the same thread as creation of the object.
     *
     */

    void loop();

    /*!
     *  Quits loop.
     *
     *  This is not 100% thread safe, if you call through a raw pointer,
     *  better to call through shared_ptr<EventLoop> for 100% safety.
     *
     */

    void quit();

    /*!
     *  Time when poll return, ususlly means data arrival.
     */

    Timestamp pollReturnTime() const { return pollReturnTime_; }

    int64_t iteration() const { return iteration_; }

    /*!
     *  Runs callback immediately in the loop thread.
     *  It wakes up the loop, and run the cb.
     *  If in the smae loop thread, cb is run within the function.
     *  Safe to call from other threads.
     */

    void runInLoop(const Functor& cb);
    /*!
     *  Queues callback in the loop thread.
     *  Runs after finish polling.
     *  Safe to call from other thread.
     *
     */

    void queueInLoop(const Functor& cb);

    size_t queueSize() const;

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    void runInLoop(Functor&& cb);
    void queueInLoop(Functor&& cb);
#endif

    // timers

    /*!
     *  Runs callback at 'time'
     *  Safe to call from other threads.
     */
    TimerId runAt(const Timestamp& time, const TimerCallback& cb);
    /*!
     *  Runs callback after @c delay seconds.
     *  Safe to call from other threads.
     */
    TimerId runAfter(double delay, const TimerCallback& cb);
    /*!
     *  Runs callback every @c interval seconds.
     *  Safe to call from other threads.
     */
    TimerId runEvery(double interval, const TimerCallback& cb);

    /*!
     *  Cancels the timers.
     *  Safe to call from other threads.
     */
    void cancel(TimerId timerId);

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    TimerId runAt(const Timestamp& time, TimerCallback&& cb);
    TimerId runAfter(double delay, TimerCallback&& cb);
    TimerId runEvery(double interval, TimerCallback&& cb);
#endif

    // internal usage
    void wakeup();
    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);
    bool hasChannel(Channel* channel);

    // pid_t threadId() cosnt { return threadId_; }

    void assertInLoopThread()
    {
        if (!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }

    bool isInLoopThread() const { return threadId_ == CurrentThread::tid(); }

    bool eventHandling() const { return eventHanding_; }

    void setContext(const boost::any& context)
    { context_ = context; }

    boost::any* getMutableContext()
    {
        return &context_;
    }

    static EventLoop* getEventLoopOfCurrentThread();

private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();

    void printActiveChannels() const; // debug

    typedef std::vector<Channel*> ChannelList;

    bool looping_; /* atomic */
    bool quit_; /* atomic and shared between threads, okay on x86, I guess*/
    bool eventHanding_; /* atomic */
    bool callingPendingFunctors_; /* atomic */
    int64_t iteration_;
    const pid_t threadId_;
    Timestamp pollReturnTime_;
    boost::scoped_ptr<Poller> poller_;
    boost::scoped_ptr<TimerQueue> timerQueue_;
    int wakeupFd_;

    // unlike in TimerQueue, which is an internal class
    // we don't expose Channel to client
    boost::scoped_ptr<Channel> wakeupChannel_;
    boost::any context_;

    ChannelList activeChannels_;
    Channel* currentActiveChannel_;

    mutable MutexLock mutex_;
    std::vector<Functor> pendingFunctors_; // Guarded by mutex_
};
}
}

#endif //CNET_EVENTLOOP_H
