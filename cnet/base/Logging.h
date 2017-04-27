#ifndef CNET_LOGGING_H
#define CNET_LOGGING_H

#include <cnet/base/LogStream.h>
#include <cnet/base/Timestamp.h>

namespace cnet
{
class TimeZone;

class Logger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        NUM_LOG_LEVELS,
    };

    // source file of log
    class SourceFile
    {
    public:
        template <int N>
        inline SourceFile(const char (&arr)[N])
            : data_(arr),
              size_(N-1)
        {
            const char* slash = strrchr(data_, '/');
            if (slash)
            {
                data_ = slash + 1;
                size_ -= static_cast<int>(data_ - arr);
            }
        }

        explicit SourceFile(const char* filename)
            : data_(filename)
        {
            const char *slash = strrchr(filename, '/');
            if (slash)
            {
                data_ = slash + 1;
            }
            size_ = static_cast<int>(strlen(data_));
        }

        const char* data_;
        int size_;
    };

    Logger(SourceFile file, int line);
    Logger(SourceFile file, int line, LogLevel level);
    Logger(SourceFile file, int line, LogLevel level, const char* func);
    Logger(SourceFile file, int line, bool toAbort);
    ~Logger();

    LogStream& stream() { return impl_.stream_; }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char *msg, int len);
    typedef void (*FlushFunc)();
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);
    static void setTimeZone(const TimeZone& tz);

private:
class Impl
{
public:
    typedef Logger::LogLevel LogLevel;
    Impl(LogLevel level, int old_errno, const SourceFile& file, int line);
    void formatTime();
    void finish();

    Timestamp time_;
    LogStream stream_;
    LogLevel level_;
    int line_;
    SourceFile basename_;
};

    Impl impl_;
};

extern Logger::LogLevel g_logLevel;

inline Logger::LogLevel Logger::logLevel()
{
    return g_logLevel;
}

#define LOG_TRACE if (cnet::Logger::logLevel() <= cnet::Logger::TRACE) \
    cnet::Logger(__FILE__, __LINE__, cnet::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (cnet::Logger::logLevel() <= cnet::Logger::DEBUG) \
    cnet::Logger(__FILE__, __LINE__, cnet::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (cnet::Logger::logLevel() <= cnet::Logger::INFO) \
    cnet::Logger(__FILE__, __LINE__, cnet::Logger::INFO).stream()

#define LOG_WARN cnet::Logger(__FILE__, __LINE__, cnet::Logger::WARN).stream()
#define LOG_ERROR cnet::Logger(__FILE__, __LINE__, cnet::Logger::ERROR).stream()
#define LOG_FATAL cnet::Logger(__FILE__, __LINE__, cnet::Logger::FATAL).stream()
#define LOG_SYSERR cnet::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL cnet::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);
// Taken from glog/logging.h
//
// Check that the input is non NULL. This is very useful in constructor
// initializer lists.

#define CHECK_NOTNULL(val) \
    ::cnet::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

//  A small helper for CHECK_NOTNULL()

template <typename T>
T* CheckNotNull(Logger::SourceFile file, int line, const char *names, T* ptr)
{
    if (ptr == NULL)
    {
        Logger(file, line, Logger::FATAL).stream() << names;
    }
    return ptr;
}

}

#endif //CNET_LOGGING_H
