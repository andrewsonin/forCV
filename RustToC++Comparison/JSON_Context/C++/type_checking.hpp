#ifndef TYPE_CHECKING_HPP
#define TYPE_CHECKING_HPP

#include <memory>
#include <set>
#include <unordered_set>
#include <unordered_map>

template<typename T, typename Enable = void>
struct IsSmartPointer : std::false_type {
};

template<typename T>
struct IsSmartPointer<T,
        std::enable_if_t<
                std::is_same<
                        typename std::decay_t<T>,
                        std::unique_ptr<
                                typename std::decay_t<T>::element_type,
                                typename std::decay_t<T>::deleter_type
                        >
                >::value
        >
> : std::true_type {
};

template<typename T>
struct IsSmartPointer<T,
        std::enable_if_t<
                std::is_same<
                        typename std::decay_t<T>, std::shared_ptr<typename std::decay_t<T>::element_type>
                >::value
        >
> : std::true_type {
};

template<typename T>
struct IsSmartPointer<T,
        std::enable_if_t<
                std::is_same<
                        typename std::decay_t<T>, std::weak_ptr<typename std::decay_t<T>::element_type>
                >::value
        >
> : std::true_type {
};

template<typename T, typename Enable = void>
struct IsTreeSet : std::false_type {
};

template<typename T>
struct IsTreeSet<T,
        std::enable_if_t<
                std::is_same<
                        typename std::decay_t<T>,
                        std::set<
                                typename std::decay_t<T>::key_type,
                                typename std::decay_t<T>::key_compare,
                                typename std::decay_t<T>::allocator_type
                        >
                >::value
        >
> : std::true_type {
};

template<typename T, typename Enable = void>
struct IsTreeMap : std::false_type {
};

template<typename T>
struct IsTreeMap<T,
        std::enable_if_t<
                std::is_same<
                        typename std::decay_t<T>,
                        std::map<
                                typename std::decay_t<T>::key_type,
                                typename std::decay_t<T>::mapped_type,
                                typename std::decay_t<T>::key_compare,
                                typename std::decay_t<T>::allocator_type
                        >
                >::value
        >
> : std::true_type {
};

template<typename T, typename Enable = void>
struct IsHashSet : std::false_type {
};

template<typename T>
struct IsHashSet<T,
        std::enable_if_t<
                std::is_same<
                        typename std::decay_t<T>,
                        std::unordered_set<
                                typename std::decay_t<T>::key_type,
                                typename std::decay_t<T>::hasher,
                                typename std::decay_t<T>::key_equal,
                                typename std::decay_t<T>::allocator_type
                        >
                >::value
        >
> : std::true_type {
};

template<typename T, typename Enable = void>
struct IsHashMap : std::false_type {
};

template<typename T>
struct IsHashMap<T,
        std::enable_if_t<
                std::is_same<
                        typename std::decay_t<T>,
                        std::unordered_map<
                                typename std::decay_t<T>::key_type,
                                typename std::decay_t<T>::mapped_type,
                                typename std::decay_t<T>::hasher,
                                typename std::decay_t<T>::key_equal,
                                typename std::decay_t<T>::allocator_type
                        >
                >::value
        >
> : std::true_type {
};

template<typename T, typename Enable = void>
struct IsVector : std::false_type {
};

template<typename T>
struct IsVector<T,
        std::enable_if_t<
                std::is_same<
                        typename std::decay_t<T>,
                        std::vector<
                                typename std::decay_t<T>::value_type,
                                typename std::decay_t<T>::allocator_type
                        >
                >::value
        >
> : std::true_type {
};

template<typename T, typename Enable = void>
struct IsContainer : std::false_type {
};

template<typename T>
struct IsContainer<T,
        std::enable_if_t<
                IsVector<T>::value
                || IsTreeMap<T>::value
                || IsHashMap<T>::value
                || IsTreeSet<T>::value
                || IsHashSet<T>::value
        >
> : std::true_type {
};

namespace SaveLoad {
    enum Type {
        CONTAINER,
        STRING,
        PAIR,
        ARITHMETIC,
        OTHER
    };

    template<typename T, typename Enable = void>
    struct CheckType {
        static const Type type = Type::OTHER;
    };

    template<typename T>
    struct CheckType<T, std::enable_if_t<IsContainer<T>::value>> {
        static const Type type = Type::CONTAINER;
    };

    template<typename T>
    struct CheckType<T, std::enable_if_t<std::is_arithmetic<T>::value>> {
        static const Type type = Type::ARITHMETIC;
    };

    template<typename Pair>
    struct CheckType<Pair,
            std::enable_if_t<
                    std::is_same<
                            std::decay_t<Pair>,
                            std::pair<
                                    typename std::decay_t<Pair>::first_type,
                                    typename std::decay_t<Pair>::second_type
                            >
                    >::value
            >
    > {
        static const Type type = Type::PAIR;
    };

    template<typename String>
    struct CheckType<String,
            std::enable_if_t<
                    std::is_same<
                            std::decay_t<String>,
                            std::basic_string<
                                    typename std::decay_t<String>::value_type,
                                    typename std::decay_t<String>::traits_type,
                                    typename std::decay_t<String>::allocator_type
                            >
                    >::value
            >
    > {
        static const Type type = Type::STRING;
    };
}

#endif //TYPE_CHECKING_HPP
