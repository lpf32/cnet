#include <cnet/base/Logging.h>
#include <cnet/base/LogFile.h>
#include <cnet/base/ThreadPool.h>
#include <cnet/base/TimeZone.h>

#include <stdio.h>
#include <boost/noncopyable.hpp>

int g_total;
FILE* g_file;
boost::scoped_ptr<cnet::LogFile> g_logFile;

void dummyOutput(const char* msg, int len)
{
    g_total += len;
    if(g_file)
    {
        fwrite(msg, 1, len, g_file);
    }
    else if (g_logFile)
    {
        g_logFile->append(msg, len);
    }
}

void bench(const char* type)
{
    cnet::Logger::setOutput(dummyOutput);
    cnet::Timestamp start(cnet::Timestamp::now());
    g_total = 0;

    int n = 1000*1000;
    const bool kLongLog = false;
    cnet::string empty  = " ";
    cnet::string logStr(3000, 'X');
    logStr += " ";
    for (int i = 0; i < n; ++i) {
        LOG_INFO << "Hello 0123456789" << "abcdefjghqjkimnoupxyz"
                                       << (kLongLog ? logStr : empty)
                                        << i ;
    }

    cnet::Timestamp end(cnet::Timestamp::now());
    double seconds = cnet::timeDifference(end, start);
    printf("%12s: %f seconds, %d bytes, %10.2f msg/s, %.2f MiB/s\n",
        type, seconds, g_total, n / seconds, g_total/seconds/(1024*1024));
}

void logInThread()
{
    LOG_INFO << "logInThread";
    usleep(1000);
}

int main()
{
    getppid();

    cnet::ThreadPool pool("pool");
    pool.start(5);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);
    pool.run(logInThread);

    LOG_TRACE << "tarce";
    LOG_DEBUG << "debug";
    LOG_INFO << "hello";
    LOG_WARN << "World";
    LOG_ERROR << "Error";
    LOG_INFO << sizeof(cnet::Logger);
    LOG_INFO << sizeof(cnet::LogStream);
    LOG_INFO << sizeof(cnet::Fmt);
    LOG_INFO << sizeof(cnet::LogStream::Buffer);

    // cent::noncopyable add a entry of virtual table than boost::noncopyable
    LOG_INFO << sizeof(cnet::noncopyable);
    LOG_INFO << sizeof(boost::noncopyable);

    // bench will change default output function
    sleep(1);
    bench("nop");

    char buffer[64*1024];
    g_file = fopen("/dev/null", "w");
    setbuffer(g_file, buffer, sizeof buffer);
    bench("/dev/null");
    fclose(g_file);

    g_file = fopen("/tmp/log", "w");
    setbuffer(g_file, buffer, sizeof buffer);
    bench("/tmp/log");
    fclose(g_file);

    g_file = NULL;
    g_logFile.reset(new cnet::LogFile("test_log_st", 500*1000*1000, false));
    bench("test_log_st");

    g_file = NULL;
    g_logFile.reset(new cnet::LogFile("test_log_mt", 500*1000*1000, true));
    bench("test_log_mt");
    g_logFile.reset();

    {
        g_file = stdout;
        sleep(1);
        cnet::TimeZone beijing(8*3600, "CST");
        cnet::Logger::setTimeZone(beijing);
        LOG_TRACE << "trace CST";
        LOG_DEBUG << "debug CST";
        LOG_INFO << "Hello CST";
        LOG_WARN << "World CST";
        LOG_ERROR << "ERROR CST";

        sleep(1);
        cnet::TimeZone newyork("/usr/share/zoneinfo/America/New_York");
        cnet::Logger::setTimeZone(newyork);
        LOG_TRACE << "trace CST";
        LOG_DEBUG << "debug CST";
        LOG_INFO << "Hello CST";
        LOG_WARN << "World CST";
        LOG_ERROR << "ERROR CST";
        g_file = NULL;
    }
    bench("timezone nop");
}