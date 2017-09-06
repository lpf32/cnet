#include <cnet/net/EventLoop.h>

#include <cnet/base/Logging.h>
#include <cnet/base/Mutex.h>
#include <cnet/net/Channel.h>
#include <cnet/net/Poller.h>
#include <cnet/net/SocketsOps.h>
#include <cnet/net/TimerQueue.h>

#include <boost/bind.hpp>

#include <signal.h>
#include <sys/eventfd.h>

using namespace cnet;
using namespace cnet::net;

namespace
{
__thread EventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_SYSFATAL << "Failed in eventfd";
    }
    return evtfd;
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
class IgnoreSigPipe
{
public:
    IgnoreSigPipe()
    {
        ::signal(SIGPIPE, SIG_IGN);
    }
};
#pragma GCC diagnostic error "-Wold-style-cast"
IgnoreSigPipe initObj;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

EventLoop::EventLoop()
    : looping_(false),
      quit_(EventLoop::CREATED),
      eventHanding_(false),
      iteration_(0),
      threadId_(CurrentThread::tid()),
      poller_(Poller::newDefaultPoller(this)),
      timerQueue_(new TimerQueue(this)),
      wakeupFd_(createEventfd()),
      wakeupChannel_(new Channel(this, wakeupFd_)),
      currentActiveChannel_(NULL)
{
    LOG_DEBUG << "EventLoop created " << this << " in thread " << threadId_;
    if (t_loopInThisThread)
    {
        LOG_FATAL << "Another EventLoop " << t_loopInThisThread
                  << " exists in this thread " << threadId_;
    }
    else
    {
        t_loopInThisThread = this;
    }
    wakeupChannel_->setReadCallback(
            boost::bind(&EventLoop::handleRead, this)
    );
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
    LOG_DEBUG << "EventLoop " << this << " of thread " << threadId_
               << " destructs in thread " << CurrentThread::tid();
    wakeupChannel_->disableAll();
    wakeupChannel_->remove();
    ::close(wakeupFd_);
    t_loopInThisThread = NULL;
}

void EventLoop::loop()
{
    assert(!looping_);
    assertInLoopThread();
    looping_ = true;
    if (quit_ == EventLoop::CREATED)
        quit_ = EventLoop::RUNNING; // FIXME: what if someone calls quit() before loop()?
    LOG_TRACE << "EventLoop " << this << " start looping";

    while (quit_ == EventLoop::RUNNING)
    {
        activeChannels_.clear();
        pollReturnTime_ = poller_->poll(kPollTimeMs, &activeChannels_);
        ++iteration_;
        if (Logger::logLevel() <= Logger::TRACE)
        {
            printActiveChannels();
        }
        // TODO sort channel by priority
        eventHanding_ = true;
        for (ChannelList::iterator it = activeChannels_.begin();
                it != activeChannels_.end(); ++it)
        {
            currentActiveChannel_ = *it;
            currentActiveChannel_->handleEvent(pollReturnTime_);
        }
        currentActiveChannel_ = NULL;
        eventHanding_ = false;
        doPendingFunctors();
    }

    LOG_TRACE << "EventLoop " << this << " stop looping";
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = EventLoop::QUITED;
    // There is a chance that loop() just executes while(!quit_) and exits,
    // then EventLoop destructs, then we are accessing an invalid object.
    // Can be fixed using mutex_ in both place.
    if (!isInLoopThread())
    {
        wakeup();
    }
}

void EventLoop::runInLoop(const Functor &cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const Functor &cb)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }

    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

size_t EventLoop::queueSize() const
{
    MutexLockGuard lock(mutex_);
    return pendingFunctors_.size();
}

TimerId EventLoop::runAt(const Timestamp &time, const TimerCallback &cb)
{
    return timerQueue_->addTiemr(cb, time, 0.0);
}

TimerId EventLoop::runAfter(double delay, const TimerCallback &cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, cb);
}

TimerId EventLoop::runEvery(double interval, const TimerCallback &cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTiemr(cb, time, interval);
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
void EventLoop::runInLoop(Functor &&cb)
{
    if (isInLoopThread())
    {
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}

void EventLoop::queueInLoop(Functor &&cb)
{
    {
        MutexLockGuard lock(mutex_);
        pendingFunctors_.push_back(std::move(cb));
    }

    if (!isInLoopThread() || callingPendingFunctors_)
    {
        wakeup();
    }
}

TimerId EventLoop::runAt(const Timestamp &time, TimerCallback &&cb)
{
    return timerQueue_->addTiemr(std::move(cb), time, 0.0);
}

TimerId EventLoop::runAfter(double delay, TimerCallback &&cb)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(time, std::move(cb));
}

TimerId EventLoop::runEvery(double interval, TimerCallback &&cb)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTiemr(std::move(cb), time, interval);
}
#endif

void EventLoop::cancel(TimerId timerId)
{
    return timerQueue_->cancel(timerId);
}

void EventLoop::updateChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    if (eventHanding_)
    {
        assert(currentActiveChannel_ == channel ||
            std::find(activeChannels_.begin(), activeChannels_.end(), channel) == activeChannels_.end());
    }
    poller_->removeChannel(channel);
}

bool EventLoop::hasChannel(Channel *channel)
{
    assert(channel->ownerLoop() == this);
    assertInLoopThread();
    return poller_->hasChannel(channel);
}

void EventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "EventLoop::abortNotInLoopThread - EventLoop " << this
              << " was created in ThreadId_ = " << threadId_
              << ", current thread id = " << CurrentThread::tid();
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    ssize_t n = sockets::write(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::wakeup() write " << n << " bytes instead of 8";
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = sockets::read(wakeupFd_, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "EventLoop::handleRead() write " << n << " bytes instead of 8";
    }
}

void EventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    callingPendingFunctors_ = true;

    {
        MutexLockGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for (size_t i = 0; i < functors.size(); ++i)
    {
        functors[i]();
    }
    callingPendingFunctors_ = false;
}

void EventLoop::printActiveChannels() const
{
    for (ChannelList::const_iterator it = activeChannels_.begin();
            it != activeChannels_.end(); ++it)
    {
        const Channel* ch = *it;
        LOG_TRACE << "{" << ch->reventsToString() << "} ";
    }
}