#ifndef CNET_GZIPFILE_H
#define CNET_GZIPFILE_H

#include <cnet/base/StringPiece.h>
#include <cnet/base/noncopyable.h>
#include <zlib.h>

namespace cnet
{

class GzipFile: public noncopyable
{
public:
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    GzipFile(GzipFile&& rhs)
        : file_(rhs.file_)
    {
        rhs.file_ = NULL;
    }

    GzipFile& operator=(GzipFile&& rhs)
    {
        swap(rhs);
        return *this;
    }
#endif

    ~GzipFile()
    {
        if (file_)
        {
            ::gzclose(file_);
        }
    }



    bool valid() const { return file_ != NULL; }
    void swap(GzipFile& rhs) { std::swap(file_, rhs.file_); }

#if ZLIB_VERNUM >= 0x1024
    bool setBuffer(int size) { return ::gzbuffer(file_, size) == 0;}
#endif
    int read(void* buf, int len) { return ::gzread(file_, buf, len); }
    int write(StringPiece buf) { return ::gzwrite(file_, buf.data(), buf.size()); }

    off_t tell() const {return ::gztell(file_); }

#if ZLIB_VERNUM >= 0x1024
    off_t offset() const { return ::gzoffset(file_); }
#endif

    static GzipFile openForRead(StringArg filename)
    {
        return GzipFile(::gzopen(filename.c_str(), "rbe"));
    }

    static GzipFile openForAppend(StringArg filename)
    {
        return GzipFile(::gzopen(filename.c_str(), "abe"));
    }

    static GzipFile openForWriteExclusive(StringArg filename)
    {
        return GzipFile(::gzopen(filename.c_str(), "wbxe"));
    }

    static GzipFile openForWriteTruncate(StringArg filename)
    {
        return GzipFile(::gzopen(filename.c_str(), "wbe"));
    }
private:
    explicit GzipFile(gzFile file)
        : file_(file)
    { }
    gzFile file_;
};
}

#endif //CNET_GZIPFILE_H
