#include <cnet/net/EventLoopThreadPool.h>
#include <cnet/net/EventLoop.h>
#include <cnet/base/Thread.h>

#include <boost/bind.hpp>

#include <stdio.h>

using namespace cnet;
using namespace cnet::net;

void print(EventLoop *p = NULL)
{
    printf("main(): pid = %d, tid = %d, loop = %p\n",
        getpid(), CurrentThread::tid(), p);
}

void init(EventLoop* p)
{
    printf("init(): pid = %d, tid = %d, loop = %p\n",
           getpid(), CurrentThread::tid(), p);
}

int main()
{
    print();

    EventLoop loop;
    loop.runAfter(11, boost::bind(&EventLoop::quit, &loop));

    {
        printf("Single thread %p:\n", &loop);
        EventLoopThreadPool model(&loop, "single");
        model.setThreadNum(0);
        model.start(init);
        assert(model.getNextLoop() == &loop);
        assert(model.getNextLoop() == &loop);
        assert(model.getNextLoop() == &loop);
    }

    {
        printf("Another thread:\n");
        EventLoopThreadPool model(&loop, "another");
        model.setThreadNum(1);
        model.start(init);
        EventLoop *nextLoop = model.getNextLoop();
        nextLoop->runAfter(2, boost::bind(print, nextLoop));
        assert(nextLoop == model.getNextLoop());
        assert(nextLoop == model.getNextLoop());
        ::sleep(3);
    }

    {
        printf("three thread:\n");
        EventLoopThreadPool model(&loop, "three");
        model.setThreadNum(3);
        model.start(init);
        EventLoop *nextLoop = model.getNextLoop();
        nextLoop->runInLoop(boost::bind(print, nextLoop));
        assert(nextLoop != &loop);
        assert(nextLoop != model.getNextLoop());
        assert(nextLoop != model.getNextLoop());
        assert(nextLoop == model.getNextLoop());
    }
    loop.loop();
}