#ifndef CNET_BLOCKINGQUEUE_H
#define CNET_BLOCKINGQUEUE_H

#include <cnet/base/Condition.h>
#include <cnet/base/Mutex.h>
#include <cnet/base/noncopyable.h>

#include <deque>
#include <assert.h>

namespace cnet
{
template <typename T>
class BlockingQueue : public noncopyable
{
public:
    BlockingQueue()
        : mutex_(),
          notEmpty_(mutex_),
          queue_()
    { }

    void put(const T& x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(x);
        notEmpty_.notify();
    }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    void put(T&& x)
    {
        MutexLockGuard lock(mutex_);
        queue_.push_back(std::move(x));
        notEmpty_.notify();
    }
#endif

    T take()
    {
        MutexLockGuard lock(mutex_);
        while (queue_.empty())
        {
            notEmpty_.wait();
        }
        assert(!queue_.empty());
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        T front(std::move(queue_.front()));
#else
        T front(queue_.front());
#endif
        queue_.pop_front();
        return front;
    }

    size_t size() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.size();
    }
private:
    mutable MutexLock mutex_;
    Condition notEmpty_;
    std::deque<T> queue_;
};
}

#endif //CNET_BLOCKINGQUEUE_H
