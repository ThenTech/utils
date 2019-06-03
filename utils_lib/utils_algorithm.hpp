#ifndef UTILS_ALGORITHM_HPP
#define UTILS_ALGORITHM_HPP

#include "utils_traits.hpp"

#include <algorithm>
#include <functional>


namespace utils::algorithm {
    /**
     *  \brief  Generic AND all args.
     *
     *  \return
     *      True if arg1 && ... && argn == true
     */
    template<typename... T> ATTR_MAYBE_UNUSED
    static inline constexpr bool all(T const&... args) {
        return (... && args);
    }

    /**
     *  \brief  Generic OR all args.
     *
     *  \return
     *      True if arg1 || ... || argn == true
     */
    template<typename... T> ATTR_MAYBE_UNUSED
    static inline constexpr bool any(T const&... args) {
        return (... || args);
    }

    /**
     *  \brief  Generic NOT OR all args.
     *
     *  \return
     *      True if !(arg1 || ... || argn) == true
     */
    template<typename... T> ATTR_MAYBE_UNUSED
    static inline constexpr bool none(T const&... args) {
        return !any(args...);
    }

    /**
     *  \brief  Generic test equality of all args.
     *
     *  \return
     *      True if arg1 == ... == argn
     */
    template <typename... T> ATTR_MAYBE_UNUSED
    static inline constexpr bool all_equal(T const&... args) {
        if constexpr (sizeof...(T) == 0) {
            return true;
        } else {
            return [](auto const& a0, auto const&... rest){
                return ((a0 == rest) && ...);
            }(args...);
        }
    }

    /**
     *  \brief  Generic test if all args are within [min, max] (inclusive).
     *
     *  \return
     *      True if (min <= arg1 && arg1 <= max) && ...
     */
    template <typename T, typename... Ts> ATTR_MAYBE_UNUSED
    static inline constexpr bool all_within(T min, T max, const Ts&... ts) {
        if constexpr (sizeof...(Ts) == 0) {
            UNUSED(min, max);
            return true;
        } else {
            if (max < min) // Allow range from higher to lower
                std::swap(min, max);
            return ((min <= ts && ts <= max) && ...);
        }
    }

    /**
     *  \brief  Generic test if all elements in the container
     *          are within [min, max] (inclusive).
     *
     *  \return
     *      True if (min <= arg1 && arg1 <= max) && ...
     */
    template <typename T, typename Container> ATTR_MAYBE_UNUSED
    static inline constexpr bool within(T min, T max, const Container& cont) {
        static_assert (is_iterable_v(cont),
                       "utils::bits::all_within: Container must have iterator support.");
        if (max < min) // Allow range from higher to lower
            std::swap(min, max);

        return std::all_of(std::begin(cont), std::end(cont), [min, max](const T& x){
            return min <= x && x <= max;
        });
    }

    /**
     *  \brief  Generic sum all args.
     *
     *  \return
     *      Returns arg1 + ... + argn
     */
    template<typename... T> ATTR_MAYBE_UNUSED
    static inline constexpr auto sum(T const&... args) {
        return (args + ... + 0);
    }

    /**
     *  \brief  Generic multiply all args.
     *
     *  \return
     *      Returns arg1 * ... * argn
     */
    template<typename... T> ATTR_MAYBE_UNUSED
    static inline constexpr auto multiply(T const&... args) {
        return (args * ... * 1);
    }

    /**
     *  \brief  Generic determine minimum of all args.
     *
     *  \return
     *      Returns std::min(arg1, std::min(..., argn))
     */
    template<typename T, typename... Args> ATTR_MAYBE_UNUSED
    static inline constexpr auto min(T const& first, T const& second, Args const&... args) {
        if constexpr (sizeof...(Args) == 0) {
            return std::min(first, second);
        } else {
            return first < second ? utils::algorithm::min(first , args...)
                                  : utils::algorithm::min(second, args...);
        }
    }

    /**
     *  \brief  Generic determine maximum of all args.
     *
     *  \return
     *      Returns std::max(arg1, std::max(..., argn))
     */
    template<typename T, typename... Args> ATTR_MAYBE_UNUSED
    static inline constexpr auto max(T const& first, T const& second, Args const&... args) {
        if constexpr (sizeof...(Args) == 0) {
            return std::max(first, second);
        } else {
            return first > second ? utils::algorithm::max(first , args...)
                                  : utils::algorithm::max(second, args...);
        }
    }

    /**
     *  \brief  Generic repaet Action a certain amount of times.
     *          Call std::invoke on fn with the given arguments, \p times times.
     *          (default: 1 time)
     *
     *  \param  fn
     *      The action to call. Must be invocable with the given args.
     *      e.g. a lambda expression (scopeness does not matter),
     *           a function pointer.
     *  \param  ...args
     *      Arguments to be passed to fn.
     */
    template<size_t times = 1, typename F, typename... Args>
    void repeat(F&& fn, Args &&... args) {
        static_assert(std::is_invocable_v<F, Args...>, "utils::algorithm::repeat: Callable function required.");
        for (size_t i = 0; i < times; i++) {
            std::invoke(std::forward<F>(fn), std::forward<Args>(args)...);
        }
    }

    template <
        typename T,
        typename TIter = decltype(std::begin(std::declval<T>())),
        typename       = decltype(std::end  (std::declval<T>()))
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto enumerate(T&& iterable) {
        struct iterator {
            size_t i;
            TIter iter;

            bool operator!=(const iterator& other) const {
                return iter != other.iter;
            }

            void operator++() {
                ++i; ++iter;
            }

            auto operator*() const {
                return std::tie(i, *iter);
            }
        };

        struct iterable_wrapper {
            T iterable;
            auto begin() { return iterator{ 0, std::begin(iterable) }; }
            auto end()   { return iterator{ 0, std::end  (iterable) }; }
        };

        return iterable_wrapper{ std::forward<T>(iterable) };
    }
}

#endif // UTILS_ALGORITHM_HPP
