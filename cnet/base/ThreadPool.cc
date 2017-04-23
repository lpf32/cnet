#include <cnet/base/ThreadPool.h>
#include <cnet/base/Exception.h>

#include <boost/bind.hpp>
#include <algorithm>
#include <assert.h>
#include <assert.h>
#include <stdio.h>

using namespace cnet;

ThreadPool::ThreadPool(const string &nameArg)
    : mutex_(),
      notEmpty_(mutex_),
      notFull_(mutex_),
      name_(nameArg),
      maxQueueSize_(0),
      running_(false)
{ }

ThreadPool::~ThreadPool()
{
    if (running_)
    {
        stop();
    }
}

void ThreadPool::start(size_t numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);
    for (size_t i = 0; i < numThreads; i++)
    {
        char id[32];
        snprintf(id, sizeof id, "%lu", i+1);
        threads_.push_back(new cnet::Thread(
                boost::bind(&ThreadPool::runInThread, this), name_+id));
        threads_[i].start();
    }

    if (numThreads == 0 && threadInitCallback_)
    {
        threadInitCallback_();
    }
}

void ThreadPool::stop()
{
    {
        MutexLockGuard lock(mutex_);
        running_ = false;
        notEmpty_.notifyAll();
    }
    std::for_each(threads_.begin(),
                    threads_.end(),
                    boost::bind(&cnet::Thread::join, _1));
}

size_t ThreadPool::queueSize() const
{
    MutexLockGuard lock(mutex_);
    return queue_.size();
}

void ThreadPool::run(const Task &task)
{
    if (threads_.empty())
    {
        task();
    }
    else
    {
        MutexLockGuard lock(mutex_);
        while (isFull())
        {
            notFull_.wait();
        }
        assert(!isFull());
        queue_.push_back(task);
        notEmpty_.notify();
    }
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
void ThreadPool::run(Task &&task)
{
    if (threads_.empty())
    {
        task();
    }
    else
    {
        MutexLockGuard lock(mutex_);
        while (isFull())
        {
            notFull_.wait();
        }
        assert(!isFull());
        queue_.push_back(std::move(task));
        notEmpty_.notify();
    }
}
#endif

ThreadPool::Task ThreadPool::take()
{
    MutexLockGuard lock(mutex_);
    // always use a while-loop, due to spurious wakeup
    while (queue_.empty() && running_)
    {
        notEmpty_.wait();
    }

    Task task;
    if (!queue_.empty())
    {
        task = queue_.front();
        queue_.pop_front();
        // queue_ is limited only maxQueueSize > 0
        if (maxQueueSize_ > 0)
        {
            notFull_.notify();
        }
    }
    return task;
}

bool ThreadPool::isFull() const
{
    mutex_.assertLocked();
    return maxQueueSize_ > 0 && queue_.size() >= maxQueueSize_;
}

void ThreadPool::runInThread()
{
    try {
        if (threadInitCallback_)
        {
            threadInitCallback_();
        }
        while (running_)
        {
            Task task(take());
            if (task)
            {
                task();
            }
        }
    }
    catch (const Exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        fprintf(stderr, "statck trace: %s\n", ex.stackTrace());
    }
    catch (const std::exception& ex)
    {
        fprintf(stderr, "exception caught in ThreadPool %s\n", name_.c_str());
        fprintf(stderr, "reason: %s\n", ex.what());
        abort();
    }
    catch (...)
    {
        fprintf(stderr, "unknown exception caught in ThreadPool %s\n", name_.c_str());
        throw;
    }

}