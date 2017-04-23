#ifndef CNET_COUNTDOWNLATCH_H
#define CNET_COUNTDOWNLATCH_H

#include <cnet/base/Condition.h>
#include <cnet/base/Mutex.h>
#include <cnet/base/noncopyable.h>

namespace cnet
{
class CountDownLatch : public noncopyable
{
public:
    explicit CountDownLatch(int count);

    void wait();

    void countDown();

    int getCount() const;

private:
    mutable MutexLock mutex_;
    Condition condition_;
    int count_;
};
}

#endif //CNET_COUNTDOWNLATCH_H
