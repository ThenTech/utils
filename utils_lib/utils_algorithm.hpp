#ifndef UTILS_ALGORITHM_HPP
#define UTILS_ALGORITHM_HPP

#include "external/cppitertools/itertools.hpp"
#include "utils_compiler.hpp"
#include "utils_traits.hpp"

#include <algorithm>
#include <functional>
#include <numeric>
#include <optional>
#include <tuple>


namespace utils::algorithm {
    /*
     *  Wrap CPPItertools as ::iter
     *  Reference: https://github.com/ryanhaining/cppitertools (04/08/2019)
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
     *          If \p container has `.constains` size_t(-1) is returned if found
     *          or std::nullopt othertwise.
     */
    template<
        typename C, typename T,
        typename = typename std::enable_if_t<utils::traits::is_container<C>::value>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr utils::traits::found_t contains(const C& container, const T& item) {
        if constexpr (utils::traits::has_contains_v<C, T>) {
            if (const auto check = container.contains(item)) {
                return { -1 };
            }
        } else if constexpr (utils::traits::has_find_v<C, T>) {
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
            if (HEDLEY_UNLIKELY(max < min)) // Allow range from higher to lower
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
        if (HEDLEY_UNLIKELY(max < min)) // Allow range from higher to lower
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
        static_assert(utils::traits::is_iterable_v<Container>,
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
        static_assert(utils::traits::is_iterable_v<Container>,
                      "utils::algorithm::product: Container must have iterator support.");
        return utils::algorithm::product(std::begin(cont), std::end(cont));
    }

    /**
     *  \brief  Reverse all elements between \p start and \p end.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     */
    template <
        typename Iterator,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED
    static inline constexpr void reverse(Iterator start, Iterator end) {
        if (start != end) {
            std::reverse(start, end);
        }
    }

    /**
     *  \brief  Reverse all elements inside the container \p cont.
     *
     *  \param cont
     *      The container to reverse.
     */
    template <typename Container> ATTR_MAYBE_UNUSED
    static inline constexpr void reverse(Container& cont) {
        static_assert(utils::traits::is_iterable_v<Container>,
                      "utils::algorithm::reverse: Container must have iterator support.");
        utils::algorithm::reverse(std::begin(cont), std::end(cont));
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
        static_assert(utils::traits::is_invocable_v<F, Args...>,
                      "utils::algorithm::repeat: Callable function required.");
        for (size_t i = 0; i < times; i++) {
            std::invoke(std::forward<F>(fn), std::forward<Args>(args)...);
        }
    }

    /**
     *  \brief  Wrapper to call std::for_each with iterators.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \param  fn
     *      The action to call. Must be invocable with Iterator::value_type.
     */
    template <
        typename Iterator,
        typename F,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED
    static inline constexpr auto for_each(Iterator start, Iterator end, F&& fn) {
        static_assert(utils::traits::is_invocable_v<F, T>,
                      "utils::algorithm::for_each: Callable function required.");

        return std::for_each(start, end, std::forward<F>(fn));
    }

    /**
     *  \brief  Wrapper to call std::for_each with a Container instead of iterators.
     *
     *  \param cont
     *      The container to apply for_each to.
     *  \param  fn
     *      The action to call. Must be invocable with Container::value_type.
     */
    template <typename Container, typename F> ATTR_MAYBE_UNUSED
    static inline constexpr auto for_each(const Container& cont, F&& fn) {
        static_assert(utils::traits::is_iterable_v<Container>,
                      "utils::algorithm::for_each: Container must have iterator support.");
        return utils::algorithm::for_each(std::begin(cont), std::end(cont), std::forward<F>(fn));
    }

    /**
     *  \brief  Wrapper to call std::min_element with iterators.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \param  fn_compare
     *      The compare function to call. Must be invocable with Iterator::value_type.
     *  \return Returns the smallest element.
     */
    template <
        typename Iterator,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename F = typename std::less<T>,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto min_element(Iterator start, Iterator end, F&& fn_compare = F{}) {
        static_assert(utils::traits::is_invocable_v<F, T, T>,
                      "utils::algorithm::min_element: Callable function required.");
        return std::min_element(start, end, std::forward<F>(fn_compare));
    }

    /**
     *  \brief  Wrapper to call std::min_element with a Container instead of iterators.
     *
     *  \param cont
     *      The container to apply min_element to.
     *  \param  fn
     *      The compare function to call. Must be invocable with Container::value_type.
     *  \return Returns the smallest element.
     */
    template <
        typename Container,
        typename T = typename Container::value_type,
        typename F = typename std::less<T>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto min_element(const Container& cont, F&& fn_compare = F{}) {
        static_assert(utils::traits::is_iterable_v<Container>,
                      "utils::algorithm::min_element: Container must have iterator support.");
        return utils::algorithm::min_element(std::begin(cont), std::end(cont), std::forward<F>(fn_compare));
    }

    /**
     *  \brief  Wrapper to call std::max_element with iterators.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \param  fn_compare
     *      The compare function to call. Must be invocable with Iterator::value_type.
     *  \return Returns the largest element.
     */
    template <
        typename Iterator,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename F = typename std::less<T>,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto max_element(Iterator start, Iterator end, F&& fn_compare = F{}) {
        static_assert(utils::traits::is_invocable_v<F, T, T>,
                      "utils::algorithm::max_element: Callable function required.");
        return std::max_element(start, end, std::forward<F>(fn_compare));
    }

    /**
     *  \brief  Wrapper to call std::max_element with a Container instead of iterators.
     *
     *  \param cont
     *      The container to apply min_element to.
     *  \param  fn
     *      The compare function to call. Must be invocable with Container::value_type.
     *  \return Returns the largest element.
     */
    template <
        typename Container,
        typename T = typename Container::value_type,
        typename F = typename std::less<T>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto max_element(const Container& cont, F&& fn_compare = F{}) {
        static_assert(utils::traits::is_iterable_v<Container>,
                      "utils::algorithm::max_element: Container must have iterator support.");
        return utils::algorithm::max_element(std::begin(cont), std::end(cont), std::forward<F>(fn_compare));
    }

    /**
     *  \brief  Wrapper to call std::is_sorted with iterators to check ascending order.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \param  fn_compare
     *      The compare function to call. Must be invocable with Iterator::value_type.
     *  \return Returns true if the elements are sorted from low to high.
     */
    template <
        typename Iterator,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename F = typename std::less<T>,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool is_ascending(Iterator start, Iterator end, F&& fn_compare = F{}) {
        return std::is_sorted(start, end, std::forward<F>(fn_compare));
    }

    /**
     *  \brief  Wrapper to call std::is_sorted with a Container instead of iterators
     *          to check ascending order.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \param  fn_compare
     *      The compare function to call. Must be invocable with Iterator::value_type.
     *  \return Returns true if the elements are sorted from low to high.
     */
    template <
        typename Container,
        typename T = typename Container::value_type,
        typename F = typename std::less<T>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool is_ascending(const Container& cont, F&& fn_compare = F{}) {
        static_assert(utils::traits::is_iterable_v<Container>,
                      "utils::algorithm::is_ascending: Container must have iterator support.");
        return std::is_sorted(std::begin(cont), std::end(cont), std::forward<F>(fn_compare));
    }

    /**
     *  \brief  Wrapper to call std::is_sorted with iterators to check descending order.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \param  fn_compare
     *      The compare function to call. Must be invocable with Iterator::value_type.
     *  \return Returns true if the elements are sorted from high to low.
     */
    template <
        typename Iterator,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename F = typename std::greater<T>,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool is_descending(Iterator start, Iterator end, F&& fn_compare = F{}) {
        return std::is_sorted(start, end, std::forward<F>(fn_compare));
    }

    /**
     *  \brief  Wrapper to call std::is_sorted with a Container instead of iterators
     *          to check descending order.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \param  fn_compare
     *      The compare function to call. Must be invocable with Iterator::value_type.
     *  \return Returns true if the elements are sorted from high to low.
     */
    template <
        typename Container,
        typename T = typename Container::value_type,
        typename F = typename std::greater<T>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool is_descending(const Container& cont, F&& fn_compare = F{}) {
        static_assert(utils::traits::is_iterable_v<Container>,
                      "utils::algorithm::is_descending: Container must have iterator support.");
        return std::is_sorted(std::begin(cont), std::end(cont), std::forward<F>(fn_compare));
    }

    namespace sort {
        /**
         *  \brief  Basic insertion sort with iterators.
         *
         *  \param  start
         *      The start iterator to begin from.
         *  \param  end
         *      The end iterator to stop at.
         */
        template <typename Iterator> ATTR_MAYBE_UNUSED
        static inline constexpr void insertion(Iterator start, Iterator end) {
            for (auto it = start; it != end; ++it) {
                std::rotate(std::upper_bound(start, it, *it), it, std::next(it));
            }
        }

        /**
         *  \brief  Quicksort using STL algos, with iterators.
         *
         *  \param  start
         *      The start iterator to begin from.
         *  \param  end
         *      The end iterator to stop at.
         *  \param  fn_compare
         *      The compare function to call. Must be invocable with Iterator::value_type.
         */
        template <
            typename Iterator,
            typename T = typename std::iterator_traits<Iterator>::value_type,
            typename F = typename std::less<T>
        > ATTR_MAYBE_UNUSED
        static constexpr void quick(Iterator start, Iterator end, F&& fn_compare = F{}) {
            const auto length = std::distance(start, end);
            if (HEDLEY_UNLIKELY(length <= 1)) return;

            const auto pivot = std::next(start, length / 2);
            std::nth_element(start, pivot, end, fn_compare);
            utils::algorithm::sort::quick(start, pivot, fn_compare);
            utils::algorithm::sort::quick(pivot, end, fn_compare);
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

    /**
     *  \brief  The any_tie struct. Use to tie a result back into itself
     *          for any type that supports structured bindings access.
     *
     *          Example:
     *              any_tie(value1, value2) = value2;
     *
     *          because this does not work:
     *              auto& [value1, value2] = value2;
     *
     *  \ref    Structured bindings uncovered - Dawid Zalewski [ C++ on Sea 2020 ]
     */
    template<typename ...Types>
    struct any_tie {
        std::tuple<Types&...> _tuple;

        any_tie(Types&... values) noexcept : _tuple{values...} {}

        template<typename TupleLike>
        any_tie& operator=(TupleLike&& tl) {
            constexpr auto size = std::tuple_size_v<utils::traits::remove_cvref_t<TupleLike>>;
            this->assign(std::forward<TupleLike>(tl), std::make_index_sequence<size>());
            return *this;
        }

        private:
            template<typename TupleLike, size_t... Idx>
            void assign(TupleLike&& tl, std::index_sequence<Idx...>) {
                this->_tuple = std::forward_as_tuple(std::get<Idx>(std::forward<TupleLike>(tl))...);
            }
    };
}

#endif // UTILS_ALGORITHM_HPP
