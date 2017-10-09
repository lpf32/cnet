#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/Buffer.h>

using namespace cnet;
using namespace cnet::net;

void HttpRequest::appendToBuffer(Buffer *output) const
{
    char uri[256];
    snprintf(uri, sizeof uri, "%s %s HTTP/1.1", methodString(), path().c_str());
    output->append(uri);
    output->append("\r\n");

    for (std::map<string, string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
    {
        output->append(it->first + ":" + it->second);
        output->append("\r\n");
    }
    output->append("\r\n");
}