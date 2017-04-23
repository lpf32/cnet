#ifndef CNET_TIMEZONE_H
#define CNET_TIMEZONE_H

#include <cnet/base/copyable.h>
#ifdef __GXX_EXPERIMENTAL_CXX0X__
#include <memory>
#else
#include <boost/shared_ptr.hpp>
#endif
#include <time.h>
#include <boost/smart_ptr/shared_ptr.hpp>

namespace cnet
{
// TimeZone for 1970-2030
class TimeZone : public cnet::copyable
{
public:
    explicit TimeZone(const char* zonefile);
    TimeZone(int eastOfUtc, const char* tzname);
    TimeZone() {} // an invalid timezone

    // default copy ctor/assignment/dtor are Okay

    bool valid() const {
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        return bool(data_);
#else
        return static_cast<bool>(data_);
#endif
    }

    struct tm toLocalTime(time_t secondsSinceEpoch) const;
    time_t fromLocalTime(const struct tm&) const;

    // gmtime(3)
    static struct tm toUtcTime(time_t secondsSinceEpoch, bool yday = false);
    // timegm(3)
    static time_t fromUtcTime(const struct tm&);
    // year in [1900..2500], month[1..12], day in [1..31]
    static time_t fromUtcTime(int year, int month, int day,
                                int hour, int minute, int seconds);

    struct Data;

private:
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    std::shared_ptr<Data> data_;
#else
    boost::shared_ptr<Data> data_;
#endif
};
}

#endif //CNET_TIMEZONE_H
