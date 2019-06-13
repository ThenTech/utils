#ifndef UTILS_ALGORITHM_HPP
#define UTILS_ALGORITHM_HPP

#include "external/cppitertools/itertools.hpp"
#include "utils_traits.hpp"

#include <algorithm>
#include <functional>
#include <numeric>
#include <optional>


namespace utils::algorithm {
    /**
     *  Wrap CPPItertools as ::iter
     *  Reference: https://github.com/ryanhaining/cppitertools (08/06/2019)
     */
    namespace iter = ::iter;

    /**
     *  \brief  Check if the given container contains the given item.
     *          The type of item must be something that the container holds.
     *              e.g.
     *                  vector<int>  <- int
     *                  string       <- char
     *              and not:
     *                  vector<Node> /- int
     *
     *  \param  container
     *      The container to look in.
     *  \param  item
     *      The item to look for.
     *  \return Returns an `std:optional<size_t>` holding the offset from
     *          std::begin(container) if the item was found, or nothing if not.
     */
    template<
        typename C, typename T,
        typename = typename std::enable_if_t<utils::traits::is_container<C>::value>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr utils::traits::found_t contains(const C& container, const T& item) {
        if constexpr (utils::traits::has_find_v<C, T>) {
            if (const auto idx = container.find(item); std::end(container) != idx) {
                return { std::distance(std::begin(container), idx) };
            }
        } else {
            if (const auto idx = std::find(std::begin(container), std::end(container), item);
                std::end(container) != idx)
            {
                return { std::distance(std::begin(container), idx) };
            }
        }

        return std::nullopt;
    }

    /**
     *  \brief  Generic AND all args.
     *
     *  \return
     *      True if arg1 && ... && argn == true
     */
    template<typename... T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool all(T const&... args) {
        return (... && args);
    }

    /**
     *  \brief  Generic OR all args.
     *
     *  \return
     *      True if arg1 || ... || argn == true
     */
    template<typename... T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool any(T const&... args) {
        return (... || args);
    }

    /**
     *  \brief  Generic NOT OR all args.
     *
     *  \return
     *      True if !(arg1 || ... || argn) == true
     */
    template<typename... T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool none(T const&... args) {
        return !any(args...);
    }

    /**
     *  \brief  Generic test equality of all args.
     *
     *  \return
     *      True if arg1 == ... == argn
     */
    template <typename... T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool all_equal(T const&... args) {
        if constexpr (sizeof...(T) < 2) {
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
    template <typename T, typename... Ts> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool all_within(ATTR_MAYBE_UNUSED T min,
                                            ATTR_MAYBE_UNUSED T max,
                                            const Ts&... ts)
    {
        if constexpr (sizeof...(Ts) == 0) {
            return true;
        } else {
            if (max < min) // Allow range from higher to lower
                std::swap(min, max);
            return ((min <= ts && ts <= max) && ...);
        }
    }

    /**
     *  \brief  Generic test if all elements in the range
     *          are within [min, max] (inclusive).
     *
     *  \return
     *      True if (min <= start[0] && start[0] <= max) && ...
     */
    template <
        typename Iterator,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool within(T min, T max, Iterator start, Iterator end) {
        if (max < min) // Allow range from higher to lower
            std::swap(min, max);

        return std::all_of(start, end, [min, max](const T& x){
            return min <= x && x <= max;
        });
    }

    /**
     *  \brief  Generic test if all elements in the container
     *          are within [min, max] (inclusive).
     *
     *  \return
     *      True if (min <= arg1 && arg1 <= max) && ...
     */
    template <typename T, typename Container> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool within(T min, T max, const Container& cont) {
        static_assert (utils::traits::is_iterable_v<Container>,
                       "utils::algorithm::within: Container must have iterator support.");
        return utils::algorithm::within(min, max, std::begin(cont), std::end(cont));
    }

    /**
     *  \brief  Calculate the sum of all elements between \p start and \p end.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \return Returns the sum of the elements with type `Iterator::value_type`.
     */
    template <
        typename Iterator,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr T sum(Iterator start, Iterator end) {
        return std::accumulate(start, end, T{0});
    }

    /**
     *  \brief  Calculate the sum of all elements inside the container \p cont.
     *
     *  \param cont
     *      The container to calculate the sum for.
     *  \return Returns the sum of the elements with type `Container::value_type`.
     */
    template <typename Container> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto sum(const Container& cont) {
        static_assert (utils::traits::is_iterable_v<Container>,
                       "utils::algorithm::sum: Container must have iterator support.");
        return utils::algorithm::sum(std::begin(cont), std::end(cont));
    }

    /**
     *  \brief  Calculate the product of all elements between \p start and \p end.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \return Returns the product of the elements with type `Iterator::value_type`.
     */
    template <
        typename Iterator,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr T product(Iterator start, Iterator end) {
        return start != end
             ? std::accumulate(start, end, T{1}, [](const T& x, const T& y){ return x * y; })
             : T{0};
    }

    /**
     *  \brief  Calculate the product of all elements inside the container \p cont.
     *
     *  \param cont
     *      The container to calculate the sum for.
     *  \return Returns the product of the elements with type `Container::value_type`.
     */
    template <typename Container> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto product(const Container& cont) {
        static_assert (utils::traits::is_iterable_v<Container>,
                       "utils::algorithm::product: Container must have iterator support.");
        return utils::algorithm::product(std::begin(cont), std::end(cont));
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
    void repeat(F&& fn, Args&&... args) {
        static_assert(std::is_invocable_v<F, Args...>,
                      "utils::algorithm::repeat: Callable function required.");
        for (size_t i = 0; i < times; i++) {
            std::invoke(std::forward<F>(fn), std::forward<Args>(args)...);
        }
    }

    /**
     *  \brief  Enumerate wrapper for containers.
     *          Return an iterator that also holds an index, starting at \p start_t.
     *
     *          Does the same as the one from ::iter::enumerate, but includes
     *          an offsetable start index.
     *
     *  \param  iterable
     *      The container to wrap.
     *  \param  start_i
     *      The starting index when looping over the container.
     *  \return Returns a wrapper to enumarete over \p iterable.
     */
    template <
        typename T,
        typename TIter = decltype(std::begin(std::declval<T>())),
        typename       = decltype(std::end  (std::declval<T>()))
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto enumerate(T&& iterable, size_t start_i = 0) {
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
            size_t start;
            auto begin() { return iterator{ start, std::begin(iterable) }; }
            auto end()   { return iterator{ start, std::end  (iterable) }; }
        };

        return iterable_wrapper{ std::forward<T>(iterable), start_i };
    }
}

#endif // UTILS_ALGORITHM_HPP
