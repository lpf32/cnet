#include "MemcacheServer.h"
#include <cnet/net/EventLoop.h>

#include <stdio.h>
#ifdef HAVE_TCMALLOC
#include <gperftools/heap-profiler.h>
#include <gperftools/malloc_extension.h>
#endif

using namespace cnet::net;

int main(int argc, char *argv[])
{
#ifdef HAVE_TCMALLOC
    MallocExtension::Initialize();
#endif

    int items = argc > 1 ? atoi(argv[1]) : 10000;
    int keylen = argc > 2 ? atoi(argv[2]) : 10;
    int valuelen = argc > 3 ? atoi(argv[2]) : 100;

    EventLoop loop;
    MemcacheServer::Options options;
    MemcacheServer server(&loop, options);

    printf("sizeof(Item) = %zd\npid = %d\nitems = %d\nkeylen = %d\nvalueLen = %d\n",
        sizeof(Item), getpid(), items, keylen, valuelen);
    char key[256] = { 0 };
    string value;
    for (int i = 0; i < items; ++i)
    {
        snprintf(key, sizeof key, "%0*d", keylen, i);
        value.assign(valuelen, "0123456789"[i % 10]);
        ItemPtr item(Item::makeItem(key, 0, 0, valuelen+2, 1));
        item->append(value.data(), value.size());
        item->append("\r\n", 2);
        assert(item->endsWithCRLF());
        bool  exists = false;
        bool stored = server.storeItem(item, Item::kAdd, &exists);
        assert(stored);
        assert(!exists);
    }

}