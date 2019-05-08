#ifndef UTILS_MATH_HPP
#define UTILS_MATH_HPP

#include <cmath>

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
    template <class T> [[maybe_unused]]
    static inline constexpr int sign(const T& value) {
        return (T(0) < value) - (value < T(0));
    }

    /**	\brief	Check if the given doubles are equal.
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
    [[maybe_unused]]
    static inline bool epsilon_equals(double x, double y, double epsilon = 1e-4) {
        // TODO templated with enable_if decimal else just == compare
        //      and std::numeric_limits<T>::epsilon()*100 ?
        return std::abs(x - y) < epsilon;
    }
}

#endif // UTILS_MATH_HPP
