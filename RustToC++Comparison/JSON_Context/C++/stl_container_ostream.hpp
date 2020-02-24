#ifndef STL_CONTAINER_OSTREAM_HPP
#define STL_CONTAINER_OSTREAM_HPP

#include <ostream>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>

#include "type_checking.hpp"

namespace __STL_container_ostream__ {
    template<typename Map>
    auto PrintMap(std::ostream &os, const Map &m) -> std::enable_if_t<
            IsHashMap<Map>::value || IsTreeMap<Map>::value,
            std::ostream &
    > {
        os << '{';
        bool first = true;
        for (auto &[key, val] : m) {
            if (!first) {
                os << ", ";
            }
            first = false;
            os << key << ": " << val;
        }
        return os << '}';
    }

    template<typename T, template<typename> class Set>
    std::ostream &PrintSet(std::ostream &os, const Set<T> &s) {
        os << '{';
        bool first = true;
        for (auto &x : s) {
            if (!first) {
                os << ", ";
            }
            first = false;
            os << x;
        }
        return os << '}';
    }
}

template<class T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &s) {
    os << '{';
    bool first = true;
    for (const auto &x : s) {
        if (!first) {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << '}';
}

template<class K, class V>
std::ostream &operator<<(std::ostream &os, const std::map<K, V> &m) {
    return __STL_container_ostream__::PrintMap(os, m);
}

template<class K, class V>
std::ostream &operator<<(std::ostream &os, const std::unordered_map<K, V> &m) {
    return __STL_container_ostream__::PrintMap(os, m);
}

template<class K, class V>
std::ostream &operator<<(std::ostream &os, const std::set<K, V> &m) {
    return __STL_container_ostream__::PrintSet(os, m);
}

template<class K, class V>
std::ostream &operator<<(std::ostream &os, const std::unordered_set<K, V> &m) {
    return __STL_container_ostream__::PrintSet(os, m);
}

#endif //STL_CONTAINER_OSTREAM_HPP