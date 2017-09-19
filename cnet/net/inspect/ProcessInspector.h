#ifndef CNET_PROCESSINSPECTOR_H
#define CNET_PROCESSINSPECTOR_H

#include <cnet/net/inspect/Inspector.h>
#include <boost/noncopyable.hpp>

namespace cnet
{
namespace net
{
class ProcessInspector : boost::noncopyable
{
public:
    void registerCommands(Inspector *ins);

    static string overview(HttpRequest::Method, const Inspector::ArgList&);
    static string pid(HttpRequest::Method, const Inspector::ArgList&);
    static string procStatus(HttpRequest::Method, const Inspector::ArgList&);
    static string openedFiles(HttpRequest::Method, const Inspector::ArgList&);
    static string threads(HttpRequest::Method, const Inspector::ArgList&);

    static string username_;
};
}
}

#endif //CNET_PROCESSINSPECTOR_H
