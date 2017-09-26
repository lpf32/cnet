#include "sudoku.h"

#include <cnet/net/EventLoop.h>
#include <cnet/net/TcpServer.h>
#include <cnet/net/TcpConnection.h>
#include <cnet/net/InetAddress.h>

#include <cnet/base/ThreadPool.h>
#include <cnet/base/Logging.h>

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

using namespace cnet;
using namespace cnet::net;

class SudokuServer : boost::noncopyable
{
public:
    SudokuServer(EventLoop *loop, const InetAddress& addr, const string& name, int numThreads)
        : loop_(loop),
          server_(loop, addr, name),
          numThreads_(numThreads)
    {
        server_.setConnectionCallback(boost::bind(&SudokuServer::onConnection, this, _1));
        server_.setMessageCallback(boost::bind(&SudokuServer::onMessage, this, _1, _2, _3));
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
    }

    void onMessage(const TcpConnectionPtr& conn,
                   Buffer* buf,
                   Timestamp timestamp)
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

                if (!processRequest(conn, request))
                {
                    conn->send("Bad Reqeust!\r\n");
                    conn->shutdown();
                    break;
                }
            }
            else if (len > 100)
            {
                conn->send("Id too long!\r\n");
                conn->shutdown();
                break;
            }
            else
            {
                break;
            }
        }
    }

private:
    bool processRequest(const TcpConnectionPtr& conn, const string& request)
    {
        string id;
        string puzzle;
        bool goodRequest = true;

        string::const_iterator colon = find(request.begin(), request.end(), ':');
        if (colon != request.end())
        {
            id.assign(request.begin(), colon);
            puzzle.assign(colon+1, request.end());
        }
        else
        {
            puzzle = request;
        }

        if (puzzle.size() == implicit_cast<size_t>(kCells))
        {
            threadPool_.run(boost::bind(&solve, conn, puzzle, id));
        }
        else
        {
            goodRequest = false;
        }
        return goodRequest;
    }

    static void solve(const TcpConnectionPtr& conn, const string& puzzle, const string& id)
    {
        string answer = solveSudoku(puzzle);
        if (id.empty())
        {
            conn->send(answer + "\r\n");
        }
        else
        {
            conn->send(id + ":" + answer + "\r\n");
        }
    }

    EventLoop* loop_;
    TcpServer server_;
    ThreadPool threadPool_;
    int numThreads_;
};

int main(int argc, char *argv[])
{
    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();

    int numThreads = 0;
    if (argc > 1)
    {
        numThreads = atoi(argv[1]);
    }

    EventLoop loop;
    InetAddress addr(9981);
    SudokuServer server(&loop, addr, "sudoku_basic", numThreads);

    server.start();
    loop.loop();
}