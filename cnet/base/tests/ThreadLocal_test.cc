#include <cnet/base/ThreadLocal.h>
#include <stdio.h>
#include <cnet/base/Thread.h>

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
        printf("tid=%d, destructing %p\n", CurrentThread::tid(), this);
    }

    const string& name() { return name_; }
    void setName(const string& s) { name_ = s; }

private:
    string name_;
};

ThreadLocal<Test> test1;
ThreadLocal<Test> test2;

void print()
{
    printf("tid=%d, obj1 %p name=%s\n",
    CurrentThread::tid(),
    &test1.value(),
    test1.value().name().c_str());

    printf("tid=%d, obj1 %p name=%s\n",
           CurrentThread::tid(),
           &test2.value(),
           test2.value().name().c_str());
}

void threadFunc() {
    print();
    test1.value().setName("changed 1");
    test2.value().setName("changed 2");
    print();
}

int main()
{
    test1.value().setName("main one");
    print();
    Thread t1(threadFunc);
    t1.start();
    t1.join();
    test2.value().setName("main two");
    print();
    return 0;
}
