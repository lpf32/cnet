#include "plot.h"

#include <cnet/base/FileUtil.h>
#include <cnet/base/ProcessInfo.h>
#include <cnet/net/EventLoop.h>
#include <cnet/net/http/HttpRequest.h>
#include <cnet/net/http/HttpResponse.h>
#include <cnet/net/http/HttpServer.h>

#include <boost/algorithm/string/replace.hpp>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/type_traits/is_pod.hpp>


#include <sstream>
#include <stdarg.h>
#include <stdio.h>

using namespace cnet;
using namespace cnet::net;

/**
 *  用于监控进程
 */


// Represent parsed /proc/pid/stat
struct StatData
{
    void parse(const char* startAtState, int kbPerPage)
    {
        // istringstream is probably not the most efficient way to parse it,
        // see muduo-protorpc/examples/collect/ProcFs.cc for alternatives.
        std::istringstream iss(startAtState);

        //            0    1    2    3     4    5       6   7 8 9  11  13   15
        // 3770 (cat) R 3718 3770 3718 34818 3770 4202496 214 0 0 0 0 0 0 0 20
        // 16  18     19      20 21                   22      23      24              25
        //  0 1 0 298215 5750784 81 18446744073709551615 4194304 4242836 140736345340592
        //              26
        // 140736066274232 140575670169216 0 0 0 0 0 0 0 17 0 0 0 0 0 0

        iss >> state;
        iss >> ppid >> pgrp >> session >> tty_nr >> tpgid >> flags;
        iss >> minflt >> cminflt >> majflt >> cmajflt;
        iss >> utime >> stime >> cutime >> cstime;
        iss >> priority >> nice >> num_threads >> itrealvalue >> starttime;
        long vsize, rss;
        iss >> vsize >> rss >> rsslim;
        vsizeKb = vsize / 1024;
        rssKb = rss * kbPerPage;
    }
    // int pid;
    char state;
    int ppid;
    int pgrp;
    int session;
    int tty_nr;
    int tpgid;
    int flags;

    long minflt;
    long cminflt;
    long majflt;
    long cmajflt;

    long utime;
    long stime;
    long cutime;
    long cstime;

    long priority;
    long nice;
    long num_threads;
    long itrealvalue;
    long starttime;

    long vsizeKb;
    long rssKb;
    long rsslim;
};

class Procmon: boost::noncopyable
{
public:
    Procmon(EventLoop *loop, pid_t pid, uint16_t port, const char *procname)
            : kClockTicksPerSecond_(cnet::ProcessInfo::clockTicksPerSecond()),
              kbPerPage_(cnet::ProcessInfo::pageSize() / 1024),
              kBootTime_(getBootTime()),
              pid_(pid),
              server_(loop, InetAddress(port), getName()),
              procname_(ProcessInfo::procname(readProcFile("stat")).as_string()),
              hostname_(ProcessInfo::hostname()),
              cmdline_(getCmdLine()),
              ticks_(0),
              cpu_usage_(600 / kPeriod_),
              cpu_chart_(640, 100, 600, kPeriod_),
              ram_char_(640, 100, 7200, 30)
    {
        bzero(&lastStatDate_, sizeof(lastStatDate_));
        server_.setHttpCallback(boost::bind(&Procmon::onRequest, this, _1, _2));
    }

    void start()
    {
        tick();
        server_.getLoop()->runEvery(kPeriod_, boost::bind(&Procmon::tick, this));
        server_.start();

    }

    ~Procmon() {}

private:
    string getName() const
    {
        char name[256];
        snprintf(name, sizeof name, "procmon-%d", pid_);
        return name;
    }

    static long getBootTime()
    {
        string stat;
        FileUtil::readFile("/proc/stat", 65536, &stat);
        return getLong(stat, "btime ");
    }

    void tick()
    {
        string stat = readProcFile("stat");
        if (stat.empty())
            return;
        StringPiece procname = ProcessInfo::procname(stat);
        StatData statData;
        bzero(&statData, sizeof statData);
        statData.parse(procname.end()+1, kbPerPage_);
        if (ticks_ > 0)
        {
            CpuTime time;
            time.userTime_ = std::max(0, static_cast<int>(statData.utime - lastStatDate_.utime));
            time.sysTime_ = std::max(0, static_cast<int>(statData.stime - lastStatDate_.stime));
            cpu_usage_.push_back(time);
        }

        lastStatDate_ = statData;
        ++ticks_;
    }

    static long getLong(const string &status, const char *key)
    {
        long result = 0;
        size_t pos = status.find(key);
        if (pos != string::npos)
        {
            result = ::atoi(status.c_str() + pos + strlen(key));
        }
        return result;
    }

    void onRequest(const HttpRequest &req, HttpResponse *resp)
    {
        resp->setStatusCode(HttpResponse::k200OK);
        resp->setStatusMessage("OK");
        resp->addHeader("Server", "Cnet-Procmon");

        if (req.path() == "/")
        {
            resp->setContentType("text/html");
            fillOverview(req.query());
            resp->setBody(response_.retrieveAllAsString());
        } else if (req.path() == "/cmdline")
        {
            resp->setBody(cmdline_);
        } else if (req.path() == "/cpu.png")
        {
            std::vector<double> cpu_usage;
            for (size_t i = 0; i < cpu_usage_.size(); ++i)
            {
                cpu_usage.push_back(cpu_usage_[i].cpuUsage(kPeriod_, kClockTicksPerSecond_));
            }

            string png = cpu_chart_.plotCpu(cpu_usage);
            resp->setContentType("image/png");
            resp->setBody(png);
        }
    }

    void fillOverview(const string &query)
    {
        response_.retrieveAll();
        Timestamp now = Timestamp::now();
        appendResponse("<html><head><title>%s on %s</title>\n",
                       procname_.c_str(), hostname_.c_str());
        fillRefresh(query);
        appendResponse("</head><body>\n");

        string stat = readProcFile("stat");
        if (stat.empty())
        {
            appendResponse("<h1>PID %d doesn't exist.</h1></body></html>", pid_);
            return;
        }

        int pid = atoi(stat.c_str());
        assert(pid == pid_);
        StringPiece procname = ProcessInfo::procname(stat);
        appendResponse("<h1>%s on %s</h1>",
                       procname.as_string().c_str(), hostname_.c_str());
        response_.append("<p>Refresh <a href=\"?refresh=1\">1s</a> ");
        response_.append("<a href=\"?refresh=2\">2s</a> ");
        response_.append("<a href=\"?refresh=5\">5s</a> ");
        response_.append("<a href=\"?refresh=15\">15s</a> ");
        response_.append("<a href=\"?refresh=60\">60s</a>\n");
        response_.append("<p><a href=\"/cmdline\">Command line</a>\n");
        response_.append("<a href=\"/environ\">Environment variables</a>\n");
        response_.append("<a href=\"/threads\">Threads</a>\n");

        appendResponse("<p>Page generated at %s (UTC)", now.toFormattedString().c_str());

        response_.append("<p><table>");
        StatData statData;
        bzero(&statData, sizeof statData);
        statData.parse(procname.end() + 1, kbPerPage_);

        appendTableRow("PID", pid);
        Timestamp started(getStartTime(statData.starttime));
        appendTableRow("Started at", started.toFormattedString(false) + " (UTC)");
        appendTableRowFloat("Uptime (s)", timeDifference(now, started));  // FIXME: format as days+H:M:S
        appendTableRow("Executable", readLink("exe"));
        appendTableRow("Current dir", readLink("cwd"));


        appendTableRow("State", getState(statData.state));
        appendTableRowFloat("User time (s)", getSeconds(statData.utime));
        appendTableRowFloat("System time (s)", getSeconds(statData.stime));

        appendTableRow("VmSize (KiB)", statData.vsizeKb);
        appendTableRow("VmRSS (KiB)", statData.rssKb);
        appendTableRow("Threads", statData.num_threads);
        appendTableRow("CPU usage", "<img src=\"/cpu.png\" height=\"100\" witdh=\"640\">");

        appendTableRow("Priority", statData.priority);
        appendTableRow("Nice", statData.nice);

        appendTableRow("Minor page faults", statData.minflt);
        appendTableRow("Major page faults", statData.majflt);
        // TODO: user

        response_.append("</table>");
        response_.append("</body></html>");
    }

    void fillRefresh(const string &query)
    {
        size_t p = query.find("refresh=");
        if (p != string::npos)
        {
            int seconds = atoi(query.c_str() + p + 8);
            if (seconds > 0)
            {
                appendResponse("<meta http-equiv=\"refresh\" content=\"%d\">\n", seconds);
            }
        }
    }

    double getSeconds(long ticks)
    {
        return static_cast<double>(ticks) / kClockTicksPerSecond_;
    }

    static const char *getState(char state)
    {
        // One character from the string "RSDZTW" where R is running, S is sleeping in
        // an interruptible wait, D is waiting in uninterruptible disk sleep, Z is zombie,
        // T is traced or stopped (on a signal), and W is paging.
        switch (state)
        {
            case 'R':
                return "Running";
            case 'S':
                return "Sleeping";
            case 'D':
                return "Disk sleep";
            case 'Z':
                return "Zombie";
            default:
                return "Unknown";
        }
    }

    string readLink(const char *basename)
    {
        char filename[256];
        snprintf(filename, sizeof filename, "/proc/%d/%s", pid_, basename);
        char link[1024];
        ssize_t len = ::readlink(filename, link, sizeof link);
        string result;
        if (len > 0)
        {
            result.assign(link, len);
        }
        return result;
    }

    int appendResponse(const char *fmt, ...) __attribute__((format (printf, 2, 3)));

    void appendTableRow(const char *name, long value)
    {
        appendResponse("<tr><td>%s</td><td>%ld</td></tr>\n", name, value);
    }

    void appendTableRowFloat(const char *name, double value)
    {
        appendResponse("<tr><td>%s</td><td>%.2f</td></tr>\n", name, value);
    }

    void appendTableRow(const char *name, StringArg value)
    {
        appendResponse("<tr><td>%s</td><td>%s</td></tr>\n", name, value.c_str());
    }

    string getCmdLine()
    {
        return boost::replace_all_copy(readProcFile("cmdline"), string(1, '\0'), "\n\t");
    }

    Timestamp getStartTime(long starttime)
    {
        return Timestamp(Timestamp::kMicroSecondsPerSecond * kBootTime_
                         + Timestamp::kMicroSecondsPerSecond * starttime / kClockTicksPerSecond_);
    }

    string readProcFile(const char *basename)
    {
        char filename[256];
        snprintf(filename, sizeof filename, "/proc/%d/%s", pid_, basename);
        string content;
        FileUtil::readFile(filename, 1024*1024, &content);
        return content;
    }

    struct CpuTime
    {
        int userTime_;
        int sysTime_;
        double cpuUsage(double kPeriod, double kClockTicksPerSecond) const
        {
            return (userTime_ + sysTime_) / (kClockTicksPerSecond * kPeriod);
        }
    };
private:
    const static int kPeriod_ = 2.0;
    const int kClockTicksPerSecond_;
    const int kbPerPage_;
    const long kBootTime_;
    const pid_t pid_;
    HttpServer server_;
    const string procname_;
    const string hostname_;
    const string cmdline_;
    int ticks_;
    StatData lastStatDate_;
    boost::circular_buffer<CpuTime> cpu_usage_;
    Plot cpu_chart_;
    Plot ram_char_;
    Buffer response_;
};

int Procmon::appendResponse(const char *fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    int ret = vsnprintf(buf, sizeof buf, fmt, args);
    va_end(args);
    response_.append(buf);
    return ret;
}

bool processExists(pid_t pid)
{
    char filename[256];
    snprintf(filename, sizeof filename, "/proc/%d/stat", pid);
    return ::access(filename, R_OK) == 0;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Usage: %s pid port [name]\n", argv[0]);
        return 0;
    }
    int pid = atoi(argv[1]);
    if (!processExists(pid))
    {
        printf("Process %d doesn't exist.\n", pid);
        return 1;
    }

    EventLoop loop;
    uint16_t port = static_cast<uint16_t>(atoi(argv[2]));
    Procmon procmon(&loop, pid, port, argc > 3? argv[3] : "");
    procmon.start();
    loop.loop();
}