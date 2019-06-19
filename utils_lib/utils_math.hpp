#ifndef UTILS_MATH_HPP
#define UTILS_MATH_HPP

#include "utils_catch.hpp"
#include "utils_traits.hpp"
#include "utils_algorithm.hpp"

#include <cmath>
#include <numeric>


namespace utils::math {
    /**
     *  \brief  Return the sign of the given value.
     *
     *          Refer to: from https://stackoverflow.com/questions/1903954
     *
     *  \param  value
     *      The value to return the sign from.
     *  \return
     *      Returns -1, 0, or 1 according to the sign of value.
     */
    template <class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr T sign(const T& value) {
        static_assert(std::is_signed_v<T>, "utils::math::sign: Signed type required.");
        return (T(0) < value) - (value < T(0));
    }

    /**
     *  \brief  Generic sum all args.
     *
     *  \return
     *      Returns arg1 + ... + argn
     */
    template<typename... T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto sum(T const&... args) {
        return (args + ... + 0);
    }

    /**
     *  \brief  Generic multiply all args.
     *
     *  \return
     *      Returns arg1 * ... * argn
     */
    template<typename... T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto product(T const&... args) {
        return (args * ... * 1);
    }

    /**
     *  \brief  Generic determine minimum of all args.
     *
     *  \return
     *      Returns std::min(arg1, std::min(..., argn))
     */
    template<typename T, typename... Args> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto min(T const& first, T const& second, Args const&... args) {
        if constexpr (sizeof...(Args) == 0) {
            return std::min(first, second);
        } else {
            return first < second ? utils::math::min(first , args...)
                                  : utils::math::min(second, args...);
        }
    }

    /**
     *  \brief  Generic determine maximum of all args.
     *
     *  \return
     *      Returns std::max(arg1, std::max(..., argn))
     */
    template<typename T, typename... Args> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr auto max(T const& first, T const& second, Args const&... args) {
        if constexpr (sizeof...(Args) == 0) {
            return std::max(first, second);
        } else {
            return first > second ? utils::math::max(first , args...)
                                  : utils::math::max(second, args...);
        }
    }

    /**
     *  \brief  Calculate \p value raised to the power \p Exp,
     *          where \p Exp must be an unsigned int.
     *
     *          The result is calulated by unpacking \p value times itself \p Exp times.
     *
     *  \tparam Exp
     *      The power to raise to as template parameter. Default is squared.
     *  \param  value
     *      The value to raise.
     *  \return Returns the result of \p Exp times multiplying \value with itself.
     */
    template <size_t Exp = 2ull, typename T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr T pow(ATTR_MAYBE_UNUSED const T value) {
        if constexpr (Exp == 0ull) {
            return T(1);
        } else if constexpr (Exp == 1ull) {
            return value;
        } else {
            return value * utils::math::pow<Exp - 1>(value);
        }
    }

    /**
     *  brief   Greatest Common Divisor (Euclid)
     *          Find the GCD of \p x and \p y:
     *              - will cast to size_t if their common type is not integral;
     *              - will take absolute value.
     *
     *  param   x
     *      The first number to check.
     *  param   y
     *      The second number to check.
     *  return  Returns the GCD between \p x and \p y.
     */
    template<
        typename A,
        typename B,
        typename C = typename std::common_type<A, B>::type
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr C gcd(const A x, const B y) noexcept {
        if constexpr (std::is_integral_v<C>) {
            return std::gcd(static_cast<C>(x), static_cast<C>(y));
        } else {
            return static_cast<C>(std::gcd(static_cast<int64_t>(x), static_cast<int64_t>(y)));
        }
    }

    /**
     *  brief   Least Common Multiple
     *          Find the LCM of \p x and \p y:
     *              - will cast to size_t if their common type is not integral;
     *              - will take absolute value.
     *
     *  param   x
     *      The first number to check.
     *  param   y
     *      The second number to check.
     *  return  Returns the LCM of \p x and \p y.
     */
    template<
        typename A,
        typename B,
        typename C = typename std::common_type<A, B>::type
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr C lcm(const A x, const B y) {
        if constexpr (std::is_integral_v<C>) {
            return std::lcm(static_cast<C>(x), static_cast<C>(y));
        } else {
            return static_cast<C>(std::lcm(static_cast<int64_t>(x), static_cast<int64_t>(y)));
        }
    }

    /**
     *  brief   Least Common Multiple
     *          Find the LCM of \p x, \p y and every other argument
     *          by calling `utils::math::lcm` on the first two and
     *          chining that result with the other arguments.
     *
     *  param   x
     *      The first number to check.
     *  param   y
     *      The second number to check.
     *  param   ...args
     *      The other numbers to check.
     *  return  Returns the LCM of \p x, \p y and every other argument.
     */
    template<
        typename A,
        typename B,
        typename ...Args,
        typename C = typename std::common_type<A, B, Args...>::type
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr C lcm_chain(const A x, const B y, const Args... args) {
        if constexpr (sizeof...(Args) == 0) {
            return utils::math::lcm(x, y);
        } else {
            return utils::math::lcm_chain(utils::math::lcm(x, y), args...);
        }
    }

    /**	\brief	Check if the given values are equal.
     *          For integral types, `==` is used, while floating point types
     *          use a comparison with diff(x, y) and epsilon.
     *
     *	\param	x
     *		The first value to compare.
     *	\param	y
     *		The second value to compare.
     *	\param	epsilon
     *		The precision to compare with (standard deviation of 1e-4 or 0.0001).
     *
     *	\return	bool
     *		Returns whether x equals y within the given epsilon precision.
     */
    template<typename T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool epsilon_equals(const T x, const T y, ATTR_MAYBE_UNUSED const double epsilon = 1e-4) {
        if constexpr (std::is_integral_v<T>) {
            return x == y;
        } else {
            // TODO Use std::numeric_limits<T>::epsilon() ?
            return std::abs(x - y) < epsilon;
        }
    }

    namespace stats {
        /**
         *  \brief  Calculate sample mean between \p first and \p last.
         *          (`std::distance(first, last)` elements)
         *
         *  \param  first
         *      The iterator to start from.
         *  \param  last
         *      The end iterator.
         *  \return Returns the mean of the items [first, ..., last].
         */
        template<
            typename Iterator,
            typename DiffType = typename std::iterator_traits<Iterator>::difference_type,
            typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
        > ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static inline double mean(Iterator first, Iterator last) {
            const DiffType size = std::distance(first, last);
            ASSERT(size > 0);

            return double(utils::algorithm::sum(first, last)) / size;
        }

        /**
         *  \brief  Calculate sample mean of \p cont.
         *
         *  \param  cont
         *      The container to calculate from.
         *  \return Return the sample mean of \p cont.
         */
        template<typename Container> ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static inline double mean(const Container& cont) {
            static_assert(utils::traits::is_iterable_v<Container>,
                          "utils::algorithm::mean: Container must have iterator support.");
            return utils::math::stats::mean(std::begin(cont), std::end(cont));
        }

        /**
         *  \brief  Calculate sample variance between \p first and \p last.
         *          (`std::distance(first, last)` elements)
         *
         *  \param  first
         *      The iterator to start from.
         *  \param  last
         *      The end iterator.
         *  \return Returns the variance of the items [first, ..., last].
         */
        template<
            typename Iterator,
            typename DiffType = typename std::iterator_traits<Iterator>::difference_type,
            typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
        > ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static double variance(Iterator first, Iterator last) {
            const DiffType size = std::distance(first, last);
            ASSERT(size > 1);

            const double mean   = utils::math::stats::mean(first, last);
            const double sq_sum = std::inner_product(first, last, first, 0.0,
                [    ](const double& x, const double& y) { return x + y; },
                [mean](const double& x, const double& y) { return (x - mean) * (y - mean); });

            return sq_sum / (size - 1);
        }

        /**
         *  \brief  Calculate sample variance of \p cont.
         *
         *  \param  cont
         *      The container to calculate from.
         *  \return Return the sample variance of \p cont.
         */
        template<typename Container> ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static inline double variance(const Container& cont) {
            static_assert(utils::traits::is_iterable_v<Container>,
                          "utils::algorithm::variance: Container must have iterator support.");
            return utils::math::stats::variance(std::begin(cont), std::end(cont));
        }

        /**
         *  \brief  Calculate sample stddev between \p first and \p last.
         *          (`std::distance(first, last)` elements)
         *
         *  \param  first
         *      The iterator to start from.
         *  \param  last
         *      The end iterator.
         *  \return Returns the stddev of the items [first, ..., last].
         */
        template<
            typename Iterator,
            typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
        > ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static double stddev(Iterator first, Iterator last) {
            return std::sqrt(utils::math::stats::variance(first, last));
        }

        /**
         *  \brief  Calculate sample stddev of \p cont.
         *
         *  \param  cont
         *      The container to calculate from.
         *  \return Return the sample stddev of \p cont.
         */
        template<typename Container> ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static inline double stddev(const Container& cont) {
            static_assert(utils::traits::is_iterable_v<Container>,
                          "utils::algorithm::stddev: Container must have iterator support.");
            return utils::math::stats::stddev(std::begin(cont), std::end(cont));
        }
    }
}

#endif // UTILS_MATH_HPP
