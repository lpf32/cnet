#include <cnet/base/Exception.h>

#include <execinfo.h>
#include <stdlib.h>

using namespace cnet;

Exception::Exception(const char *what)
    : message_(what)
{
    fillStackTrace();
}

Exception::Exception(const string &what)
    : message_(what)
{
    fillStackTrace();
}

Exception::~Exception() nothrow
{ }

const char* Exception::what() const nothrow
{
    return message_.c_str();
}

const char* Exception::stackTrace() const nothrow
{
    return statck_.c_str();
}

void Exception::fillStackTrace()
{
    const int len = 200;
    void* buffer[len];
    int nptrs = ::backtrace(buffer, len);
    char **strings = ::backtrace_symbols(buffer, nptrs);
    if (strings)
    {
        for (int i = 0; i < nptrs; ++i) {
            // TODO demangle function name with abi::__cxa_demangle
            statck_.append(strings[i]);
            statck_.push_back('\n');
        }
        free(strings);
    }
}