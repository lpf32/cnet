#ifndef CENT_DATE_H
#define CENT_DATE_H

#include <cnet/base/copyable.h>
#include <cnet/base/Types.h>

struct tm;

namespace cnet
{
class Date : public cnet::copyable {
    friend bool operator<(const Date&, const Date&);
    friend bool operator==(const Date&, const Date&);
public:
    struct YearMonthDay {
        int year;
        int month;
        int day;
    };

    static const int kDaysPerWeek = 7;
    static const int kJulianDayOf1970_01_01;

    /*!
     * Constructs an invalid Date
     */
    Date() : julianDayNumber_(0) {}

    /*!
     * Constructs a yyyy-mm-dd Date
     */

    Date(int year, int month, int day);

    /*!
     * Constructs a Date from Julian Day Number
     */
    explicit Date(int julianDayNum)
            : julianDayNumber_(julianDayNum)
    {}

    /*!
     * Constructs a Date from struct tm
     */
    explicit Date(const struct tm&);

    void swap(Date& that)
    {
        std::swap(julianDayNumber_, that.julianDayNumber_);
    }

    bool valid() const { return julianDayNumber_ > 0; }

    /*!
     * Converts to yyyy-mm-dd format
     */
    string toIsoString() const;

    struct YearMonthDay yearMonthDay() const;

    int year() const
    {
        return yearMonthDay().year;
    }

    int month() const
    {
        return yearMonthDay().month;
    }

    int day() const
    {
        return yearMonthDay().day;
    }

    // [0, 1, ..., 6] => [Sunday, Monday,...,Saturday]
    int weekDay() const
    {
        return (julianDayNumber_+1) % kDaysPerWeek;
    }

    int julianDayNumber() const { return julianDayNumber_; }

private:
    int julianDayNumber_;
};

inline bool operator<(const Date &lhs, const Date &rhs)
{
    return lhs.julianDayNumber() < rhs.julianDayNumber();
}

inline bool operator==(const Date &lhs, const Date &rhs)
{
    return lhs.julianDayNumber() == rhs.julianDayNumber();
}

}


#endif //CENT_DATE_H
