#ifndef CNET_CURRENTTHREAD_H
#define CNET_CURRENTTHREAD_H

namespace cnet
{
namespace CurrentThread
{

    extern __thread int t_cachedTid;
    extern __thread char t_tidString[32];
    extern __thread int t_tidStringLength;
    extern __thread const char *t_threadName;

    void cacheTid();

    inline int tid()
    {
        if (__buildin_expect(t_cachedTid == 0, 0))
        {
            cacheTid();
        }
        return t_cachedTid;
    }
}

}

#endif //CNET_CURRENTTHREAD_H
