#include <cnet/base/Thread.h>
#include <cnet/base/CurrentThread.h>
#include <cnet/base/Exception.h>
#include <cnet/base/Logging.h>

#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

#ifdef __GXX_EXPERIMENTAL_CXX0X_
#include <memory>
using std::weak_ptr;
#else
#include <boost/weak_ptr.hpp>
using boost::weak_ptr;
#endif

#include <errno.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

namespace cnet
{
namespace CurrentThread
{
    __thread int t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength = 6;
    __thread const char* t_threadName = "unknow";
    const bool sameType = boost::is_same<int, pid_t>::value;
    BOOST_STATIC_ASSERT(sameType);
}

namespace detail
{
pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}
void afterFork()
{
    cnet::CurrentThread::t_cachedTid = 0;
    cnet::CurrentThread::t_threadName = "main";
    CurrentThread::tid();
}

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        cnet::CurrentThread::t_threadName = "main";
        CurrentThread::tid();
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer init;
struct ThreadData
{
    typedef cnet::Thread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    string name_;
    weak_ptr<pid_t> wkTid_;

    ThreadData(const ThreadFunc& func, const string& name, const shared_ptr<pid_t>& tid)
        : func_(func),
          name_(name),
          wkTid_(tid)
    { }

    void runInThread()
    {
        pid_t tid = cnet::CurrentThread::tid();

        shared_ptr<pid_t> ptid = wkTid_.lock();
        if (ptid)
        {
            *ptid = tid;
            ptid.reset();
        }

        cnet::CurrentThread::t_threadName = name_.empty() ? "cnetThread" : name_.c_str();
        ::prctl(PR_SET_NAME, cnet::CurrentThread::t_threadName); // set thread name, opposition with PR_GET_NAME
        try
        {
            func_();
            cnet::CurrentThread::t_threadName = "finished";
        }
        catch (const Exception& ex)
        {
            cnet::CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
            fprintf(stderr, "statck trace: %s\n", ex.stackTrace());
        }
        catch (const std::exception& ex)
        {
            cnet::CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "exception caught in Thread %s\n", name_.c_str());
            fprintf(stderr, "reason: %s\n", ex.what());
        }
        catch (...)
        {
            cnet::CurrentThread::t_threadName = "crashed";
            fprintf(stderr, "unknown exception caught in Thread %s\n", name_.c_str());
            throw ;
        }
    }
};

void *startThread(void *obj)
{
    ThreadData *data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return NULL;
}

}
}

using namespace cnet;

void CurrentThread::cacheTid() {
    if (t_cachedTid == 0)
    {
        t_cachedTid = detail::gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof t_tidString, "%5d ", t_cachedTid);
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

void CurrentThread::sleepUsec(int64_t usec)
{
    struct timespec ts = {0, 0};
    ts.tv_sec = static_cast<time_t>(usec / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>(usec % Timestamp::kMicroSecondsPerSecond * 1000);
    // thread sleep
    ::nanosleep(&ts, NULL);
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc &func, const string &name)
    : started_(false),
      joined_(false),
      pthreadId_(0),
      tid_(new pid_t(0)),
      func_(func),
      name_(name)
{
    setDefaultName();
}

#ifdef __GXX_EXPERIMENTAL_CXX0X_
Thread::Thread(ThreadFunc &&func, const string &name)
        : started_(false),
          joined_(false),
          pthreadId_(0),
          tid_(new pid_t(0)),
          func_(std::move(func)),
          name_(name)
{
    setDefaultName();
}
#endif

Thread::~Thread()
{
    if (started_ && !joined_)
    {
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    int num = numCreated_.incrementAndGet();
    if (name_.empty())
    {
        char buf[32];
        snprintf(buf, sizeof buf, "Thread%d", num);
        name_ = string(buf);
    }
}

void Thread::start()
{
    assert(!started_);
    started_ = true;
    detail::ThreadData *data = new detail::ThreadData(func_, name_, tid_);
    if (pthread_create(&pthreadId_, NULL, &detail::startThread, data))
    {
        started_ = false;
        delete data;
        LOG_SYSFATAL << "Failed in pthread_create";
    }
}

int Thread::join()
{
    assert(started_);
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, NULL);
}