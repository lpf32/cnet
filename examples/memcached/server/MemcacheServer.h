#ifndef CNET_MEMCACHESERVER_H
#define CNET_MEMCACHESERVER_H

#include "Item.h"
#include "Session.h"

#include "cnet/base/Mutex.h"
#include "cnet/net/TcpServer.h"
#include "examples/wordcount/hash.h"

#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

class MemcacheServer : boost::noncopyable
{
public:
    struct Options {
        Options();
        uint16_t tcpport;
        uint16_t udpport;
        uint16_t gperfport;
        int threads;
    };

    MemcacheServer(cnet::net::EventLoop* loop, const Options&);
    ~MemcacheServer();

    void setThreadNum(int threads) { server_.setThreadNum(threads); }
    void start();
    void stop();

    time_t startTime() const { return startTime_; }

    bool storeItem(const ItemPtr& item, Item::UpdatePolicy policy, bool* exists);
    ConstItemPtr getItem(const ConstItemPtr& key) const;
    bool deleteItem(const ConstItemPtr& key);

private:
    void onConnection(const cnet::net::TcpConnectionPtr& conn);

    struct Stats;

    cnet::net::EventLoop* loop_;
    Options options_;
    const time_t startTime_;

    mutable cnet::MutexLock mutex_;
    boost::unordered_map<string, SessionPtr> sessions_;

    struct Hash
    {
        size_t operator()(const ConstItemPtr& x) const
        {
            return x->hash();
        }
    };

    struct Equal
    {
        bool operator()(const ConstItemPtr& x, const ConstItemPtr& y) const
        {
            return x->key() == y->key();
        }
    };

    typedef boost::unordered_set<ConstItemPtr, Hash, Equal> ItemMap;

    struct MapWithLock
    {
        ItemMap items;
        mutable cnet::MutexLock mutex;
    };

    const static int kShards = 4096;

    boost::array<MapWithLock, kShards> shards_;

    cnet::net::TcpServer server_;
    boost::scoped_ptr<Stats> stats_;
};

#endif //CNET_MEMCACHESERVER_H
