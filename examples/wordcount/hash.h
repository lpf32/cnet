#ifndef CNET_HASH_H
#define CNET_HASH_H

namespace boost
{
    std::size_t hash_value(const cnet::string &x);
}

#include <boost/unordered_map.hpp>

namespace boost
{
    inline std::size_t hash_value(const cnet::string& x)
    {
        return hash_range(x.begin(), x.end());
    }
}

typedef boost::unordered_map<cnet::string, int64_t > WordCountMap;

#endif //CNET_HASH_H
