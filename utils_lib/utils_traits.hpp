#ifndef UTILS_TRAITS_HPP
#define UTILS_TRAITS_HPP

#include <type_traits>
#include <iterator>
#include <variant>


#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var " = "  VALUE(var)

//#pragma message(VAR_NAME_VALUE(__GNUC__))
//#pragma message(VAR_NAME_VALUE(__cplusplus))
//#pragma message(VAR_NAME_VALUE(__TIME__))


/**
 *  Indicates that the function does not return.
 */
#define ATTR_NORETURN [[noreturn]]

/**
 *  ATTR_DEPRECATED indicates that the use of the name or entity declared
 *  with this attribute is allowed, but discouraged for some reason.
 */
#define ATTR_DEPRECATED [[deprecated]]

/**
 *  ATTR_FALLTHROUGH indicates that the fall through from the previous
 *  case label is intentional and should not be diagnosed by a compiler that
 *  warns on fall-through.
 */
#define ATTR_FALLTHROUGH [[fallthrough]];

/**
 * ATTR_NODISCARD encourages the compiler to issue a warning
 * if the return value is discarded.
 */
#define ATTR_NODISCARD [[nodiscard]]

/**
 * ATTR_MAYBE_UNUSED suppresses compiler warnings on unused entities, if any.
 */
#define ATTR_MAYBE_UNUSED [[maybe_unused]]


namespace std {
    // Pre-declarations of container types so we don't actually have to
    // include the relevant headers if not needed, speeding up compilation time.
    // These aren't necessary if you do actually include the headers.
    template<typename T, typename TAllocator> class vector;
    template<typename T, typename TAllocator> class list;
    template<typename T, typename TTraits, typename TAllocator> class set;
    template<typename TKey, typename TValue, typename TTraits, typename TAllocator> class map;
    template<typename TKey, typename TValue, typename THash, typename TPred, typename TAllocator> class unordered_map;
    template<typename T, std::size_t N> struct array;
}

namespace utils::traits {
    ////////////////////////////////////////////////////////////////////////////
    /// Function with varying number of arguments to avoid "unused variable" warnings.
    ////////////////////////////////////////////////////////////////////////////
    template <typename... A>
    #if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
        constexpr
    #else
        inline
    #endif
    void unused_variable(const A& ...) noexcept {}

    #if defined(_MSC_VER)
        // Macro with varying number of arguments to avoid "unused variable" warnings.
        #define UNUSED(...) ((void)(__VA_ARGS__));
    #else
        // Macro with varying number of arguments to avoid "unused variable" warnings.
        #define UNUSED(...) (decltype(utils::traits::unused_variable(__VA_ARGS__))());
    #endif

    ////////////////////////////////////////////////////////////////////////////
    /// True if type T is plain byte
    ////////////////////////////////////////////////////////////////////////////
    template<typename T>
    struct is_byte {
        static constexpr bool value =
               std::is_same<T,   signed char>::value
            || std::is_same<T, unsigned char>::value;
    };

    template<typename T>
    inline constexpr bool is_byte_v = is_byte<T>::value;

    ////////////////////////////////////////////////////////////////////////////
    /// True if type T is iterator
    ////////////////////////////////////////////////////////////////////////////
    template<typename T>
    struct is_iterator {
        private:
            static char test(...);

            template <typename U,
                      typename = typename std::iterator_traits<U>::difference_type,
                      typename = typename std::iterator_traits<U>::pointer,
                      typename = typename std::iterator_traits<U>::reference,
                      typename = typename std::iterator_traits<U>::value_type,
                      typename = typename std::iterator_traits<U>::iterator_category
                      > static long test(U&&);
        public:
            static constexpr bool value = std::is_same <
                                          decltype(test(std::declval<T>())), long >::value;
    };

    template<typename T>
    inline constexpr bool is_iterator_v = is_iterator<T>::value;

    // True if variable CONTAINER is a container with iterator support.
    #define is_iterable(CONTAINER)   utils::traits::is_iterator<decltype(std::begin(CONTAINER))>
    #define is_iterable_v(CONTAINER) utils::traits::is_iterator_v<decltype(std::begin(CONTAINER))>

    ////////////////////////////////////////////////////////////////////////////
    /// True if type T is a member of the variant VARIANT_T
    ////////////////////////////////////////////////////////////////////////////
    template<typename T, typename VARIANT_T>
    struct is_variant_member;

    template<typename T, typename... ALL_T>
    struct is_variant_member<T, std::variant<ALL_T...>>
        : public std::disjunction<std::is_same<T, ALL_T>...> {};

    template<typename T, typename... ALL_T>
    inline constexpr bool is_variant_member_v = is_variant_member<T, ALL_T...>::value;

    ////////////////////////////////////////////////////////////////////////////
    /// True if type T is a std:: container type
    ////////////////////////////////////////////////////////////////////////////
    // Basic is_container template; specialize to derive from std::true_type for all desired container types
    template<typename T>
    struct is_container : public std::false_type { };

    // Mark vector as a container
    template<typename T, typename TAllocator>
    struct is_container<std::vector<T, TAllocator>> : public std::true_type { };

    // Mark list as a container
    template<typename T, typename TAllocator>
    struct is_container<std::list<T, TAllocator>> : public std::true_type { };

    // Mark set as a container
    template<typename T, typename TTraits, typename TAllocator>
    struct is_container<std::set<T, TTraits, TAllocator>> : public std::true_type { };

    // Mark map as a container
    template<typename TKey, typename TValue, typename TTraits, typename TAllocator>
    struct is_container<std::map<TKey, TValue, TTraits, TAllocator>> : public std::true_type { };

    // Mark unordered_map as a container
    template<typename TKey, typename TValue, typename THash, typename TPred, typename TAllocator>
    struct is_container<std::unordered_map<TKey, TValue, THash, TPred, TAllocator>> : public std::true_type { };

    // Mark std::array as a container
    template<typename T, std::size_t N>
    struct is_container<std::array<T, N>> : public std::true_type { };

    ////////////////////////////////////////////////////////////////////////////
    /**
     *  \brief   Cast enum type to underlining data type.
     *  \param   e
     *      The enum value to cast.
     */
    template <typename E>
    inline constexpr auto to_underlying(const E e) noexcept {
        return static_cast<std::underlying_type_t<E>>(e);
    }
}



#endif // UTILS_TRAITS_HPP
