#include <cnet/base/AsyncLogging.h>
#include <cnet/base/Logging.h>
#include <cnet/base/Timestamp.h>

#include <stdio.h>
#include <sys/resource.h>

using namespace cnet;

int KRollSize = 500*1000*1000;

cnet::AsyncLogging* g_asyncLog = NULL;

void asyncOutput(const char* msg, int len)
{
    g_asyncLog->append(msg, len);
}

void bench(bool longLog)
{
    cnet::Logger::setOutput(asyncOutput);

    int cnt = 0;
    const int kBatch = 1000;
    string empty = " ";
    string longStr(3000, 'X');
    longStr += empty;

    for (int i = 0; i < 30; ++i) {
        Timestamp start = Timestamp::now();
        for (int j = 0; j < kBatch; ++j) {
            LOG_INFO << "Hello 0123456789" << " abcdefghijklmnoqprstuvwxyz "
                                           << (longLog ? longStr : empty)
                                           << cnt;
            ++cnt;
        }
        Timestamp end = Timestamp::now();
        printf("%f\n", timeDifference(end, start)*1000000/kBatch);
        struct timespec ts = {0, 500*1000*1000};
        nanosleep(&ts, NULL);
    }
}

int main(int argc, char *argv[])
{
    {
        size_t kOneGB = 1000*1024*1024;
        struct rlimit rl = {2*kOneGB, 2*kOneGB};
        setrlimit(RLiMIT_AS, &rl);
    }

    printf("pid = %d\n", getpid());

    char name[256];
    strncpy(name, argv[0], 256);
    AsyncLogging log(::basename(name), kRollSize);
    log.start();
    g_asyncLog = &log;

    bool longLog = argc > 1;
    bench(longLog);
}