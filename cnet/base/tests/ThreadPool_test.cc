#include <cnet/base/ThreadPool.h>
#include <cnet/base/CountDownLatch.h>
#include <cnet/base/CurrentThread.h>
#include <cnet/base/Logging.h>
#include <cnet/base/Types.h>

#include <boost/bind.hpp>
#include <stdio.h>

using namespace cnet;

void print()
{
    printf("tid=%d\n", cnet::CurrentThread::tid());
}

void printString(const std::string& str)
{
    LOG_INFO << str;
    usleep(100*1000);
}

void test(int maxSize)
{
    LOG_WARN << "Test ThreadPool with max queue size = " << maxSize;
    cnet::ThreadPool pool("MainThreadPool");
    pool.setMaxQueueSize(maxSize);
    pool.start(5);

    LOG_WARN << "Adding";
    pool.run(print);
    pool.run(print);
    for (int i = 0; i < 100; ++i) {
        char buf[32];
        snprintf(buf, sizeof buf, "task %d", i);
        pool.run(boost::bind(printString, std::string(buf)));
    }
    LOG_WARN << "Done";

    cnet::CountDownLatch latch(1);
    pool.run(boost::bind(&CountDownLatch::countDown, &latch));
    latch.wait();
    pool.stop();
}

int main()
{
    test(0);
    test(1);
    test(5);
    test(10);
    test(50);
}