#ifndef CNET_TIMERID_H
#define CNET_TIMERID_H

#include <cnet/base/copyable.h>

namespace cnet
{
namespace net
{

class Timer;
    
/*!
 *  An opaque identifier, for canceling Timer.
 */
    
class TimerId: public copyable
{
public:
    TimerId()
        : timer_(NULL),
          sequence_(0)
    { }
    
    TimerId(Timer* timer, int64_t seq)
        : timer_(timer),
          sequence_(seq)
    { }
    
    // default copy-ctor, dtor and assignment are okay
    friend class TimerQueue;

private:
    Timer* timer_;
    int64_t sequence_;
};
}
}

#endif //CNET_TIMERID_H
