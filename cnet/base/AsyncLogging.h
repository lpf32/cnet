#ifndef CNET_ASYNCLOGGING_H
#define CNET_ASYNCLOGGING_H

#include <cnet/base/BlockingQueue.h>
#include <cnet/base/BoundedBlockingQueue.h>
#include <cnet/base/CountDownLatch.h>
#include <cnet/base/Mutex.h>
#include <cnet/base/Thread.h>
#include <cnet/base/LogStream.h>
#include <cnet/base/noncopyable.h>

#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

namespace cnet
{

class AsyncLogging: public noncopyable
{
public:
    AsyncLogging(const string& basename,
        size_t rollSize,
        int flushInterval = 3);

    ~AsyncLogging()
    {
        if(running_)
        {
            stop();
        }
    }

    void append(const char* logline, int len);

    void start()
    {
        running_ = true;
        thread_.start();
        latch_.wait();
    }

    void stop()
    {
        running_ = false;
        cond_.notify();
        thread_.join();
    }

private:

    AsyncLogging(const AsyncLogging&); //ptr_container
    void operator=(const AsyncLogging&); //ptr_container

    void threadFunc();

    typedef cnet::detail::FixedBuffer<cnet::detail::kLargeBuffer> Buffer;
    typedef boost::ptr_vector<Buffer> BufferVector;
    typedef BufferVector::auto_type BufferPtr;

    const int flushInterval_;
    bool running_;
    string basename_;
    size_t rollSize_;
    cnet::Thread thread_;
    cnet::CountDownLatch latch_;
    cnet::MutexLock mutex_;
    cnet::Condition cond_;
    BufferPtr currentBuffer_;
    BufferPtr nextBuffer_;
    BufferVector buffers_;
};
}

#endif //CNET_ASYNCLOGGING_H
