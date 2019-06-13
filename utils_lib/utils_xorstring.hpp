#ifndef UTILS_XORSTRING_HPP
#define UTILS_XORSTRING_HPP

#define XORSTR_USE 2

#if XORSTR_USE == 1
    /**
     *  Reference: https://github.com/JustasMasiulis/xorstr
     *
     *  Usage:
     *      const std::string str = xorstr_("xorstr_test_verify_error");
     */
    #include <immintrin.h>
    #include <cstdint>
    #include <cstddef>
    #include <utility>

    #define JM_XORSTR_DISABLE_AVX_INTRINSICS

    #define xorstr_(str)                                         \
        utils::xostring::make_xorstr(                            \
        []() {                                                   \
            struct {                                             \
                const std::decay_t<decltype(*str)>* value = str; \
            } a;                                                 \
            return a;                                            \
        }(),                                                     \
        std::make_index_sequence<sizeof(str) / sizeof(*str)>{},  \
        std::make_index_sequence<utils::xostring::detail::_buffer_size<sizeof(str)>()>{})

    #define xorstr(str) xorstr_(str).crypt_get()

    #ifdef _MSC_VER
        #define XORSTR_FORCEINLINE __forceinline
    #else
        #define XORSTR_FORCEINLINE __attribute__((always_inline))
    #endif

    // you can define this macro to get possibly faster code on gcc/clang
    // at the expense of constants being put into data section.
    #if !defined(XORSTR_ALLOW_DATA)
        // MSVC - no volatile
        // GCC and clang - volatile everywhere
        #if defined(__clang__) || defined(__GNUC__)
            #define XORSTR_VOLATILE volatile
        #endif
    #endif

    #ifndef XORSTR_VOLATILE
        #define XORSTR_VOLATILE
    #endif

    namespace utils::xostring {
        namespace detail {
            template<std::size_t S>
            struct unsigned_;

            template<>
            struct unsigned_<1> {
                using type = std::uint8_t;
            };
            template<>
            struct unsigned_<2> {
                using type = std::uint16_t;
            };
            template<>
            struct unsigned_<4> {
                using type = std::uint32_t;
            };

            template<auto C, auto...>
            struct pack_value_type {
                using type = decltype(C);
            };

            template<std::size_t Size>
            constexpr std::size_t _buffer_size() {
                return ((Size / 16) + (Size % 16 != 0)) * 2;
            }

            template<auto... Cs>
            struct tstring_ {
                using value_type                  = typename pack_value_type<Cs...>::type;
                constexpr static std::size_t size = sizeof...(Cs);
                constexpr static value_type  str[size] = { Cs... };

                constexpr static std::size_t buffer_size = _buffer_size<sizeof(str)>();
                constexpr static std::size_t buffer_align =
                    #ifndef JM_XORSTR_DISABLE_AVX_INTRINSICS
                        ((sizeof(str) > 16) ? 32 : 16);
                    #else
                        16;
                    #endif
            };

            template<std::uint32_t Seed>
            constexpr std::uint32_t key4() noexcept {
                std::uint32_t value = Seed;

                for (char c : __TIME__)
                    value = static_cast<std::uint32_t>((value ^ c) * 16777619ull);

                return value;
            }

            template<std::size_t S>
            constexpr std::uint64_t key8() {
                constexpr auto first_part  = key4 < 2166136261 + S > ();
                constexpr auto second_part = key4<first_part>();
                return (static_cast<std::uint64_t>(first_part) << 32) | second_part;
            }

            // clang and gcc try really hard to place the constants in data
            // sections. to counter that there was a need to create an intermediate
            // constexpr string and then copy it into a non constexpr container with
            // volatile storage so that the constants would be placed directly into
            // code.
            template<class T, std::uint64_t... Keys>
            struct string_storage {
                std::uint64_t storage[T::buffer_size];

                XORSTR_FORCEINLINE constexpr string_storage() noexcept : storage { Keys... }
                {
                    using cast_type =
                        typename unsigned_<sizeof(typename T::value_type)>::type;
                    constexpr auto value_size = sizeof(typename T::value_type);

                    // puts the string into 64 bit integer blocks in a constexpr
                    // fashion
                    for (std::size_t i = 0; i < T::size; ++i)
                        storage[i / (8 / value_size)] ^=
                            (std::uint64_t{ static_cast<cast_type>(T::str[i]) }
                             << ((i % (8 / value_size)) * 8 * value_size));
                }
            };
        }

        template<class T, std::uint64_t... Keys>
        class xor_string {
                alignas(T::buffer_align) XORSTR_VOLATILE std::uint64_t _storage[T::buffer_size];

                template<std::size_t N>
                XORSTR_FORCEINLINE void _crypt(XORSTR_VOLATILE std::uint64_t *keys) noexcept {
                    if constexpr (T::buffer_size > N) {
                        #ifndef JM_XORSTR_DISABLE_AVX_INTRINSICS
                            if constexpr ((T::buffer_size - N) >= 4) {
                                _mm256_store_si256(
                                    (__m256i *)(_storage + N),
                                    _mm256_xor_si256(
                                        _mm256_load_si256((const __m256i *)(_storage + N)),
                                        _mm256_load_si256((const __m256i *)(keys + N))));
                                _crypt < N + 4 > (keys);
                            } else
                        #endif
                        {
                            _mm_store_si128(
                                (__m128i *)(_storage + N),
                                _mm_xor_si128(_mm_load_si128((const __m128i *)(_storage + N)),
                                              _mm_load_si128((const __m128i *)(keys + N))));
                            _crypt<N + 2>(keys);
                        }
                    }
                }

                XORSTR_FORCEINLINE constexpr void _copy_single(
                        XORSTR_VOLATILE std::uint64_t& dst,
                        std::uint64_t val) noexcept
                {
                    dst = val;
                }

                // loop generates vectorized code which places constants in data dir
                template<std::size_t... Indices>
                XORSTR_FORCEINLINE constexpr void _copy(std::index_sequence<Indices...>) noexcept {
                    constexpr detail::string_storage<T, Keys...> storage;
                    (_copy_single(_storage[Indices], storage.storage[Indices]), ...);
                }

                template<std::size_t... Indices>
                XORSTR_FORCEINLINE constexpr void _copy_keys(
                    XORSTR_VOLATILE std::uint64_t *keys, std::index_sequence<Indices...>) noexcept {
                    (_copy_single(keys[Indices], Keys), ...);
                }

            public:
                using value_type    = typename T::value_type;
                using size_type     = std::size_t;
                using pointer       = value_type*;
                using const_pointer = const pointer;

                XORSTR_FORCEINLINE xor_string() noexcept {
                    _copy(std::make_index_sequence<T::buffer_size> {});
                }

                XORSTR_FORCEINLINE constexpr size_type size() const noexcept {
                    return T::size - 1;
                }

                XORSTR_FORCEINLINE void crypt() noexcept {
                    alignas(T::buffer_align) XORSTR_VOLATILE std::uint64_t keys[T::buffer_size];
                    _copy_keys(keys, std::make_index_sequence<sizeof...(Keys)>());
                    _crypt<0>(keys);
                }

                XORSTR_FORCEINLINE const_pointer get() const noexcept {
                    // C casts are used because buffer may or may not be volatile
                    return (const_pointer)(_storage);
                }

                XORSTR_FORCEINLINE const_pointer crypt_get() noexcept {
                    crypt();
                    // C casts are used because buffer may or may not be volatile
                    return (const_pointer)(_storage);
                }
        };

        template<class Tstr, std::size_t... StringIndices, std::size_t... KeyIndices>
        XORSTR_FORCEINLINE constexpr auto
        make_xorstr(Tstr,
                    std::index_sequence<StringIndices...>,
                    std::index_sequence<KeyIndices...>) noexcept {
            return xor_string<
                   detail::tstring_<Tstr().value[StringIndices]...>,
                   detail::key8<KeyIndices>()...> {};
        }
    }

    #undef XORSTR_FORCEINLINE
    #undef XORSTR_VOLATILE
    #undef JM_XORSTR_DISABLE_AVX_INTRINSICS
#elif XORSTR_USE == 2
    /**
     *  Reference: https://github.com/qis/xorstr
     *
     *  Usage:
     *      const std::string str = xorstr("xorstr_test_verify_error");
     */
    #include <array>
    #include <utility>

    #ifdef _MSC_VER
        #define XORSTR_INLINE __forceinline
    #else
        #define XORSTR_INLINE inline
    #endif

    namespace utils::xostring {
        constexpr auto time = __TIME__;
        constexpr auto seed = static_cast<int>(time[7]) +
                              static_cast<int>(time[6]) * 10 +
                              static_cast<int>(time[4]) * 60 +
                              static_cast<int>(time[3]) * 600 +
                              static_cast<int>(time[1]) * 3600 +
                              static_cast<int>(time[0]) * 36000;

        // 1988, Stephen Park and Keith Miller
        // "Random Number Generators: Good Ones Are Hard To Find", considered as "minimal standard"
        // Park-Miller 31 bit pseudo-random number generator, implemented with G. Carta's optimisation:
        // with 32-bit math and without divisions
        template <int N>
        struct random_generator {
            private:
                static constexpr unsigned a   = 16807;  // 7^5
                static constexpr unsigned m   = 2147483647;  // 2^31 - 1
                static constexpr unsigned s   = random_generator<N - 1>::value;
                static constexpr unsigned lo  = a * (s & 0xFFFF);  // multiply lower 16 bits by 16807
                static constexpr unsigned hi  = a * (s >> 16);  // multiply higher 16 bits by 16807
                static constexpr unsigned lo2 = lo + ((hi & 0x7FFF) << 16);  // combine lower 15 bits of hi with lo's upper bits
                static constexpr unsigned hi2 = hi >> 15;  // discard lower 15 bits of hi
                static constexpr unsigned lo3 = lo2 + hi;

            public:
                static constexpr unsigned max   = m;
                static constexpr unsigned value = lo3 > m ? lo3 - m : lo3;
        };

        template<>
        struct random_generator<0> {
            static constexpr unsigned value = seed;
        };

        template<int N, int M>
        struct random_int {
            static constexpr auto value = random_generator<N + 1>::value % M;
        };

        template<int N>
        struct random_char {
            static const char value = static_cast<char>(1 + random_int<N, 0x7F - 1>::value);
        };

        template<size_t N, int K>
        struct string {
            private:
                const char key_;
                std::array<char, N + 1> encrypted_;

                constexpr char enc(char c) const {
                    return c ^ key_;
                }

                char dec(char c) const {
                    return c ^ key_;
                }

            public:
                template<size_t... Is>
                constexpr XORSTR_INLINE string(const char *str, std::index_sequence<Is...>) :
                    key_(random_char<K>::value), encrypted_{ { enc(str[Is])... } } {}

                XORSTR_INLINE decltype(auto) decrypt() {
                    for (size_t i = 0; i < N; ++i) {
                        encrypted_[i] = dec(encrypted_[i]);
                    }

                    encrypted_[N] = '\0';
                    return encrypted_.data();
                }
        };

        /**
         *  Obfuscate strings at compile time
         */
        #define xorstr(s) (utils::xostring::string<sizeof(s) - 1, \
                            __COUNTER__>(s, std::make_index_sequence<sizeof(s) - 1>()).decrypt())

    }

    #undef XORSTR_INLINE
#else
    #define xorstr(s) s
#endif

#undef XORSTR_USE
#endif // UTILS_XORSTRING_HPP
