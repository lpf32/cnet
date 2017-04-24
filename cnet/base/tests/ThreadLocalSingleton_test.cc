#include <cnet/base/ThreadLocalSingleton.h>
#include <cnet/base/CurrentThread.h>
#include <cnet/base/Thread.h>
#include <cnet/base/noncopyable.h>

#include <boost/bind.hpp>
#include <stdio.h>

using namespace cnet;

class Test : public noncopyable
{
public:
    Test()
    {
        printf("tid=%d, constructing %p\n", CurrentThread::tid(), this);
    }

    ~Test()
    {
        printf("tid=%d, destructing %p %s\n", CurrentThread::tid(), this, name_.c_str());
    }

    const string& name() const { return name_; }
    void setName(const cnet::string& n) { name_ = n; };

private:
    string name_;
};

void threadFunc(const char* changeTo)
{
    printf("tid=%d, %p name=%s\n",
        CurrentThread::tid(),
        &ThreadLocalSingleton<Test>::instance(),
        ThreadLocalSingleton<Test>::instance().name().c_str());
    ThreadLocalSingleton<Test>::instance().setName(changeTo);
    printf("tid=%d, %p name=%s\n",
           CurrentThread::tid(),
           &ThreadLocalSingleton<Test>::instance(),
           ThreadLocalSingleton<Test>::instance().name().c_str());

}

int main()
{
    ThreadLocalSingleton<Test>::instance().setName("main one");
    Thread t1(boost::bind(threadFunc, "thread1"));
    Thread t2(boost::bind(threadFunc, "thread2"));
    t1.start();
    t2.start();
    t1.join();
    printf("tid=%d, %p name=%s\n",
           CurrentThread::tid(),
           &ThreadLocalSingleton<Test>::instance(),
           ThreadLocalSingleton<Test>::instance().name().c_str());
    t2.join();

    pthread_exit(0);
}

