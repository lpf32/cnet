#ifndef CNET_HTTPCONTEXT_H
#define CNET_HTTPCONTEXT_H

#include <cnet/base/noncopyable.h>
#include <cnet/net/http/HttpRequest.h>

namespace cnet
{
namespace net
{

class Buffer;

class HttpContext : public cnet::copyable {
public:
    enum HttpRequestParseState {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpContext()
        : state_(kExpectRequestLine)
    {}

    // return false if any error
    bool parseRequest(Buffer *buf, Timestamp receiveTime);

    bool gotAll() const
    {
        return state_ == kGotAll;
    }

    void reset()
    {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    const HttpRequest& request() const
    { return request_; }

    HttpRequest& request()
    { return request_; }

private:
    bool processRequestLine(const char *begin, const char *end);

    HttpRequestParseState state_;
    HttpRequest request_;
};
}
}

#endif //CNET_HTTPCONTEXT_H
