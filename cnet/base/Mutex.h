#ifndef CNET_MUTEX_H
#define CNET_MUTEX_H

#include <cnet/base/CurrentThread.h>
#include <cnet/base/noncopyable.h>
#include <assert.h>
#include <pthread.h>

#ifdef CHECK_PTHREAD_RETURN_VALUE

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail (int errnum,
                                  const char *file,
                                  unsigned int line,
                                  const char *function)
        __THROW __attribute__ ((__noreturn__));
__END_DECLS
#endif

#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret); \
                        if (__builtin_expect(errnum != 0, 0)) \
                        __assert_perror_fail(errnum, __FILE__, __LINE__, __func__);})

#else
#define MCHECK(ret) ({ __typeof__ (ret) errnum = (ret); \
                        assert(errnum == 0); UNUSED(errnum);})
#endif //CNET_MUTEX_H

namespace cnet
{
    // Use as data member of a claess
class MutexLock: public noncopyable
{
public:
    MutexLock()
        : holder_(0)
    {
        MCHECK(pthread_mutex_init(&mutex_, NULL));
    }

    ~MutexLock()
    {
        assert(holder_ == 0);
        MCHECK(pthread_mutex_destroy(&mutex_));
    }

    // must be called when locked, e.e. for assertion
    bool isLockedByThisThread() const
    {
        return holder_ == CurrentThread::tid();
    }

    void assertLocked() const
    {
        assert(isLockedByThisThread());
    }

    // internal usage
    void lock()
    {
        MCHECK(pthread_mutex_lock(&mutex_));
        assignHolder();
    }

    void unlock()
    {
        unassignHolder();
        MCHECK(pthread_mutex_unlock(&mutex_));
    }

    pthread_mutex_t* getPthreadMutex()
    {
        return &mutex_;
    }

private:
    friend class Condition;
/// usage: for condition:wait
    class UnassignGuard : public noncopyable
    {
    public:
        UnassignGuard(MutexLock& owner)
            : owner_(owner)
        {
            owner_.unassignHolder();
        }

        ~UnassignGuard()
        {
            owner_.assertLocked();
        }

    private:
        MutexLock& owner_;
    };

    void unassignHolder()
    {
        holder_ = 0;
    }

    void assignHolder()
    {
        holder_ = CurrentThread::tid();
    }

    pthread_mutex_t mutex_;
    pid_t holder_;
};

// Use as a stack variable, eg.
// int Foo::size() const
//  {
//     MutexLockGuard lock(mutex_);
//       return data_.size();
//  }

class MutexLockGuard : public noncopyable
{
public:
    explicit MutexLockGuard(MutexLock& mutex)
        : mutex_(mutex)
    {
        mutex_.lock();
    }

    ~MutexLockGuard()
    {
        mutex_.unlock();
    }

private:
    MutexLock& mutex_;
};

}
// Prevent misuse lick:
//    MutexLockGuard(mutex_)
//    A tempory object doesn't hold the lock for long

#define MutexLockGuard(x) error "Missing guard object name"

#endif