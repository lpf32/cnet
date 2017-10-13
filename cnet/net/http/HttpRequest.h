#ifndef CNET_HTTPREQUEST_H
#define CNET_HTTPREQUEST_H

#include <cnet/base/copyable.h>
#include <cnet/base/Timestamp.h>
#include <cnet/base/Types.h>

#include <cnet/net/InetAddress.h>

#include <map>
#include <assert.h>
#include <stdio.h>
#include <cnet/base/StringPiece.h>

namespace cnet
{
namespace net
{

class Buffer;

class HttpRequest: public cnet::copyable
{
public:
    enum Method
    {
        kInvalid, kGet, kPost, kHead, kPut, kDelete
    };
    enum Version
    {
        kUnknown, kHttp10, kHttp11
    };
    enum Scheme
    {
        kInvalidScheme, kHttp, kHttps
    };

    struct HttpUri
    {
        void setScheme(const string &version)
        {
            if (version == "http")
                scheme_ = kHttp;
            else if (version == "http")
                scheme_ = kHttps;
        }

        void setScheme(const Scheme& scheme)
        {
            scheme_ = scheme;
        }


        Scheme scheme() const
        {
            return scheme_;
        }

        int flags_;
        Scheme scheme_;
        string userinfo_;
        string host_;
        int port_;
        string path_;
        string query_;
        string fragment_;
    };

    HttpRequest();

    void setVersion(Version v)
    {
        version_ = v;
    }

    const InetAddress &getServerAddr()
    {
        return serverAddr_;
    }

    Version getVersion() const { return version_; }

    bool setMethod(const char *start, const char *end)
    {
        assert(method_ == kInvalid);
        string m(start, end);
        if (m == "GET")
            method_ = kGet;
        else if (m == "POST")
            method_ = kPost;
        else if (m == "HEAD")
            method_ = kHead;
        else if (m == "PUT")
            method_ = kPut;
        else if (m == "DELETE")
            method_ = kDelete;
        else
            method_ = kInvalid;

        return method_ != kInvalid;
    }

    void setScheme(const string &scheme)
    {
        if (scheme == "http")
            uri_.scheme_ = kHttp;
        else if (scheme == "http")
            uri_.scheme_ = kHttps;
    }

    void setScheme(const Scheme& scheme)
    {
        uri_.scheme_ = scheme;
    }

    Method method() const
    {
        return method_;
    }

    Scheme scheme() const
    {
        return uri_.scheme_;
    }

    const char* schemeString() const
    {
        const char *result = "UNKNOWN";
        switch (uri_.scheme_)
        {
            case kHttp:
                result = "HTTP";
                break;
            case kHttps:
                result = "HTTPS";
                break;
            default:
                break;
        }
        return result;
    }

    const char *methodString() const
    {
        const char *result = "UNKNOWN";
        switch (method_)
        {
            case kGet:
                result = "GET";
                break;
            case kPost:
                result = "POST";
                break;
            case kHead:
                result = "HEAD";
                break;
            case kPut:
                result = "PUT";
                break;
            case kDelete:
                result = "DELETE";
                break;
            default:
                break;

        }
        return result;
    }

    const char *versionString() const
    {
        const char *result = "UNKNOWN";
        switch (version_)
        {
            case kHttp10:
                result = "HTTP/1.0";
                break;
            case kHttp11:
                result = "HTTP/1.1";
                break;
            default:
                break;

        }
        return result;
    }

    void setPath(const char *start, const char *end)
    {
        uri_.path_.assign(start, end);
    }

    void setPath(const string &path)
    {
        uri_.path_ = path;
    }

    void setMethod(const StringPiece &method)
    {
        setMethod(method.begin(), method.end());
    }

    void setMethod(const Method &method)
    {
        method_ = method;
    }

    const string& path() const
    { return uri_.path_; }

    void setQuery(const char* start, const char* end)
    {
        uri_.query_.assign(start, end);
    }

    const string& query() const
    {
        return uri_.query_;
    }

    void setReceiveTime(Timestamp t)
    { receiveTime_ = t;}

    Timestamp receiveTime() const
    { return receiveTime_; }

    void addHeader(const char* start, const char* colon, const char* end)
    {
        string field(start, colon);
        ++colon;
        while (colon < end && isspace(*colon))
        {
            ++colon;
        }
        string value(colon, end);
        while (!value.empty() && isspace(value[value.size()-1]))
        {
            value.resize(value.size()-1);
        }
        headers_[field] = value;
    }

    void addHeader(const string& field, const string& value)
    {
        headers_[field] = value;
    }

    string getHeader(const string& field) const
    {
        string result;
        std::map<string, string>::const_iterator it = headers_.find(field);
        if (it != headers_.end())
        {
            result = it->second;
        }
        return result;
    }

    const std::map<string, string>& headers() const
    { return headers_; }

    void swap(HttpRequest& that)
    {
        std::swap(method_, that.method_);
        std::swap(version_, that.version_);
        uri_.path_.swap(that.uri_.path_);
        uri_.query_.swap(that.uri_.query_);
        receiveTime_.swap(that.receiveTime_);
        headers_.swap(that.headers_);
    }


    void appendToBuffer(Buffer* output) const;

    static HttpRequest* parseURL(const StringPiece& url, const Method& method);

private:
    Method method_;
    Version version_;
    Timestamp receiveTime_;
    InetAddress serverAddr_;
    HttpUri uri_;
    std::map<string, string> headers_;
};
}
}

#endif //CNET_HTTPREQUEST_H
