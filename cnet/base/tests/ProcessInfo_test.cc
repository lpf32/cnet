#include <cnet/base/ProcessInfo.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

using namespace cnet;

int main()
{
    printf("pid = %d\n", ProcessInfo::pid());
    printf("uid = %d\n", ProcessInfo::uid());
    printf("euid = %d\n", ProcessInfo::euid());
    printf("start time = %s\n", ProcessInfo::startTime().toFormattedString().c_str());
    printf("hostname = %s\n", ProcessInfo::hostname().c_str());
    printf("opened files = %d\n", ProcessInfo::openedFiles());
    printf("threads = %zd\n", ProcessInfo::threads().size());
    printf("num threads = %d\n", ProcessInfo::numThreads());
    printf("status = %s\n", ProcessInfo::procStatus().c_str());
}