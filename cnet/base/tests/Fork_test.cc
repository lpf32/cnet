#include <cnet/base/CurrentThread.h>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

namespace {
    __thread int x = 0;
}

using namespace cnet;

void print()
{
    printf("pid=%d tid=%d x=%d\n", getpid(), CurrentThread::tid(), x);
}

int main()
{
    printf("parent %d\n", getpid());
    print();
    x = 1;
    print();
    pid_t p = fork();

    if (p == 0)
    {
        printf("child %d\n", getpid());
        print();
        x = 2;
        print();

        if (fork() == 0)
        {
            printf("grandchild %d\n", getpid());
            print();
            x = 3;
            print();
        }
    }
    else
    {
        print();
    }
}