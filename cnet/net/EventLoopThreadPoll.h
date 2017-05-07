#ifndef CNET_EVENTLOOPTHREADPOLL_H
#define CNET_EVENTLOOPTHREADPOLL_H

#include <cnet/base/Types.h>
#include <cnet/base/noncopyable.h>

#include <vector>
#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace cnet
{
namespace net
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPoll: public noncopyable
{
public:
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThreadPoll(EventLoop* baseLoop, const string& nameArg);
    ~EventLoopThreadPoll();
    void setThreadNum(int numThreads) { numThreads_ = numThreads; }
    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    // valid after calling start()
    /// round-robin
    EventLoop* getNextLoop();

    // with the same hash code, it will always return the same EventLoop
    EventLoop* getLoopForHash(size_t hashCode);

    std::vector<EventLoop*> getAllLoops();

    bool started() const
    { return started_; }

    const string& name() const
    { return name_; }

private:
    EventLoop* baseLoop_;
    string name_;
    bool started_;
    int numThreads_;
    int next_;
    boost::ptr_vector<EventLoopThread> threads_;
    std::vector<EventLoop*> loops_;
};
}
}

#endif //CNET_EVENTLOOPTHREADPOLL_H
