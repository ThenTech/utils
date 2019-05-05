#ifndef UTILS_BITS_HPP
#define UTILS_BITS_HPP

#include <cstdint>
#include <type_traits>

#include "external/catch_extra.hpp"

/*
 *  Refer to: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
 */
#ifdef _MSC_VER
    #define UTILS_BITS_CLZ_ULL  __lzcnt
#else
    #define UTILS_BITS_CLZ_ULL  __builtin_clzll
    #define UTILS_BITS_FFS_LL   __builtin_ffsll
#endif

namespace utils::bits {
    /**
     * \brief Return the size in bits of the given type.
     */
    template<class T>
    constexpr inline size_t size_of(void) {
        return sizeof(T) * 8u;
    }

    /**
     * \brief  Find First Set
     *         This function identifies the least significant index or position of the
     *         bits set to one in the word.
     *
     * \param  value
     *      Value to find least significant index
     * \retval bitIndex
     *      Index of least significat bit at one
     */
    template<class T> [[maybe_unused]]
    static inline uint_fast32_t ffs(T value) {
        return UTILS_BITS_FFS_LL(uint64_t(value));
    }

    /**
     * \brief  Find MSB Set (first from left)
     *         This function identifies the position of the MSB starting from LSB.
     *
     * \param  value
     *      Value to find msb index
     * \retval bitIndex
     *      Index of msb
     */
    template<class T> [[maybe_unused]]
    static inline uint_fast32_t msb(T value) {
        return value >= 0
             ? uint_fast32_t(64 - UTILS_BITS_CLZ_ULL(uint64_t(value) | 1)) - (value == 0)
             : utils::bits::size_of<T>();
    }

    template<class T> [[maybe_unused]]
    static inline bool is_power_of_2(T value) {
        static_assert(std::is_integral<T>::value, "utils::bits::is_power_of_2: Integral required.");
        return value && !(value & (value - 1));
    }

    [[maybe_unused]]
    static inline int64_t round_to_multiple(int64_t value, int64_t multiple) noexcept(false) {
        ASSERT(multiple);
        const int64_t isPositive = int64_t(value >= 0ll);
        return ((value + isPositive * (multiple - 1ll)) / multiple) * multiple;
    }

    /**
     *  @brief  Round the given bits to the next byte.
     *
     *  @param  bits
     *          The amount of bits to round to a byte.
     *  @return Returns the next byte if bits has 1-7 surplus bits
     *          or the current byte if no surplus bits.
     */
    [[maybe_unused]]
    static inline size_t round_to_byte(size_t bits) {
        return (bits + 7ull) / 8ull;
    }

    /**
     *  @brief  Create a signed value of type T
     *          with the given value that is only src_bits long.
     *
     *          This abuses the fact that right-shifting a signed number
     *          with an MSB of 1, will fill in the new MSBs as 1, making the
     *          original value signed in the new (bigger) size.
     *  @tparam T
     *          Target type (signed).
     *  @param  value
     *          The value to make signed in the available bits of T.
     *  @param  src_bits
     *          The amount of bits that are used in value.
     *  @return Returns a signed or unsigned number,
     *          depending on whether value[src_bits : 0] was considered signed.
     */
    template<class T, size_t Tlen = utils::bits::size_of<T>()> [[maybe_unused]]
    static inline T shift_signed(const size_t value, size_t src_bits) {
        #if 0 // ASSERT that src_bits is not bigger than sizeof(T)
            ASSERT(src_bits <= Tlen);
            const size_t bit_length = Tlen - src_bits;
            return T(value << bit_length) >> bit_length;
        #else // If src_bits is bigger than sizeof(T), don't shift
            const uint64_t bit_length = src_bits <= Tlen ? Tlen - src_bits : 0;
            return T(value << bit_length) >> bit_length;
        #endif
    }

    /**
     *  @brief  Same as shift_signed, but with known bit size
     *          of value at compile time.
     *  @tparam T
     *          Target type (signed).
     *  @tparam bits
     *          Amount of bits in value.
     *  @param  value
     *          The value to make signed in the available bits of T.
     *  @return Returns a signed or unsigned number,
     *          depending on whether value[src_bits : 0] was considered signed.
     */
    template<class T, uint_fast8_t bits> [[maybe_unused]]
    static inline T extend_sign(size_t value) {
        static_assert (bits > 0);
        struct { T value:bits; } s;
        return s.value = value;
    }

    /**
     *  \brief  Determine the amount of bits needed to represent the given T.
     *
     *  \param  value
     *      The T value to check.
     *  \return
     *      Returns the amount of bits required to represent the value if reshifted to size_of<T> bits.
     */
    template<class T> [[maybe_unused]]
    static inline uint_fast32_t bits_needed(T value) {
        uint_fast32_t bits = 0;

        // 1. Mask value with amount of current bits : (value & ((1 << bits) - 1))
        // 2. Shift left to size of 16 bits          : << (16 - bits)
        // 3. Cast to int16_t
        // 4. Shift back to original size
        // 5. Check if value represented with <bits> bits and re-shifted to 16 bits
        //    equals the starting value.
        // 6. Repeat until a minimal amount of bits has been found to represent the int16_t.
        //    (between 1 and 16)

        while (utils::bits::shift_signed<T>(value, ++bits) != value);

        return bits;
    }
}

#undef UTILS_BITS_CLZ_ULL
#endif // UTILS_BITS_HPP
