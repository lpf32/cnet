#ifndef CNET_SYSTEMINSPECTOR_H
#define CNET_SYSTEMINSPECTOR_H

#include <cnet/net/inspect/Inspector.h>

namespace cnet
{
    namespace net
    {
        class SystemInspector : boost::noncopyable
        {
        public:
            void registerCommands(Inspector* ins);

            static string overview(HttpRequest::Method, const Inspector::ArgList&);
            static string loadavg(HttpRequest::Method, const Inspector::ArgList&);
            static string version(HttpRequest::Method, const Inspector::ArgList&);
            static string cpuinfo(HttpRequest::Method, const Inspector::ArgList&);
            static string meminfo(HttpRequest::Method, const Inspector::ArgList&);
            static string stat(HttpRequest::Method, const Inspector::ArgList&);
        };
    }
}


#endif //CNET_SYSTEMINSPECTOR_H
