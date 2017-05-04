#ifndef CNET_TIMER_H
#define CNET_TIMER_H

#include <cnet/base/noncopyable.h>
#include <cnet/base/Atomic.h>
#include <cnet/base/Timestamp.h>
#include <cnet/net/Callbacks.h>

namespace cnet
{
namespace net
{
/*!
 *  Internal class for timer evnet.
 */
class Timer : public noncopyable
{
public:
    Timer(const TimerCallback& cb, Timestamp when, double interval)
        : callback_(cb),
          expiration_(when),
          interval_(interval),
          repeat_(interval > 0.0),
          sequence_(s_numCreated_.incrementAndGet())
    { }

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    Timer(TimerCallback&& cb, Timestamp when, double interval)
        : callback_(std::move(cb)),
          expiration_(when),
          interval_(interval),
          repeat_(interval > 0.0),
          sequence_(s_numCreated_.incrementAndGet())
    { }
#endif
        
    void run() const 
    { callback_(); }
        
    Timestamp expiration() const { return expiration_; }
    bool repeat() const { return repeat_; }
    int64_t sequence() const { return sequence_; }
        
    void restart(Timestamp now);
        
    static int64_t numCreated() { return s_numCreated_.get(); }

private:
    const TimerCallback callback_;
    Timestamp expiration_;
    const double interval_;
    const bool repeat_;
    const int64_t sequence_;
    
    static AtomicInt64 s_numCreated_;
};
}
}

#endif //CNET_TIMER_H
