#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/Buffer.h>
#include <cnet/net/InetAddress.h>

#include <cnet/base/Logging.h>

#include <boost/lexical_cast.hpp>

using namespace cnet;
using namespace cnet::net;

namespace
{
    const char *end_of_authority(const char *s, const char *eos)
    {
        while (s != eos)
        {
            if (*s == '?' || *s == '#' || *s == '/')
                return s;
            ++s;
        }
//        return std::find(s, eos, "?");
        return s;
    }

    int parse_port(const char *s, const char *eos)
    {
        try
        {
            return boost::lexical_cast<int>(s, eos-s);
        }catch (boost::bad_lexical_cast const&) {
            LOG_ERROR << "uri port error " << s;
            return -1;
        }
    }

    void parse_authority(HttpRequest::HttpUri &uri, const char *s, const char *eos)
    {
        const char* port = std::find(s, eos, ':');
        if (port == eos)
        {
            uri.port_ = -1;
        }
        else
        {
            uri.port_ = parse_port(port, eos);
        }
        uri.host_.assign(s, port);
    }
}


HttpRequest:: HttpRequest()
        : method_(kInvalid),
          version_(kUnknown),
          serverAddr_(),
          uri_()
{
}

void HttpRequest::appendToBuffer(Buffer *output) const
{
    char uri[256];
    snprintf(uri, sizeof uri, "%s %s HTTP/1.1", methodString(), path().c_str());
    output->append(uri);
    output->append("\r\n");

    for (std::map<string, string>::const_iterator it = headers_.begin(); it != headers_.end(); ++it)
    {
        output->append(it->first + ": " + it->second);
        output->append("\r\n");
    }
    output->append("\r\n");
}

HttpRequest* HttpRequest::parseURL(const StringPiece &url, const Method& method)
{
    HttpUri uri;
    uri.path_ = -1;

//    request->addHeader("Accept", "*");
//    request->addHeader("Content-length", "0");

    const char *colon = std::find(url.begin(), url.end(), ':');

    uri.setScheme(string(url.begin(), colon));

    assert(StringPiece(colon + 1, 2) == StringPiece("//"));

    const char *authority = colon + 3;
    const char *path = end_of_authority(authority, url.end());
    parse_authority(uri, authority, path);

    const char *query = std::find(path, url.end(), '?');
    const char *fragment = std::find(query, url.end(), '#');

    uri.path_.assign(path, query);
    uri.query_.assign(query, fragment);
    uri.fragment_.assign(fragment, url.end());

    if (uri.port_ == -1)
    {
        uri.port_ = (uri.scheme_ == kHttp) ? 80 : 443;
    }

    if (uri.path_ == "")
    {
        uri.path_.assign("/");
    }

    HttpRequest *request = new HttpRequest();
    request->setMethod(method);
    request->uri_ = uri;

    InetAddress addr(static_cast<uint16_t>(uri.port_));
    assert(InetAddress::resolve(uri.host_, &addr));
    request->serverAddr_ = addr;

    request->addHeader("Host", uri.host_);
    return request;
}