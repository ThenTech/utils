#ifndef UTILS_BITS_HPP
#define UTILS_BITS_HPP

#include <cstdint>

/*
 *  Refer to: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
 */
#ifdef _MSC_VER
    #define UTILS_BITS_CLZ_ULL  __lzcnt
#else
    #define UTILS_BITS_CLZ_ULL  __builtin_clzll
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
    template<class T>
    [[maybe_unused]] static inline uint_fast8_t ffs(T value ) {
        return value >= 0
             ? uint_fast8_t(64 - UTILS_BITS_CLZ_ULL(uint64_t(value)))
             : utils::bits::size_of<T>();
    }

    /**
     *  @brief  Round the given bits to the next byte.
     *
     *  @param  bits
     *      The amount of bits to round to a byte.
     *  @return Returns the next byte if bits has 1-7 surplus bits
     *          or the current byte if no surplus bits.
     */
    [[maybe_unused]] static inline size_t round_to_byte(size_t bits) {
        return (bits + (8u - (bits % 8u)) % 8u) / 8u;
    }

    template<class T>
    [[maybe_unused]] static inline T shift_signed(size_t value, size_t src_bits) {
        const size_t bit_length = utils::bits::size_of<T>() - src_bits;
        return T(value << bit_length) >> bit_length;
    }

    /**
     *  \brief  Determine the amount of bits needed to represent the given T.
     *
     *  \param  value
     *      The T value to check.
     *  \return
     *      Returns the amount of bits required to represent the value if reshifted to size_of<T> bits.
     */
    template<class T>
    [[maybe_unused]] static inline uint_fast8_t bits_needed(T value) {
        uint_fast8_t bits = 0;

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

#endif // UTILS_BITS_HPP
