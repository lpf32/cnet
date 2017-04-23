#ifndef CNET_THREADPOOL_H
#define CNET_THREADPOOL_H

#include <cnet/base/Condition.h>
#include <cnet/base/Mutex.h>
#include <cnet/base/Thread.h>
#include <cnet/base/Types.h>
#include <cnet/base/noncopyable.h>

#include <boost/function.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <deque>

namespace cnet
{

class ThreadPool : public noncopyable
{
public:
    typedef boost::function<void ()> Task;

    explicit ThreadPool(const string& nameArg = string("ThreadPool"));
    ~ThreadPool();

    // Must be called before start()
    void setMaxQueueSize(size_t maxSize) { maxQueueSize_ = maxSize; }
    void setThreadInitCallback(const Task& cb) {
        threadInitCallback_ = cb;
    }

    void start(size_t numThreads);
    void stop();

    const string& name() const { return name_; }
    size_t queueSize() const;

    // Could block if maxQueueSize > 0
    void run(const Task& f);

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    void run(Task&& f);
#endif

private:
    bool isFull() const;
    void runInThread();
    Task take();

    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    string name_;
    Task threadInitCallback_;
    boost::ptr_vector<cnet::Thread> threads_;
    std::deque<Task> queue_;
    size_t maxQueueSize_;
    bool running_;
};
}

#endif //CNET_THREADPOOL_H
