#include <cnet/base/BlockingQueue.h>
#include <cnet/base/CountDownLatch.h>
#include <cnet/base/Thread.h>
#include <cnet/base/Timestamp.h>


#include <boost/bind.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

#include <map>
#include <string>
#include <stdio.h>

class Bench
{
public:
    Bench(int numThreads)
        : latch_(numThreads),
          threads_(numThreads)
    {
        for (int i = 0; i < numThreads; ++i) {
            char name[32];
            snprintf(name, sizeof name, "work thread %d", i);
            threads_.push_back(new cnet::Thread(
                    boost::bind(&Bench::threadFunc, this), cnet::string(name)
            ));
        }
        std::for_each(threads_.begin(), threads_.end(),
            boost::bind(&cnet::Thread::start, _1));
    }

    void run(int times)
    {
        printf("waiting for count down latch\n");
        latch_.wait();
        printf("all threads started\n");
        for (int i = 0; i < times; ++i) {
            cnet::Timestamp now(cnet::Timestamp::now());
            queue_.put(now);
            usleep(1000);
        }
    }

    void joinAll()
    {
        for (size_t i = 0; i < threads_.size(); ++i) {
            queue_.put(cnet::Timestamp::invalid());
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

        std::map<int, int> delays;
        latch_.countDown();
        bool running = true;
        while (running) {
            cnet::Timestamp t(queue_.take());
            cnet::Timestamp now(cnet::Timestamp::now());
            if (t.valid()) {
                int delay = static_cast<int>(cnet::timeDifference(now, t)*1000000);
                ++delays[delay];
            }
            running = t.valid();
        }

        printf("tid=%d, %s stopped\n",
            cnet::CurrentThread::tid(),
            cnet::CurrentThread::name());

        for (std::map<int, int>::iterator it = delays.begin(); it != delays.end();
                ++it)
        {
            printf("tid = %d, delay = %d, count = %d\n",
                cnet::CurrentThread::tid(),
                it->first, it->second);
        }
    }


    cnet::CountDownLatch latch_;
    boost::ptr_vector<cnet::Thread> threads_;
    cnet::BlockingQueue<cnet::Timestamp> queue_;
};

int main()
{
    Bench t(1);
    t.run(10000);
    t.joinAll();
}
