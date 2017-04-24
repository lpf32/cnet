#ifndef CNET_PROCESSINFO_H
#define CNET_PROCESSINFO_H

#include <cnet/base/StringPiece.h>
#include <cnet/base/Types.h>
#include <cnet/base/Timestamp.h>
#include <vector>

namespace cnet
{

namespace ProcessInfo
{
    pid_t pid();
    string pidString();
    uid_t uid();
    string username();
    uid_t euid();
    Timestamp startTime();
    int clockTicksPerSecond();
    int pageSize();
    bool isDebugBuild(); // constexpr

    string hostname();
    string procname();
    StringPiece procname(const string& stat);

    /// read /proc/self/status
    string procStatus();


    /// read /proc/self/stat
    string procStat();

    /// read /proc/self/task/tid/stat
    string threadStat();

    /// readlink /proc/self/exe
    string exePath();

    int openedFiles();
    int maxOpenFiles();

    struct CpuTime
    {
        double userSeconds;
        double systemSeconds;

        CpuTime() : userSeconds(0.0), systemSeconds(0.0) { }
    };
    CpuTime cpuTime();

    int numThreads();
    std::vector<pid_t> threads();
}
}


#endif //CNET_PROCESSINFO_H
