#ifndef CNET_PERFORMANCEINSPECTOR_H
#define CNET_PERFORMANCEINSPECTOR_H

#include <cnet/net/inspect/Inspector.h>
#include <boost/noncopyable.hpp>

namespace cnet
{
namespace net
{

    class PerformanceInspector : boost::noncopyable
    {
    public:
        void registerCommands(Inspector *ins);

        static string heap(HttpRequest::Method, const Inspector::ArgList &);

        static string growth(HttpRequest::Method, const Inspector::ArgList &);

        static string profile(HttpRequest::Method, const Inspector::ArgList &);

        static string cmdline(HttpRequest::Method, const Inspector::ArgList &);

        static string memstats(HttpRequest::Method, const Inspector::ArgList &);

        static string memhistogram(HttpRequest::Method, const Inspector::ArgList &);

        static string releaseFreeMemory(HttpRequest::Method, const Inspector::ArgList &);

        static string symbol(HttpRequest::Method, const Inspector::ArgList &);
    };
}
}

#endif //CNET_PERFORMANCEINSPECTOR_H
