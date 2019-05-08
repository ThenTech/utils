#ifndef UTILS_ALGORITHM_HPP
#define UTILS_ALGORITHM_HPP

#include <algorithm>

namespace utils::algorithm {
    template<typename... T> [[maybe_unused]]
    static inline constexpr bool all(T const&... args) {
        return (... && args);
    }

    template<typename... T> [[maybe_unused]]
    static inline constexpr bool any(T const&... args) {
        return (... || args);
    }

    template<typename... T> [[maybe_unused]]
    static inline constexpr bool none(T const&... args) {
        return !any(args...);
    }

    template <class... T> [[maybe_unused]]
    static inline constexpr bool all_equal(T const&... args) {
        if constexpr (sizeof...(T) == 0) {
            return true;
        } else {
            return [](auto const& a0, auto const&... rest){
                return ((a0 == rest) && ...);
            }(args...);
        }
    }

    template<typename... T> [[maybe_unused]]
    static inline constexpr auto sum(T const&... args) {
        return (args + ... + 0);
    }

    template<typename... T> [[maybe_unused]]
    static inline constexpr auto multiply(T const&... args) {
        return (args * ... * 1);
    }

    template<typename T, typename... Args> [[maybe_unused]]
    static inline constexpr auto min(T const& first, T const& second, Args const&... args) {
        if constexpr (sizeof...(Args) == 0) {
            return std::min(first, second);
        } else {
            return first < second ? utils::algorithm::min(first , args...)
                                  : utils::algorithm::min(second, args...);
        }
    }

    template<typename T, typename... Args> [[maybe_unused]]
    static inline constexpr auto max(T const& first, T const& second, Args const&... args) {
        if constexpr (sizeof...(Args) == 0) {
            return std::max(first, second);
        } else {
            return first > second ? utils::algorithm::max(first , args...)
                                  : utils::algorithm::max(second, args...);
        }
    }
}

#endif // UTILS_ALGORITHM_HPP
