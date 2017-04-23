#include <cnet/base/FileUtil.h>

#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <cnet/base/Thread.h>


using namespace cnet;

void read()
{
    string result;
    int64_t size = 0;
    int err;
    err = FileUtil::readFile("/proc/self", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/proc/self", 1024, &result, NULL);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/proc/self/cmdline", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/dev/null", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/dev/zero", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/notexist", 1024, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/dev/zero", 102400, &result, &size);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
    err = FileUtil::readFile("/dev/zero", 102400, &result, NULL);
    printf("%d %zd %" PRIu64 "\n", err, result.size(), size);
}

void threadFunc()
{
    FileUtil::AppendFile file("/tmp/log.log");
    string str = "hello world!";
    file.append(str.c_str(), str.size());
}

void write()
{
    int kThreads = 10;
    boost::ptr_vector<Thread> threads;
    for (int i = 0; i < kThreads; ++i) {
        threads.push_back(new Thread(threadFunc));
        threads.back().start();
    }

    for (int i = 0; i < kThreads; ++i) {
        threads[i].join();
    }
}

int main()
{
    read();
    write();
}