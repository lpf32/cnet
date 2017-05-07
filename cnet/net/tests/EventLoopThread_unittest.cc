#include <cnet/net/EventLoopThread.h>
#include <cnet/net/EventLoop.h>
#include <cnet/base/Thread.h>
#include <cnet/base/CountDownLatch.h>

#include <boost/bind.hpp>

#include <stdio.h>

using namespace cnet;
using namespace cnet::net;

void print (EventLoop *p = NULL)
{
    printf("print: pid = %d, tid = %d, loop = %p\n",
        getpid(), CurrentThread::tid(), p);
}

void quit(EventLoop *p)
{
    print(p);
    p->quit();
}

int main()
{
    print();
    {
        EventLoopThread thr1;
    }

    {
        // dtor calls quit()
        EventLoopThread thr2;
        EventLoop *loop = thr2.startLoop();
        loop->runInLoop(boost::bind(print, loop));
        CurrentThread::sleepUsec(500*1000);
    }

    {
        // quti() before dtor
        EventLoopThread thr3;
        EventLoop *loop = thr3.startLoop();
        loop->runInLoop(boost::bind(quit, loop));
        CurrentThread::sleepUsec(500*1000);
    }

    // FIXME test dtor before thread start return
}