#include <cnet/base/CurrentThread.h>
#include <cnet/base/Mutex.h>
#include <cnet/base/Thread.h>
#include <cnet/base/Timestamp.h>

#include <map>
#include <string>
#include <boost/bind.hpp>
#include <sys/wait.h>
#include <stdio.h>

using namespace cnet;

cnet::MutexLock g_mutex;
std::map<int, int> g_delays;

void threadFunc()
{
//    printf("tid=%d\n", cnet::CurrentThread::tid());
}

void threadFunc2(Timestamp start)
{
    Timestamp now(Timestamp::now());
    int delay = static_cast<int>(timeDifference(now, start) * 1000000);
    cnet::MutexLockGuard lock(g_mutex);
    ++g_delays[delay];
}

void forkBench()
{
    sleep(10);
    Timestamp start(Timestamp::now());
    int kProcesses = 10*1000;

    for (int i = 0; i < kProcesses; ++i)
    {
        pid_t child = fork();
        if (child == 0)
        {
            exit(0);
        } else {
            waitpid(child, NULL, 0);
        }
    }

    double timeUsed = timeDifference(Timestamp::now(), start);
    printf("process creation time used %F us\n", timeUsed*1000000/kProcesses);
    printf("number of created process %d\n", kProcesses);
}

int main(int argc, char *argv[])
{
    printf("pid=%d, tid=%d\n", ::getpid(), cnet::CurrentThread::tid());
    Timestamp start(Timestamp::now());

    int kThreads = 100 * 1000;
    for (int i = 0; i < kThreads; ++i) {
        Thread t1(threadFunc);
        t1.start();
        t1.join();
    }

    double timeUsed = timeDifference(Timestamp::now(), start);
    printf("thread creation time %F us\n", timeUsed*1000000/kThreads);
    printf("number of created threads %d\n", kThreads);

    for (int i = 0; i < kThreads; ++i) {
        Timestamp now(Timestamp::now());
        Thread t2(boost::bind(threadFunc2, now));
        t2.start();
        t2.join();
    }

    {
        cnet::MutexLockGuard lock(g_mutex);
        for (std::map<int, int>::iterator it = g_delays.begin();
                it != g_delays.end(); ++it)
        {
            printf("delay = %d, count = %d\n", it->first, it->second);
        }
    }

    forkBench();

    return 0;
}