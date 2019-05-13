#ifndef UTILS_MATH_HPP
#define UTILS_MATH_HPP

#include "utils_catch.hpp"

#include <cmath>

namespace utils::math {
    namespace internal {
        /**
         *  brief   Greatest Common Divisor (Euclid)
         *          Find the GCD of \p x and \p y, with \p T integral type.
         *
         *  param   x
         *      The first number to check.
         *  param   y
         *      The second number to check.
         *  return  Returns the GCD between \p x and \p y.
         */
        template<
            typename T,
            typename std::enable_if<std::is_integral_v<T>, int>::type = 0
        > [[maybe_unused]]
        static inline constexpr T _gcd_recur(const T x, const T y) noexcept {
            return (y == T(0) ? x : _gcd_recur(y, x % y));
        }

        /**
         *  brief   Least Common Multiple
         *          Find the LCM of \p x and \p y, with \p T integral type.
         *
         *  param   x
         *      The first number to check.
         *  param   y
         *      The second number to check.
         *  return  Returns the LCM of \p x and \p y.
         */
        template<
            typename T,
            typename std::enable_if<std::is_integral_v<T>, int>::type = 0
        > [[maybe_unused]]
        static inline constexpr T _lcm(const T x, const T y) noexcept {
            return x * (y / _gcd_recur(x, y));
        }
    }

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
    template <class T> [[maybe_unused]]
    static inline constexpr T sign(const T& value) {
        static_assert(std::is_signed_v<T>, "utils::math::sign: Signed type required.");
        return (T(0) < value) - (value < T(0));
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
    > [[maybe_unused]]
    static inline constexpr C gcd(const A x, const B y) noexcept {
        if constexpr(std::is_integral_v<C>) {
            return utils::math::internal::_gcd_recur(static_cast<C>(std::abs(x)),
                                                     static_cast<C>(std::abs(y)));
        } else {
            return utils::math::internal::_gcd_recur(static_cast<size_t>(std::abs(x)),
                                                     static_cast<size_t>(std::abs(y)));
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
    > [[maybe_unused]]
    static inline constexpr C lcm(const A x, const B y) {
        ASSERT(x != C(0) && y != C(0));

        if constexpr (std::is_integral_v<C>) {
            return utils::math::internal::_lcm(static_cast<C>(std::abs(x)),
                                               static_cast<C>(std::abs(y)));
        } else {
            return utils::math::internal::_lcm(static_cast<size_t>(std::abs(x)),
                                               static_cast<size_t>(std::abs(y)));
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
    > [[maybe_unused]]
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
    template<typename T> [[maybe_unused]]
    static inline constexpr bool epsilon_equals(const T x, const T y, const double epsilon = 1e-4) {
        if constexpr (std::is_integral_v<T>) {
            (void) epsilon;
            return x == y;
        } else {
            // TODO Use std::numeric_limits<T>::epsilon() ?
            return std::abs(x - y) < epsilon;
        }
    }
}

#endif // UTILS_MATH_HPP
