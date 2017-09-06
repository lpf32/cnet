#ifndef CNET_SESSION_H
#define CNET_SESSION_H

#include "Item.h"

#include <cnet/base/Logging.h>
#include <cnet/net/TcpConnection.h>

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/noncopyable.hpp>
#include <boost/tokenizer.hpp>

using namespace cnet;
using namespace cnet::net;

class MemcacheServer;

class Session : boost::noncopyable,
                public boost::enable_shared_from_this<Session>
{
public:
    Session(MemcacheServer *owner, const cnet::net::TcpConnectionPtr &conn)
        : owner_(owner),
          conn_(conn),
          state_(kNewCommand),
          protocol_(kAscii),
          noreply_(false),
          policy_(Item::kInvalid),
          bytesToDiscard_(0),
          needle_(Item::makeItem(kLongestKey, 0, 0, 2, 0)),
          bytesRead_(0),
          requestsProcessed_(0)
    {
        conn->setMessageCallback(boost::bind(&Session::onMessage, this, _1, _2, _3));
    }

    ~Session()
    {
        LOG_INFO << "requests processed: " << requestsProcessed_
                 << " input buffer size: " << conn_->inputBuffer()->internalCapacity()
                 << " output buffer size: " << conn_->outputBuffer()->internalCapacity();
    }

private:
    enum State
    {
        kNewCommand,
        kReceiveValue,
        kDiscardValue,
    };

    enum Protocol
    {
        kAscii,
        kBinary,
        kAuto,
    };

    void onMessage(const TcpConnectionPtr &conn, Buffer *buf, Timestamp timestamp);

    void onWriteComplete(const TcpConnectionPtr &conn);

    void receiveValue(Buffer *buf);

    void discardValue(Buffer *buf);

    // returns true if finished a request
    bool processRequest(StringPiece request);

    void resetRequest();

    void reply(StringPiece msg);

    struct SpaceSeparator
    {
        void reset() {}
        template <typename InputIterator, typename Token>
        bool operator()(InputIterator& next, InputIterator end, Token& tok);
    };

    typedef boost::tokenizer<SpaceSeparator,
            const char*,
            StringPiece> Tokenizer;
    struct Reader;
    bool doUpdate(Tokenizer::iterator& beg, Tokenizer::iterator end);
    void doDelete(Tokenizer::iterator& beg, Tokenizer::iterator end);

    MemcacheServer* owner_;
    cnet::net::TcpConnectionPtr conn_;
    State state_;
    Protocol protocol_;

    //current reqeust
    string command_;
    bool noreply_;
    Item::UpdatePolicy policy_;
    ItemPtr currItem_;
    size_t bytesToDiscard_;

    //cached
    ItemPtr needle_;
    cnet::net::Buffer outputBuf_;

    // per session stats
    size_t bytesRead_;
    size_t requestsProcessed_;

    static string kLongestKey;
};

typedef boost::shared_ptr<Session> SessionPtr;

#endif //CNET_SESSION_H
