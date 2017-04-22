#ifndef CNET_EXCEPTION_H
#define CNET_EXCEPTION_H

#include <cnet/base/Types.h>
#include <exception>

#ifdef __GXX_EXPERIMENTAL_CXX0X_
#define nothrow noexcept
#else
#define nothrow throw()
#endif

namespace cnet
{
class Exception : public std::exception
{
public:
    explicit Exception(const char* what);
    explicit Exception(const string& what);
    virtual ~Exception() nothrow;
    virtual const char* what() const nothrow;
    const char* stackTrace() const nothrow;

private:
    void fillStackTrace();

    string message_;
    string statck_;
};
}

#endif //CNET_EXCEPTION_H
