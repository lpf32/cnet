//
// Created by zhang on 17-4-18.
//

#ifndef CNET_NONCOPYABLE_H
#define CNET_NONCOPYABLE_H

namespace cnet
{
class noncopyable
{
protected:
#ifdef __GXX_EXPERIMENTAL_CXX0X__
    noncopyable() = default;
    virtual ~noncopyable() = default;
#else
    noncopyable() {}
    virtual ~noncopyable() {}
#endif

#ifdef __GXX_EXPERIMENTAL_CXX0X__
    noncopyable(const noncopyable&) = delete;
    noncopyable& operator=(const noncopyable&) = delete;
#else
private:
    noncopyable(const noncopyable&);
    noncopyable& operator=(const noncopyable&);
#endif
};
}

#endif //CNET_NONCOPYABLE_H
