#ifndef CNET_TIMESTAMP_H
#define CNET_TIMESTAMP_H

#include <cnet/base/copyable.h>
#include <cnet/base/Types.h>

#include <boost/operators.hpp>

namespace cnet
{

/*!
 *  Time stamp in UTC, in microseconds resolution.
 *
 *  This class is immutable
 *  It's recommended to pass it by value, since it's passed in register on x64
 */

class Timestamp : public copyable,
                  public boost::less_than_comparable<Timestamp>
{
public:
    /*!
     * Constructs an invalid Timestamp.
     */

    Timestamp()
        : microSecondsSinceEpoch_(0)
    { }

    explicit Timestamp(int64_t microSecondsSinceEpochArg)
        : microSecondsSinceEpoch_(microSecondsSinceEpochArg)
    { }

    void swap(Timestamp &that)
    {
        std::swap(microSecondsSinceEpoch_, that.microSecondsSinceEpoch_);
    }

    // default copy/assignment/dtor are Okay

    string toString() const;
    string toFormattedString(bool showMicroseconds = true) const;

    bool valid() const { return microSecondsSinceEpoch_ > 0; }

    // for internal usage
    int64_t microSecondsSinceEpoch() const { return microSecondsSinceEpoch_; }
    time_t secondsSinceEpoch() const
    {
        return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    }

    /*!
     * Get time of now
     *
     */

    static Timestamp now();
    static Timestamp invalid()
    {
        return Timestamp();
    }

    static Timestamp fromUnixTime(time_t t)
    {
        return fromUnixTime(t, 0);
    }

    static Timestamp fromUnixTime(time_t t, int microseconds)
    {
        return Timestamp(static_cast<int64_t>(t)*kMicroSecondsPerSecond + microseconds);
    }


    static const int kMicroSecondsPerSecond = 1000 * 1000;
private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

/*!
 * Get time difference of two timestamps, result in seconds.
 *
 * @param high, low
 * @return (high-low) in seconds
 * @c double has 52-bit precision, enough for one-microsecond
 * resolution for next 100 years
 */

inline double timeDifference(Timestamp high, Timestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

/*!
 * add @c seconds to given timestamp;
 *
 * @return timestamp+seconds as Timestamp
 */
inline Timestamp addTime(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

}

#endif //CNET_TIMESTAMP_H
