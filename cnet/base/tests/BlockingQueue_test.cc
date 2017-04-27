#include <cnet/base/BlockingQueue.h>
#include <cnet/base/CountDownLatch.h>
#include <cnet/base/Thread.h>

#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <memory>
#include <string>
#include <algorithm>

#include <stdio.h>
#include <unistd.h>

class Test
{
public:
    Test(int numThreads)
        : latch_(numThreads),
          threads_(numThreads)
    {
        for (int i = 0; i < numThreads; ++i) {
            char name[32];
            snprintf(name, sizeof name, "work thread %d", i);
            threads_.push_back(new cnet::Thread(
                    boost::bind(&Test::threadFunc, this), cnet::string(name)
            ));
        }
        std::for_each(threads_.begin(), threads_.end(), boost::bind(&cnet::Thread::start, _1));
    }

    void run(int times)
    {
        printf("waiting for count down latch\n");
        latch_.wait();
        printf("all threads started\n");
        for (int i = 0; i < times; ++i) {
            char buf[32];
            snprintf(buf, sizeof buf, "hello %d", i);
            queue_.put(buf);
            printf("tid = %d, put data = %s, size = %zd\n",
                cnet::CurrentThread::tid(), buf, queue_.size());
        }
    }

    void joinAll()
    {
        for (size_t i = 0; i < threads_.size(); ++i) {
            queue_.put("stop");
        }
        std::for_each(threads_.begin(), threads_.end(),
                      boost::bind(&cnet::Thread::join, _1));
    }

private:
    void threadFunc()
    {
        printf("tid=%d, %s started\n",
            cnet::CurrentThread::tid(),
            cnet::CurrentThread::name());
        latch_.countDown();
        bool running = true;
        while (running)
        {
            std::string d(queue_.take());
            printf("tid=%d, get data = %s, size = %zd\n", cnet::CurrentThread::tid(),
                    d.c_str(), queue_.size());
            running = (d != "stop");
        }
        printf("tid=%d, %s stopped\n",
            cnet::CurrentThread::tid(),
            cnet::CurrentThread::name());
    }
    cnet::BlockingQueue<std::string> queue_;
    cnet::CountDownLatch latch_;
    boost::ptr_vector<cnet::Thread> threads_;
};

void testMove() {
#ifdef __GXX_EXPERIMENTAL_CXX0X__
#if __GNUC_PREREQ(4,4)
    cnet::BlockingQueue<std::unique_ptr<int>> queue;
    queue.put(std::unique_ptr<int>(new int(42)));
    std::unique_ptr<int> x= queue.take();
    printf("took %d, size %zd\n", *x, queue.size());
    *x = 123;
    queue.put(std::move(x));
    std::unique_ptr<int> y = queue.take();
    printf("took %d, size %zd\n", *y, queue.size());
#endif
#endif
}

    int main()
{
    printf("pid = %d, tid = %d\n", ::getpid(), cnet::CurrentThread::tid());
    Test t(5);
    t.run(100);
    t.joinAll();

    testMove();

    printf("number of created threads %d\n", cnet::Thread::numCreated());
}