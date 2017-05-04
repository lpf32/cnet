#ifndef CNET_TIMERQUEUE_H
#define CNET_TIMERQUEUE_H

#include <set>
#include <vector>

#include <cnet/base/noncopyable.h>

#include <cnet/base/Mutex.h>
#include <cnet/base/Timestamp.h>
#include <cnet/net/Callbacks.h>
#include <cnet/net/Channel.h>

namespace cnet
{
namespace net
{
class EventLoop;
class Timer;
class TimerId;
    
/*!
 *  A best efforts timer queue.
 *  No guarantee that the callback whill be on time.
 */
// FIXME real-time system
class TimerQueue : public noncopyable
{
public:
    TimerQueue(EventLoop* loop);
    ~TimerQueue();
    
    /*!
     *  Schedules the callback to be run at given time,
     *  repeats if @c interval > 0.0
     *  
     *  Must be thread safe. Usually be called from other threads.
     */

    TimerId addTiemr(const TimerCallback& cb,
                    Timestamp when,
                    double interval);

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    TimerId addTiemr(TimerCallback&& cb,
            Timestamp when,
            double interval);
#endif
            
    void cancel(TimerId timerId);

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<Timer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;
            
    void addTimerInLoop(Timer* timer);
    void cancelInLoop(TimerId timerId);
    // called when timerfd alarms
    void handleRead();
    // move out all expired timers
    std::vector<Entry> getExpired(Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);
            
    bool insert(Timer* timer);
    
    EventLoop* loop_;
    const int timerfd_;
    Channel timerfdChannel_;
    // Timer list sorted by expiration
    TimerList timers_;
            
    // for cancel()
    ActiveTimerSet activeTimers_;
    bool callingExpiredTimers_; /* atomic */
    ActiveTimerSet cancelingTimers_;
            
};
}
}

#endif //CNET_TIMERQUEUE_H
