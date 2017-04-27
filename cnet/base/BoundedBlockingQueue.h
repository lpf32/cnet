#ifndef CNET_BOUNDEDBLOCKINGQUEUE_H
#define CNET_BOUNDEDBLOCKINGQUEUE_H

#include <cnet/base/Condition.h>
#include <cnet/base/Mutex.h>
#include <cnet/base/noncopyable.h>

#include <boost/circular_buffer.hpp>
#include <assert.h>

namespace cnet
{
template <typename T>
class BoundedBlockingQueue : public noncopyable
{
public:
    explicit BoundedBlockingQueue(int maxSize)
        : mutex_(),
          nonEmpty_(mutex_),
          nonFull_(mutex_),
          queue_(maxSize)
    {}

    void put(const T& x)
    {
        MutexLockGuard lock(mutex_);
        while (queue_.full())
        {
            notFull_.wait();
        }
        assert(!queue_.full());
        queue_.push_front(x);
        notEmpty_.notify();
    }

    T take()
    {
        MutexLockGuard lock(mutex_);
        while (queue_.empty())
        {
            notEmpty_.wait();
        }
        assert(!queue_.empty());
        T front(queue_.front());
        notFull_.notify();
        return front;
    }

    bool empty() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.empty();
    }

    bool full() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.full();
    }

    size_t capacity() const
    {
        MutexLockGuard lock(mutex_);
        return queue_.capacity();
    }

private:
    mutable MutexLock mutex_;
    Condition notEmpty_;
    Condition notFull_;
    boost::circular_buffer<T> queue_;
};
}

#endif //CNET_BOUNDEDBLOCKINGQUEUE_H
