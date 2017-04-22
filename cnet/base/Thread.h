#ifndef CNET_THREAD_H
#define CNET_THREAD_H

#include <cnet/base/Atomic.h>
#include <cnet/base/Types.h>
#include <cnet/base/noncopyable.h>

#ifdef __GXX_EXPERIMENTAL_CXX0X_
#include <functional>
#include <memory>
using std::function;
using std::shared_ptr;
#else
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
using boost::function;
using boost::shared_ptr;
#endif

namespace cnet
{

class Thread : public noncopyable
{
public:
    typedef function<void ()> ThreadFunc;

    explicit Thread(const ThreadFunc&, const string&name = string());

#ifdef __GXX_EXPERIMENTAL_CXX0X_
    explicit Thread(ThreadFunc&&, const string& name = string());
#endif

    ~Thread();

    void start();
    int join(); // return pthread_join();

    bool started() { return started_; }
    pid_t tid() const { return *tid_; }
    const string& name() const { return name_; }

    static int numCreated() { return numCreated_.get(); }

private:
    void setDefaultName();

    bool started_;
    bool joined_;
    pthread_t pthreadId_;
    shared_ptr<pid_t> tid_;
    ThreadFunc func_;
    string name_;

    static AtomicInt32 numCreated_;
};
}


#endif //CNET_THREAD_H
