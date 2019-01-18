#ifndef UTILS_MISC_HPP
#define UTILS_MISC_HPP

#include <type_traits>
#include <cmath>

namespace utils::misc {
    /**
     *  \brief   Cast enum type to underlining data type.
     *  \param   e
     *      The enum value to cast.
     */
    template <typename E>
    constexpr inline auto to_underlying(E e) noexcept {
        return static_cast<std::underlying_type_t<E>>(e);
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
    inline bool epsilon_equals(double x, double y, double epsilon = 1e-4) {
        return std::abs(x - y) < epsilon;
    }
}

#endif // UTILS_MISC_HPP
