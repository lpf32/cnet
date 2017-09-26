#include "sudoku.h"

#include <cnet/net/EventLoop.h>
#include <cnet/net/TcpServer.h>
#include <cnet/net/TcpConnection.h>
#include <cnet/net/InetAddress.h>

#include <cnet/base/Logging.h>

#include <boost/noncopyable.hpp>
#include <boost/bind.hpp>

using namespace cnet;
using namespace cnet::net;

class SudokuServer : boost::noncopyable
{
public:
    SudokuServer(EventLoop *loop, const InetAddress& addr, const string& name, int conn)
        : loop_(loop),
          server_(loop, addr, name),
          threadnum_(conn)
    {
        server_.setThreadNum(threadnum_);
        server_.setConnectionCallback(boost::bind(&SudokuServer::onConnection, this, _1));
        server_.setMessageCallback(boost::bind(&SudokuServer::onMessage, this, _1, _2, _3));
    }

    void start()
    {
        LOG_INFO << "starting " << threadnum_ << " threads";
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
        else
        {
            goodRequest = false;
        }
        return goodRequest;
    }

    EventLoop* loop_;
    TcpServer server_;
    int threadnum_;
};

int main(int argc, char *argv[])
{
    int numThread = 0;
    if (argc > 1)
    {
        numThread = atoi(argv[1]);
    }

    LOG_INFO << "pid = " << getpid() << ", tid = " << CurrentThread::tid();
    EventLoop loop;
    InetAddress addr(9981);
    SudokuServer server(&loop, addr, "sudoku_multiloop", numThread);

    server.start();
    loop.loop();
}