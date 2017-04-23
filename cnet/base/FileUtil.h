#ifndef CNET_FILEUTIL_H
#define CNET_FILEUTIL_H

#include <cnet/base/StringPiece.h>
#include <cnet/base/noncopyable.h>

namespace cnet
{
namespace FileUtil
{

// read small file < 64KB
class ReadSmallFile : public noncopyable
{
public:
    ReadSmallFile(StringArg filename);
    ~ReadSmallFile();

    // return errno
    template <typename T>
    int readToString(int maxSize,
                        T* content,
                        int64_t* fileSize,
                        int64_t* modifyTime,
                        int64_t* createTime);

    /// Read at maxium kBufferSize info buf)
    // return errno
    int readToBuffer(int* size);

    const char* buffer() const { return buf_; }

    static const int kBufferSize = 64*1024;
private:
    int fd_;
    int err_;
    char buf_[kBufferSize];
};

// read the file content, returns errno if error happens.
template <typename T>
int readFile(StringArg filename,
                 int maxSize,
                 T* content,
                 int64_t* fileSize = NULL,
                 int64_t* modifyTime = NULL,
                 int64_t* createTime = NULL)
{
    ReadSmallFile file(filename);
    return file.readToString(maxSize, content, fileSize, modifyTime, createTime);
}

// FIXME not thread safe
class AppendFile : public noncopyable
{
public:
    explicit AppendFile(StringArg filename);

    ~AppendFile();

    void append(const char*logline, const size_t len);

    void flush();

    size_t writtenBytes() const { return writtenBytes_; }
private:
    size_t write(const char*logline, size_t len);

    FILE* fp_;
    char buffer_[ReadSmallFile::kBufferSize];
    size_t writtenBytes_;
};
}
}

#endif //CNET_LOGFILE_H
