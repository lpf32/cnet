#ifndef CNET_STRINGPIECE_H
#define CNET_STRINGPIECE_H

#include <string.h>
#include <iosfwd>  // for ostream forward-declaration

#include <cnet/base/Types.h>
#ifndef CNET_STD_STRING
#include <string>
#endif

namespace cnet
{
class StringArg
{
public:
    StringArg(const char *str)
            : str_(str)
    { }

    StringArg(const string& str)
            : str_(str.c_str())
    { }

#ifndef CNET_STD_STRING
    StringArg(const std::string& str)
        : str_(str.c_str())
    { }
#endif

    const char *c_str() { return str_; }

private:
    const char *str_;
};

class StringPiece {
    friend bool operator==(const StringPiece&lhs, const StringPiece& rhs);
    friend bool operator!=(const StringPiece&lhs, const StringPiece& rhs);
public:
    // We provide non-explicit singleton constructors so users can pass
    // in a "const char*" or a "string" wherever a "StringPiece" is
    // expected.
    StringPiece()
            : ptr_(NULL),
              length_(0) {}

    StringPiece(const char *str)
            : ptr_(str),
              length_(static_cast<int>(strlen(ptr_)))
    { }

    StringPiece(const unsigned char *str)
            : ptr_(reinterpret_cast<const char*>(str)),
              length_(static_cast<int>(strlen(ptr_)))
    { }
    StringPiece(const string& str)
            : ptr_(str.c_str()),
              length_(static_cast<int>(str.size()))
    { }

#ifndef CNET_STD_STRING
    StringPiece(const std::string& str)
            : ptr_(str.c_str()),
              length_(static_cast<int>(str.size()))
    { }
#endif
    StringPiece(const char* offset, int len)
            : ptr_(offset),
              length_(len)
    { }
    const char* data() const { return ptr_; }
    const char* c_str() const { return ptr_; }
    int size() const { return length_; }
    bool empty() const { return length_ == 0; }
    const char* begin() { return ptr_; }
    const char* end() const { return ptr_+length_; }

    void clear() { ptr_ = NULL; length_ = 0; }
    void set(const char* buffer, int len) { ptr_ = buffer; length_= len; }
    void set(const char* buffer) {
        ptr_ = buffer;
        length_ = static_cast<int>(strlen(ptr_));
    }
    void set(const void* buffer, int len) {
        ptr_ = reinterpret_cast<const char*>(buffer);
        length_ = len;
    }

    char operator[](size_t i) const { return ptr_[i]; }

    void remove_prefix(int n) {
        ptr_ += n;
        length_ -= n;
    }

    void remove_suffix(int n) {
        length_ -= n;
    }

    int compare(const StringPiece& x) const {
        int r = memcmp(ptr_, x.ptr_,
                       static_cast<size_t >(length_ < x.length_ ? length_ : x.length_ ));
        if (r == 0)
        {
            if (length_ < x.length_)
                r = -1;
            else if (length_ > x.length_)
                r = 1;
        }
        return r;
    }

    string as_string() const {
        return string(data(), size());
    }
    
    void CopyToString(string *target) const {
        target->assign(ptr_, length_);
    }

#ifndef CNET_STD_STRING
    void CopyToStdString(std::string *target) const {
        target->assign(ptr_, length_);
    }
#endif
    
    bool starts_with(const StringPiece& x) const {
        return ((length_ > x.length_) 
                && (memcmp(ptr_, x.ptr_, static_cast<size_t >(x.length_)) == 0));
    }


private:
    const char *ptr_;
    int length_;
};

inline bool operator==(const StringPiece &lhs, const StringPiece &rhs) {
    return ((lhs.length_ == rhs.length_) &&
            (memcmp(lhs.ptr_, rhs.ptr_, static_cast<size_t>(lhs.length_)) == 0));
}
inline bool operator!=(const StringPiece &lhs, const StringPiece &rhs) {
    return !(lhs == rhs);
}

}

#ifdef HAVE_TYPE_TRAITS
template<> struct __type_traits<cnet::StringPiece> {
    typedef __true_type has_trivial_default_constructor;
    typedef __true_type has_trivial_copy_constructor;
    typedef __true_type has_trivial_assignment_operator;
    typedef __true_type has_trivial_destructor;
    typedef __true_type is_POD_type;
};
#endif

std::ostream& operator<<(std::ostream& o, const cnet::StringPiece* piece);

#endif //CNET_STRINGPIECE_H
