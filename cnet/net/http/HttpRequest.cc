#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/Buffer.h>

using namespace cnet;
using namespace cnet::net;

void HttpRequest::appendToBuffer(Buffer *output) const
{
    char uri[256];
    snprintf(uri, sizeof uri, "%s %s HTTP/1.1", methodString(), path(), version_);
    output->append(uri);
    output->append("\r\n");
}