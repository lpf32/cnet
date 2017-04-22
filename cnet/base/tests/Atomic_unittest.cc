#include <cnet/base/Atomic.h>
#include <assert.h>

int main()
{
    {
        cnet::AtomicInt32 a1;
        assert(a1.get() == 0);
        assert(a1.getAndAdd(1) == 0);
        assert(a1.get() == 1);
        assert(a1.addAndGet(2) == 3);
        assert(a1.get() == 3);
        assert(a1.incrementAndGet() == 4);
        assert(a1.get() == 4);
        assert(a1.decrementAndGet() == 3);
        assert(a1.get() == 3);
        a1.increment();
        assert(a1.get() == 4);
        assert(a1.addAndGet(-3) == 1);
        assert(a1.getAndSet(100) == 1);
        assert(a1.get() == 100);
    }

    {
        cnet::AtomicInt64 a1;
        assert(a1.get() == 0);
        assert(a1.getAndAdd(1) == 0);
        assert(a1.get() == 1);
        assert(a1.addAndGet(2) == 3);
        assert(a1.get() == 3);
        assert(a1.incrementAndGet() == 4);
        assert(a1.get() == 4);
        assert(a1.decrementAndGet() == 3);
        assert(a1.get() == 3);
        a1.increment();
        assert(a1.get() == 4);
        assert(a1.addAndGet(-3) == 1);
        assert(a1.getAndSet(100) == 1);
        assert(a1.get() == 100);
    }


}