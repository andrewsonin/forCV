#ifndef VISITOR_HPP
#define VISITOR_HPP

// >>> Defining matching object constructor for std::variant
template<typename... types>
struct RoundBracePackOverloader : types ... {
    using types::operator()...;
};

template<typename... types> RoundBracePackOverloader(types...) -> RoundBracePackOverloader<types...>;

template<typename... types>
static auto make_visitor(types... pack) {
    return RoundBracePackOverloader<types...>{pack...};
}

// <<< Defining matching object constructor for std::variant <<<

#endif //VISITOR_HPP
