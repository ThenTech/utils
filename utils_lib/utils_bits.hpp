#ifndef UTILS_BITS_HPP
#define UTILS_BITS_HPP

#include "utils_catch.hpp"
#include "utils_traits.hpp"

#include <cstdint>
#include <limits>


/*
 *  Refer to: https://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html
 */
#ifdef _MSC_VER
    #define UTILS_BITS_CLZ_ULL  __lzcnt
    #define UTILS_BITS_FFS_LL   utils::bits::internal::_ffs_template
    #define UTILS_BITS_CNT_LL   utils::bits::internal::_cnt_template
#else
    #define UTILS_BITS_CLZ_ULL  __builtin_clzll
    #define UTILS_BITS_FFS_LL   __builtin_ffsll
    #define UTILS_BITS_CNT_LL   __builtin_popcountll
#endif

#define UTILS_BITS_ASSERT_SHIFT_SIGNED_SIZE 0


namespace utils::bits {
    namespace internal {
        #ifdef _MSC_VER
            template <typename T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
            static inline uint_fast32_t _ffs_template(T x) {
                if constexpr (x == 0) return 0u;
                uint_fast32_t r = 1;
                while ((x & 1) == 0)
                    x >>= 1, ++r;
                return r;
            }

            template <typename T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
            static inline uint_fast32_t _cnt_template(T x) {
                if constexpr (x == 0) return 0u;
                uint_fast32_t r = 0;
                while (x)
                    r += (x & 1) == 1, x >>= 1;
                return r;
            }
        #endif
    }

    /**
     * \brief   Return the size in bits of the given type.
     *          Uses sizeof(T) * CHAR_BITS or
     *          std::numeric_limits<uint8_t>::digits
     */
    template<class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    inline constexpr size_t size_of(void) {
        return sizeof(T) * std::numeric_limits<uint8_t>::digits;
    }

    /**
     *  \brief  Generic bitwise AND all args.
     *
     *  \return Return arg1 & ... & argn
     */
    template<
        typename... T,
        typename C = std::common_type_t<T...>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr C and_all(T const&... args) {
        static_assert(std::is_integral_v<C>, "utils::bits::and_all: Integrals required.");
        return (... & args);
    }

    /**
     *  \brief  Generic bitwise OR all args.
     *
     *  \return Return arg1 | ... | argn
     */
    template<
        typename... T,
        typename C = std::common_type_t<T...>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr C or_all(T const&... args) {
        static_assert(std::is_integral_v<C>, "utils::bits::or_all: Integrals required.");
        return (... | args);
    }

    /**
     *  \brief  Generic bitwise XOR all args.
     *
     *  \return Return arg1 ^ ... ^ argn
     */
    template<
        typename... T,
        typename C = std::common_type_t<T...>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr C xor_all(T const&... args) {
        static_assert(std::is_integral_v<C>, "utils::bits::xor_all: Integrals required.");
        return (... ^ args);
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
    template<class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr uint_fast32_t ffs(const T value) {
        static_assert(std::is_integral_v<T>, "utils::bits::ffs: Integral required.");
        using uT = typename std::make_unsigned<T>::type;
        return UTILS_BITS_FFS_LL(uT(value));
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
    template<class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr uint_fast32_t msb(const T value) {
        static_assert(std::is_integral_v<T>, "utils::bits::msb: Integral required.");
        using uT = typename std::make_unsigned<T>::type;
        return uint_fast32_t(64 - UTILS_BITS_CLZ_ULL(uT(value) | 1)) - (value == 0);
    }

    /**
     * \brief  Popcount
     *         This function counts the amount of set bits.
     *
     * \param  value
     *      Value to count the set bits in.
     * \retval bitIndex
     *      The amount of bits that are `1`.
     */
    template<class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr uint_fast32_t popcount(const T value) {
        static_assert(std::is_integral_v<T>, "utils::bits::popcount: Integral required.");
        using uT = typename std::make_unsigned<T>::type;
        return UTILS_BITS_CNT_LL(uT(value));
    }

    /**
     *  \brief  Bitwise rotate the given \p value to the left by \p n bits.
     *
     *  \param  value
     *      The value to rotate.
     *  \param  n
     *      The amount of bit places to rotate left with.
     *  \return The result will be of type T but with \p bit_length bits used,
     *          other bits exceeding the length will become zeroes.
     *          The value will be shifted to the left, and any bits exceeding
     *          \p bit_length will be appended to the right.
     */
    template <
        class T,
        size_t bit_length = utils::bits::size_of<T>()
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr T rotl(const T value, const int_fast32_t n = 1) {
        static_assert(std::is_integral_v<T>, "utils::bits::rotl: Integral required.");

        constexpr T mask_size = bit_length == 64 ? uint64_t(~0ull) : ~(uint64_t(~0) << bit_length);
        const     T mask_data = ~(uint64_t(~0) << n);
        return ((value << n) & mask_size) | ((value >> (bit_length - n)) & mask_data);
    }

    /**
     *  \brief  Bitwise rotate the given \p value to the right by \p n bits.
     *
     *  \param  value
     *      The value to rotate.
     *  \param  n
     *      The amount of bit places to rotate right with.
     *  \return The result will be of type T but with \p bit_length bits used,
     *          other bits exceeding the length will become zeroes.
     *          The value will be shifted to the right, and any bits shifted out
     *          will be appended to the left starting at \p bit_length.
     */
    template <
        class T,
        size_t bit_length = utils::bits::size_of<T>()
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr T rotr(const T value, const int_fast32_t n = 1) {
        static_assert(std::is_integral_v<T>, "utils::bits::rotr: Integral required.");

        const     T mask_right = ~(uint64_t(~0) << (bit_length - n));
        const     T mask_data  = ~(uint64_t(~0) << n);
        return ((value >> n) & mask_right) | ((value & mask_data) << (bit_length - n));
    }

    /**
     *  \brief  Determine if given value is a power of 2.
     *
     *  \param  value
     *      The value to check.
     *  \return Returns true if value is the result of (1 << x).
     */
    template<class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool is_power_of_2(const T value) {
        static_assert(std::is_integral_v<T>, "utils::bits::is_power_of_2: Integral required.");
        return value && !(value & (value - 1));
    }

    /**
     *  \brief  Check if given value is an odd number.
     *
     *  \param  value
     *      The value to check.
     *  \return Returns true if the value is odd.
     */
    template<class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool is_odd(const T value) {
        static_assert(std::is_integral_v<T>, "utils::bits::is_odd: Integral required.");
        return (value & T(1)) != T(0);
    }

    /**
     *  \brief  Check if given value is an even number.
     *
     *  \param  value
     *      The value to check.
     *  \return Returns true if the value is even (divisable by 2).
     */
    template<class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool is_even(const T value) {
        static_assert(std::is_integral_v<T>, "utils::bits::is_even: Integral required.");
        return !utils::bits::is_odd(value);
    }

    /**
     *  \brief  Round the given value to the next multiple of \p multiple,
     *          greater or equal to \p value itself.
     *
     *  \param  value
     *      The number to round.
     *  \param  multiple
     *      The multiple to round to.
     *  \return Returns the nearest multiple greater than \p value.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr int64_t round_to_multiple(const int64_t value, const int64_t multiple) noexcept(false) {
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
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr size_t round_to_byte(const size_t bits) {
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
    template<
        class T,
        size_t Tlen = utils::bits::size_of<T>()
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline T shift_signed(const size_t value, const size_t src_bits) {
        #if UTILS_BITS_ASSERT_SHIFT_SIGNED_SIZE
            // ASSERT that src_bits is not bigger than sizeof(T)
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
    template<class T, uint_fast8_t bits> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline T extend_sign(const size_t value) {
        static_assert(bits > 0, "utils::bits::extend_sign: Amount of bits greater than 0 required.");
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
    template<class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline uint_fast32_t bits_needed(T value) {
        static_assert(std::is_integral_v<T>, "utils::bits::bits_needed: Integral required.");

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
#undef UTILS_BITS_FFS_LL
#endif // UTILS_BITS_HPP
