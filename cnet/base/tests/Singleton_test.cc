#include <cnet/base/Singleton.h>
#include <cnet/base/CurrentThread.h>
#include <cnet/base/Thread.h>
#include <cnet/base/noncopyable.h>

#include <stdio.h>
using namespace cnet;

class Test : public  noncopyable
{
public:
    Test()
    {
        printf("tid = %d, constructing %p\n", CurrentThread::tid(), this);
    }

    ~Test()
    {
        printf("tid = %d, destructing %p %s\n", CurrentThread::tid(), this, name_.c_str());
    }

    const string& name() const { return name_; }
    void setName(const string& n) { name_ = n; }

private:
    string name_;
};

class TestNoDestroy : public noncopyable
{
public:
    void no_destroy();

    TestNoDestroy()
    {
        printf("tid = %d, constructing %p\n", CurrentThread::tid(), this);
    }

    ~TestNoDestroy()
    {
        printf("tid = %d, destructing %p\n", CurrentThread::tid(), this);
    }
};

void threadFunc()
{
    printf("tid=%d, %p name=%s\n",
        CurrentThread::tid(),
        &Singleton<Test>::instance(),
        Singleton<Test>::instance().name().c_str());
    Singleton<Test>::instance();
    Singleton<Test>::instance().setName("only one, changed");
}

int main()
{
    Singleton<Test>::instance().setName("only one");
    Thread t1(threadFunc);
    t1.start();
    t1.join();
    printf("tid=%d, %p name=%s\n",
           CurrentThread::tid(),
           &Singleton<Test>::instance(),
           Singleton<Test>::instance().name().c_str()
    );
    Singleton<TestNoDestroy>::instance();
    printf("with valgrind,you shuould see %zd-byte memory leak.\n", sizeof(TestNoDestroy));
}
