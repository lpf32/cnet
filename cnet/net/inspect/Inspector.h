#ifndef CNET_INSPECTOR_H
#define CNET_INSPECTOR_H

#include <cnet/base/Mutex.h>
#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/http/HttpServer.h>

#include <map>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>

namespace cnet
{
namespace net
{

class ProcessInspector;
class PerformanceInspector;
class SystemInspector;

class Inspector : boost::noncopyable
{
public:
    typedef std::vector<string> ArgList;
    typedef boost::function<string (HttpRequest::Method, const ArgList& args)> Callback;
    Inspector(EventLoop* loop, const InetAddress& httpAddr, const string& name);
    ~Inspector();

    /**
     * Add a Callback for handling the special uri:/mudule/command
     * @param module
     * @param command
     * @param cb
     * @param help
     */
    void add(const string& module, const string& command, const Callback& cb, const string& help);
    void remove(const string& module, const string& command);

private:
    typedef std::map<string, Callback> CommandList;
    typedef std::map<string, string> HelpList;

    void start();
    void onRequest(const HttpRequest& req, HttpResponse *resp);

    HttpServer server_;
    boost::scoped_ptr<ProcessInspector> processInspector_;
    boost::scoped_ptr<PerformanceInspector> performanceInspector_;
    boost::scoped_ptr<SystemInspector> systemInspector_;

    MutexLock mutex_;
    std::map<string, CommandList> modules_;
    std::map<string, HelpList> helps_;
};
}
}

#endif //CNET_INSPECTOR_H
