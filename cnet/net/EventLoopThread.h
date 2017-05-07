#ifndef CNET_EVENTLOOPTHREAD_H
#define CNET_EVENTLOOPTHREAD_H

#include <cnet/base/Condition.h>
#include <cnet/base/Mutex.h>
#include <cnet/base/Thread.h>

#include <cnet/base/noncopyable.h>

namespace cnet
{
namespace net
{

class EventLoop;

class EventLoopThread : public noncopyable
{
public:
    typedef boost::function<void(EventLoop*)> ThreadInitCallback;

    EventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(),
                    const string& name = string());

    ~EventLoopThread();

    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* loop_;
    bool exiting_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    ThreadInitCallback callback_;
};
}
}

#endif //CNET_EVENTLOOPTHREAD_H
