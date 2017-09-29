#include "sudoku.h"

#include <cnet/net/EventLoop.h>
#include <cnet/net/TcpServer.h>
#include <cnet/net/TcpConnection.h>
#include <cnet/net/InetAddress.h>
#include <cnet/net/EventLoopThread.h>

#include <cnet/base/ThreadPool.h>
#include <cnet/base/Logging.h>
#include <cnet/base/Thread.h>
#include <cnet/base/Atomic.h>

#include <cnet/net/inspect/Inspector.h>

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>
#include <boost/circular_buffer.hpp>

using namespace cnet;
using namespace cnet::net;

#include "stat.h"

class SudokuServer : boost::noncopyable
{
public:
    SudokuServer(EventLoop *loop,
                 const InetAddress& addr,
                 const string& name,
                 int numEventLoops,
                 int numThreads,
                 bool nodelay)
        : loop_(loop),
          server_(loop, addr, name),
          threadPool_(),
          numThreads_(numThreads),
          startTime_(Timestamp::now()),
          tcpNoDelay_(nodelay),
          stat_(threadPool_),
          inspectThread_(),
          inspector_(inspectThread_.startLoop(), InetAddress(9982), "sudoku-solver")
    {
        LOG_INFO << "Use " << numEventLoops << " IO threads.";
        LOG_INFO << "TCP no delay " << nodelay;

        server_.setConnectionCallback(boost::bind(&SudokuServer::onConnection, this, _1));
        server_.setMessageCallback(boost::bind(&SudokuServer::onMessage, this, _1, _2, _3));
        server_.setThreadNum(numEventLoops);

        inspector_.add("sudoku", "stats", boost::bind(&SudokuStat::report, &stat_),
                       "statistics of sudoku solver");
        inspector_.add("sudoku", "reset", boost::bind(&SudokuStat::reset, &stat_),
                        "reset statistics of sudoku solver");
    }

    void start()
    {
        LOG_INFO << "starting " << numThreads_ << " threads.";
        threadPool_.start(numThreads_);
        server_.start();
    }

    void onConnection(const TcpConnectionPtr& conn)
    {
        LOG_INFO << conn->peerAddress().toIpPort() << " -> "
                  << conn->localAddress().toIpPort() << " is "
                  << (conn->connected() ? "UP" : "DONW");

        if (conn->connected() && tcpNoDelay_)
            conn->setTcpNoDelay(true);
    }

    void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf,
                   Timestamp receiveTime)
    {
        LOG_DEBUG << conn->name();
        size_t len = buf->readableBytes();

        while (len >= kCells + 2)
        {
            const char* crlf = buf->findCRLF();
            if (crlf)
            {
                string request(buf->peek(), crlf);

                buf->retrieveUntil(crlf+2);
                len = buf->readableBytes();
                stat_.recordRequest();

                if (!processRequest(conn, request, receiveTime))
                {
                    conn->send("Bad Reqeust!\r\n");
                    conn->shutdown();
                    stat_.recordBadRequest();
                    break;
                }
            }
            else if (len > 100)
            {
                conn->send("Id too long!\r\n");
                conn->shutdown();
                stat_.recordBadRequest();
                break;
            }
            else
            {
                break;
            }
        }
    }

private:
    struct Request
    {
        string id;
        string puzzle;
        Timestamp receiveTime;
    };

    bool processRequest(const TcpConnectionPtr& conn, const string& request, Timestamp receiveTIme)
    {
        Request req;
        req.receiveTime = receiveTIme;

        string::const_iterator colon = find(request.begin(), request.end(), ':');
        if (colon != request.end())
        {
            req.id.assign(request.begin(), colon);
            req.puzzle.assign(colon+1, request.end());
        }
        else
        {
            if (numThreads_ > 1)
                return false;

            req.puzzle = request;
        }

        if (req.puzzle.size() == implicit_cast<size_t>(kCells))
        {
            threadPool_.run(boost::bind(&SudokuServer::solve, this, conn, req));
            return true;
        }

        return false;
    }

    void solve(const TcpConnectionPtr& conn, const Request& req)
    {
        LOG_DEBUG << conn->name();

        string answer = solveSudoku(req.puzzle);
        if (req.id.empty())
        {
            conn->send(answer + "\r\n");
        }
        else
        {
            conn->send(req.id + ":" + answer + "\r\n");
        }
        stat_.recordResponse(Timestamp::now(), req.receiveTime, answer != kNoSolution);
    }

    EventLoop* loop_;
    TcpServer server_;
    ThreadPool threadPool_;
    const int numThreads_;
    const Timestamp startTime_;
    const bool tcpNoDelay_;

    SudokuStat stat_;
    EventLoopThread inspectThread_;
    Inspector inspector_;
};

int main(int argc, char *argv[])
{
    LOG_INFO << argv[0] << " [number of IO threads] [number of worker threads] [-n]";
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();

    int numEventThreads = 0;
    int numThreads = 0;
    bool nodelay = false;
    if (argc > 1)
    {
        numEventThreads = atoi(argv[1]);
    }
    if (argc > 2)
    {
        numThreads = atoi(argv[2]);
    }
    if (argc > 3 && string(argv[3]) == "-n")
    {
        nodelay = true;
    }


    EventLoop loop;
    InetAddress addr(9981);
    SudokuServer server(&loop, addr, "sudoku_hybrid", numEventThreads, numThreads, nodelay);

    server.start();
    loop.loop();
}