#include <stdio.h>
#include <assert.h>
#include <unistd.h>

#include <cnet/net/EventLoop.h>
#include <cnet/base/Thread.h>
#include <boost/bind.hpp>

using namespace cnet;
using namespace cnet::net;

EventLoop *g_loop;

void callback()
{
    printf("callback(): pid = %d, tid = %d\n", getpid(), CurrentThread::tid());
    EventLoop anotherLoop;
}

void threadFunc()
{
    printf("threadFunc(): pid = %d, tid = %d\n", getpid(), CurrentThread::tid());

    assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
    EventLoop loop;
    assert(EventLoop::getEventLoopOfCurrentThread() == &loop);
    loop.runAfter(1.0, callback);
    loop.loop();
}

int main()
{
    printf("main(): pid = %d, tid = %d\n", getpid(), CurrentThread::tid());

    {
        EventLoop loop;
        loop.quit();
        loop.loop();
    }

    assert(EventLoop::getEventLoopOfCurrentThread() == NULL);
    EventLoop loop;
    assert(EventLoop::getEventLoopOfCurrentThread() == &loop);

    Thread thread(threadFunc);
    thread.start();

    loop.loop();
}