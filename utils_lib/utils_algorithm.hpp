#ifndef UTILS_ALGORITHM_HPP
#define UTILS_ALGORITHM_HPP

namespace utils::algorithm {
    template<typename... T> [[maybe_unused]]
    static inline bool all(T const&... args) {
        return (... && args);
    }

    template<typename... T> [[maybe_unused]]
    static inline bool any(T const&... args) {
        return (... || args);
    }

    template<typename... T> [[maybe_unused]]
    static inline bool none(T const&... args) {
        return !any(args...);
    }

    template <class... T> [[maybe_unused]]
    static inline bool all_equal(T const&... args) {
        if constexpr (sizeof...(T) == 0) {
            return true;
        } else {
            return [](auto const& a0, auto const&... rest){
                return ((a0 == rest) && ...);
            }(args...);
        }
    }

    template<typename... T> [[maybe_unused]]
    static inline auto sum(T const&... args) {
        return (args + ... + 0);
    }

    template<typename... T> [[maybe_unused]]
    static inline auto multiply(T const&... args) {
        return (args * ... * 1);
    }
}

#endif // UTILS_ALGORITHM_HPP
