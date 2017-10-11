#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/Buffer.h>
#include <cnet/net/InetAddress.h>

using namespace cnet;
using namespace cnet::net;


HttpRequest:: HttpRequest(Method method,
                         Version version,
                         HttpVersion httpVersion)
        : method_(method),
          version_(version),
          httpVersion_(httpVersion),
          serverAddr_()
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
    HttpRequest* request = new HttpRequest();
    request->setMethod(method);

    request->addHeader("Accept", "*");
    request->addHeader("Content-length", "0");

    const char* colon = std::find(url.begin(), url.end(), ':');
    uint16_t port = 80;
    string httpVersion(url.begin(), colon);

    if (httpVersion == "http")
    {
        port = 80;
        request->setHttpVersion(HttpRequest::kHttp);
    }
    else if (httpVersion == "https")
    {
        port = 443;
        request->setHttpVersion(HttpRequest::kHttps);
    }
    assert(port == 80 || port == 443);


    StringPiece uri(colon + 3, url.size());
    colon = std::find(uri.begin(), uri.end(), ':');
    const char* slant = std::find(uri.begin(), uri.end(), '/');
    string host;

    if (colon != uri.end())
    {
        port = static_cast<uint16_t>(atoi(string(colon+1, slant).c_str()));
        host.assign(uri.begin(), colon);
    }
    else
    {
        host.assign(uri.begin(), slant);
    }

    InetAddress serverAddr(port);
    assert(InetAddress::resolve(host, &serverAddr));
    request->serverAddr_ = serverAddr;

    request->addHeader("Host", host);

    if (slant == uri.end())
    {
        request->setPath("/");
    }
    else
    {
        request->setPath(string(slant, uri.end()));
    }

    return request;
}