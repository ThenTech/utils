#ifndef UTILS_PRINT_HPP
#define UTILS_PRINT_HPP
/**
 *  Generic printing to an outputstream.
 *  Reference: https://stackoverflow.com/questions/4850473/pretty-print-c-stl-containers
 *  Also similar to: https://github.com/louisdx/cxx-prettyprint
 *
 *  Also look at https://github.com/p-ranav/pprint
 *
 *  TODO Add queue, prio_queue and stack
 */

#include "utils_traits.hpp"
#include "utils_catch.hpp"
#include "utils_string.hpp"

#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>
#include <variant>
#include <algorithm>

#if defined(UTILS_TRAITS_MSVC)
    #include <intrin.h>
    #include <typeinfo>
#else
    #include <cxxabi.h>
#endif

#define UTILS_PRINT_QUOTE_STRINGS                  1
#define UTILS_PRINT_STRINGS_CONTAINER_ON_NEW_LINES 1


namespace utils::print {
    template <typename T, typename TChar, typename TCharTraits> ATTR_MAYBE_UNUSED
    static inline auto& print_quoted_helper(std::basic_ostream<TChar, TCharTraits>& stream, const T& value) {
        #if UTILS_PRINT_QUOTE_STRINGS
            if constexpr (utils::traits::is_byte_v<T>) {
                stream << "\'" << value << "\'";
            } else
            if constexpr (std::is_convertible_v<T, std::string_view>) {
                stream << "\"";
                stream.write(value.data(), value.size());
                stream << "\"";
            } else
        #endif
        stream << value;
        return stream;
    }

    ////////////////////////////////////////////////////////////////////////////
    /// pretty_ostream_iterator
    ////////////////////////////////////////////////////////////////////////////
    // This works similar to ostream_iterator, but doesn't print a delimiter after the final item
    template<typename T, typename TChar = char, typename TCharTraits = std::char_traits<TChar> >
    class pretty_ostream_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
        public:
            typedef TChar char_type;
            typedef TCharTraits traits_type;
            typedef std::basic_ostream<TChar, TCharTraits> ostream_type;

        private:
            ostream_type *_stream;
            const char_type *_delim;
            bool _insertDelim;

        public:
            pretty_ostream_iterator(ostream_type& stream, const char_type *delim = NULL)
                : _stream(&stream), _delim(delim), _insertDelim(false) {
            }

            pretty_ostream_iterator<T, TChar, TCharTraits>& operator=(const T& value) {
                if (_delim != NULL) {
                    // Don't insert a delimiter if this is the first time the function is called
                    if (_insertDelim)
                        (*_stream) << _delim;
                    else
                        _insertDelim = true;
                }

                utils::print::print_quoted_helper(*_stream, value);
                return *this;
            }

            pretty_ostream_iterator<T, TChar, TCharTraits>& operator*() {
                return *this;
            }

            pretty_ostream_iterator<T, TChar, TCharTraits>& operator++() {
                return *this;
            }

            pretty_ostream_iterator<T, TChar, TCharTraits>& operator++(int) {
                return *this;
            }
    };

    #if defined(_MSC_VER) && _MSC_VER >= 1400
        // Declare pretty_ostream_iterator as checked
        template<typename T, typename TChar, typename TCharTraits>
        struct std::_Is_checked_helper<pretty_ostream_iterator<T, TChar, TCharTraits>>
                : public std::true_type
        {
            // Empty
        };
    #endif // _MSC_VER >= 1400

    ////////////////////////////////////////////////////////////////////////////
    /// Delimiters
    ////////////////////////////////////////////////////////////////////////////
    // Holds the delimiter values for a specific character type
    template<typename TChar>
    struct delimiters_values {
        typedef TChar char_type;
        const TChar *prefix;
        const TChar *delimiter;
        const TChar *postfix;
    };

    // Defines the delimiter values for a specific container and character type
    template<typename T, typename TChar>
    struct delimiters {
        static inline delimiters_values<TChar> values;
    };

    // Default delimiters
    #define COMMA ,
    #define DELIM_CHAR_CONT         "[ ", ", ", " ]"
    #define DELIM_WCHAR_CONT        L"[ ", L", ", L" ]"
    #define DELIM_CHAR_CONT_NL      "[\n    ", ",\n    ", "\n]"
    #define DELIM_WCHAR_CONT_NL     L"[\n    ", L",\n    ", L"\n]"

    #define DELIM_CHAR_SET          "{ ", ", ", " }"
    #define DELIM_WCHAR_SET         L"{ ", L", ", L" }"
    #define DELIM_CHAR_SET_NL       "{\n    ", ",\n    ", "\n}"
    #define DELIM_WCHAR_SET_NL      L"{\n    ", L",\n    ", L"\n}"

    #define DELIM_CHAR_PAIR         "<", ", ", ">"
    #define DELIM_WCHAR_PAIR        L"<", L", ", L">"

    #define DELIM_CHAR_TUPLE        "(", ", ", ")"
    #define DELIM_WCHAR_TUPLE       L"(", L", ", L")"

    #define DELIM_CHAR_OPTIONAL     NULL, "nullopt", NULL
    #define DELIM_WCHAR_OPTIONAL    NULL, L"nullopt", NULL

    #define DELIM_CHAR_VARIANT      NULL, NULL, NULL
    #define DELIM_WCHAR_VARIANT     NULL, NULL, NULL

    #define MAKE_CONTAINER(DELIM, TEMPL, TYPE) \
        template<TEMPL> \
        struct delimiters<TYPE, char> { \
            static inline constexpr delimiters_values<char> values = { DELIM_CHAR_ ## DELIM }; \
        }; \
        template<TEMPL> \
        struct delimiters<TYPE, wchar_t> { \
            static inline constexpr delimiters_values<wchar_t> values = { DELIM_WCHAR_ ## DELIM }; \
        };

    MAKE_CONTAINER(CONT, typename T, T)

    ////////////////////////////////////////////////////////////////////////////

    // container<string> newline
    #define MAKE_CONTAINER_NL(DELIM, TEMPL, CONT, ...) \
        template<TEMPL> \
        struct delimiters<std::CONT<std::string __VA_ARGS__>, char> { \
            static inline constexpr delimiters_values<char> values = { DELIM_CHAR_ ## DELIM ## _NL }; \
        }; \
        template<TEMPL> \
        struct delimiters<std::CONT<std::wstring __VA_ARGS__>, wchar_t> { \
            static inline constexpr delimiters_values<wchar_t> values = { DELIM_WCHAR_ ## DELIM ## _NL }; \
        }; \
        template<TEMPL> \
        struct delimiters<std::CONT<std::string_view __VA_ARGS__>, char> { \
            static inline constexpr delimiters_values<char> values = { DELIM_CHAR_ ## DELIM ## _NL }; \
        }; \
        template<TEMPL> \
        struct delimiters<std::CONT<std::wstring_view __VA_ARGS__>, wchar_t> { \
            static inline constexpr delimiters_values<wchar_t> values = { DELIM_WCHAR_ ## DELIM ## _NL }; \
        };

#if UTILS_PRINT_STRINGS_CONTAINER_ON_NEW_LINES
    MAKE_CONTAINER_NL(CONT, typename TAllocator, vector,, TAllocator)
    MAKE_CONTAINER_NL(CONT, typename TAllocator, list,, TAllocator)
    MAKE_CONTAINER_NL(CONT, size_t N, array,, N)
    MAKE_CONTAINER_NL(CONT,, initializer_list)
#endif

    ////////////////////////////////////////////////////////////////////////////

    // Delimiters for set
    MAKE_CONTAINER(SET, typename T COMMA typename TCompare COMMA typename TAllocator,
                   std::set<T COMMA TCompare COMMA TAllocator>)
    MAKE_CONTAINER(SET, typename T COMMA typename TCompare COMMA typename TAllocator,
                   std::multiset<T COMMA TCompare COMMA TAllocator>)

    MAKE_CONTAINER(SET, typename T COMMA typename THash COMMA typename TPred COMMA typename TAllocator,
                   std::unordered_set<T COMMA THash COMMA TPred COMMA TAllocator>)
    MAKE_CONTAINER(SET, typename T COMMA typename THash COMMA typename TPred COMMA typename TAllocator,
                   std::unordered_multiset<T COMMA THash COMMA TPred COMMA TAllocator>)

#if UTILS_PRINT_STRINGS_CONTAINER_ON_NEW_LINES
    MAKE_CONTAINER_NL(SET, typename TCompare COMMA typename TAllocator,
                      set,, TCompare COMMA TAllocator)
    MAKE_CONTAINER_NL(SET, typename TCompare COMMA typename TAllocator,
                      multiset,, TCompare COMMA TAllocator)

    MAKE_CONTAINER_NL(SET, typename THash COMMA typename TPred COMMA typename TAllocator,
                      unordered_set,, THash COMMA TPred COMMA TAllocator)
    MAKE_CONTAINER_NL(SET, typename THash COMMA typename TPred COMMA typename TAllocator,
                      unordered_multiset,, THash COMMA TPred COMMA TAllocator)
#endif

    // Delimiters for map
    MAKE_CONTAINER(SET_NL, typename TKey COMMA typename TVal COMMA typename TCompare COMMA typename TAllocator,
                   std::map<TKey COMMA TVal COMMA TCompare COMMA TAllocator>)
    MAKE_CONTAINER(SET_NL, typename TKey COMMA typename TVal COMMA typename TCompare COMMA typename TAllocator,
                   std::unordered_map<TKey COMMA TVal COMMA TCompare COMMA TAllocator>)

    // Delimiters for pair
    MAKE_CONTAINER(PAIR, typename T1 COMMA typename T2,
                   std::pair<T1 COMMA T2>)

    // Delimiters for tuple
    MAKE_CONTAINER(TUPLE, typename ...Args,
                   std::tuple<Args...>)

    // Delimiters for optional
    MAKE_CONTAINER(OPTIONAL, typename T,
                   std::optional<T>)

    // Delimiters for variant
    MAKE_CONTAINER(VARIANT, typename ...T,
                   std::variant<T...>)

    ////////////////////////////////////////////////////////////////////////////
    /// Functor to print containers. You can use this directly if you
    /// want to specificy a non-default delimiters type.
    ////////////////////////////////////////////////////////////////////////////
    template<typename T,
             typename TChar = char,
             typename TCharTraits = std::char_traits<TChar>,
             typename TDelimiters = delimiters<T, TChar> >
    struct print_container_helper {
        private:
            const T *_container;

        public:
            typedef TChar char_type;
            typedef TDelimiters delimiters_type;
            typedef std::basic_ostream<TChar, TCharTraits>& ostream_type;

            print_container_helper(const T& container)
                : _container(&container) {
            }

            void operator()(ostream_type& stream) const {
                if (delimiters_type::values.prefix != NULL)
                    stream << delimiters_type::values.prefix;

                std::copy(_container->begin(),
                          _container->end(),
                          pretty_ostream_iterator<typename T::value_type, TChar, TCharTraits>(
                              stream, delimiters_type::values.delimiter)
                );

                if (delimiters_type::values.postfix != NULL)
                    stream << delimiters_type::values.postfix;
            }
    };

    ////////////////////////////////////////////////////////////////////////////
    /// Functor to print tuples.
    ////////////////////////////////////////////////////////////////////////////
    template<typename Tuple,
             size_t N,
             typename TChar = char,
             typename TCharTraits = std::char_traits<TChar>,
             typename TDelimiters = delimiters<Tuple, TChar> >
    struct print_tuple_helper {
        private:
            const Tuple& _tuple;

        public:
            typedef TChar char_type;
            typedef TDelimiters delimiters_type;
            typedef std::basic_ostream<TChar, TCharTraits>& ostream_type;

            print_tuple_helper(const Tuple& tuple)
                : _tuple(tuple) {
            }

            void operator()(ostream_type& stream) const {
                if constexpr (N == 0) {
                    return;
                } else if constexpr (N == 1) {
                    utils::print::print_quoted_helper(stream, std::get<0>(_tuple));
                } else {
                    (print_tuple_helper<Tuple, N - 1, TChar, TCharTraits>(_tuple))(stream);

                    if (delimiters_type::values.delimiter != NULL)
                        stream << delimiters_type::values.delimiter;

                    utils::print::print_quoted_helper(stream, std::get<N - 1>(_tuple));
                }
            }
    };

    ////////////////////////////////////////////////////////////////////////////
    /// Functor to print raw arrays.
    ////////////////////////////////////////////////////////////////////////////
    template <typename T, size_t N>
    struct array_wrapper {
        private:
            const T* const _array;

        public:
            typedef const T * const_iterator;
            typedef T value_type;

            array_wrapper(const T (&a)[N]) : _array(a) { }
            array_wrapper(const T*& a)     : _array(a) { }
            inline const_iterator begin() const { return _array; }
            inline const_iterator end()   const { return _array + N; }
    };
}

namespace utils::traits {
    // Mark array_wrapper as a container
    template<typename T, size_t N>
    struct is_container<utils::print::array_wrapper<T, N>> : public std::true_type { };
}

namespace utils::print {
    template <typename T, size_t N> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline utils::print::array_wrapper<T, N> print_array_helper(const T (&a)[N]) {
        return utils::print::array_wrapper<T, N>(a);
    }

    template <typename T, size_t N = 0> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline utils::print::array_wrapper<T, N> print_array_helper(const T* a) {
        static_assert(N > 0, "print_array_helper requires N!");
        return utils::print::array_wrapper<T, N>(a);
    }

    /**	\brief	Returns the internal actual class name of the given object o.
     *
     *	**Uses __abi::__cxa_demangle__ which is part of <cxxabi.h> included in all GCC compilers.**
     *
     *	If GCC is not used, type2name will revert to typeid(o).name() instead.
     *
     *	\tparam	T
     *		The type of object to get the name demangled from.
     *	\param	o
     *		The object to demangle the name from.
     *  \param  filter
     *      A list of strings to filter (remove) from the output type name.
     *	\return
     *		Returns the class name of o.
     */
    template <
        class T, typename ...Strings,
        typename = typename std::enable_if_t<(std::is_convertible_v<Strings const&, std::string_view> && ...)>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static const std::string type2name(T const& o, const Strings& ...filter) {
        #ifdef _CXXABI_H
            // Valgrind warning: abi::__cxa_demangle uses malloc so needs free
            utils::memory::unique_t<char, decltype (&std::free)>
                    demang(abi::__cxa_demangle(typeid(o).name(), nullptr, nullptr, nullptr),
                           &std::free);
            std::string s(demang.get());
        #else
            std::string s(typeid(o).name());
        #endif

        // Remove every occurence in filter... list from output (e.g. std::)
        if constexpr (sizeof...(filter) > 0) {
            (utils::string::erase_all(s, filter), ...);
        }

        return s;
    }

    /**
     *  \brief  Dump the given data to ascii output and hex view.
     *
     *  \param  stream
     *      The stream to write to.
     *  \param  data
     *      The data to dump as raw `void*`, will be cast to `char*`.
     *  \param  length
     *      Length of \p data.
     *  \param  width
     *      Width for dump format, equal to amount of bytes on each line.
     */
    template<typename TChar, typename TCharTraits> ATTR_MAYBE_UNUSED
    static void hexdump(std::basic_ostream<TChar, TCharTraits>& stream,
                        const void* data, size_t length,
                        size_t width = 16)
    {
        if (data == nullptr)
            return;

        const char* const start = static_cast<const char*>(data);
        const char* const end   = start + length;
        const char* line = start;
        const auto locale = stream.getloc();

        while (line != end) {
            const size_t lineLength = std::min(width, static_cast<std::size_t>(end - line));

            stream << utils::string::format("%04X : ", line - start);

            for (const char* next = line; next != line + lineLength; ++next) {
                stream << (!std::isprint(*next, locale) ? '.' : *next);
            }

            stream << std::string(width - lineLength, ' ');

            for (const char* next = line; next != line + lineLength; ++next) {
                stream << utils::string::format(" %02X", (*next) & 0xFF);
            }

            stream << std::endl;
            line += lineLength;
        }
    }
}

namespace std {
    ////////////////////////////////////////////////////////////////////////////
    /// operator<<() overloads
    ////////////////////////////////////////////////////////////////////////////

    // Prints a print_container_helper to the specified stream.
    template<typename T, typename TChar, typename TCharTraits, typename TDelimiters>
    inline auto& operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::print::print_container_helper<T, TChar, TCharTraits, TDelimiters>& helper)
    {
        helper(stream);
        return stream;
    }

    // NOTE Redundant?
    template<typename T, size_t N, typename TChar, typename TCharTraits, typename TDelimiters>
    inline auto& operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::print::array_wrapper<T, N>& helper)
    {
        stream << utils::print::print_container_helper<utils::print::array_wrapper<T, N>, TChar, TCharTraits>(helper);
        return stream;
    }

    /// Prints a pair to the stream using delimiters from delimiters<std::pair<T1, T2>>.
    template<typename T1, typename T2, typename TChar, typename TCharTraits>
    auto& operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const std::pair<T1, T2>& value)
    {
        const auto delims = utils::print::delimiters<std::pair<T1, T2>, TChar>::values;
        if (delims.prefix != NULL) stream << delims.prefix;

        utils::print::print_quoted_helper(stream, value.first);

        if (delims.delimiter != NULL)  stream << delims.delimiter;

        utils::print::print_quoted_helper(stream, value.second);

        if (delims.postfix != NULL) stream << delims.postfix;
        return stream;
    }

    template<typename ...Args, typename TChar, typename TCharTraits, typename TDelimiters>
    auto& operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::print::print_tuple_helper<std::tuple<Args...>, sizeof...(Args), TChar, TDelimiters>& helper)
    {
        helper(stream);
        return stream;
    }

    /// Prints a tuple to the stream using delimiters from delimiters<std::tuple<Args...>>.
    template<typename ...Args, typename TChar, typename TCharTraits>
    auto& operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const std::tuple<Args...>& value)
    {
        const auto delims = utils::print::delimiters<std::tuple<Args...>, TChar>::values;
        if (delims.prefix != NULL) stream << delims.prefix;

        stream << utils::print::print_tuple_helper<std::tuple<Args...>, sizeof...(Args), TChar, TCharTraits>(value);

        if (delims.postfix != NULL) stream << delims.postfix;
        return stream;
    }

    /// Prints an optional to the stream using delimiters from delimiters<std::optional<T>>.
    template<typename T, typename TChar, typename TCharTraits>
    auto& operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const std::optional<T>& optional)
    {
        const auto delims = utils::print::delimiters<std::optional<T>, TChar>::values;
        if (delims.prefix != NULL)          stream << delims.prefix;

        if (optional.has_value())           stream << optional.value();
        else if (delims.delimiter != NULL)  stream << delims.delimiter;

        if (delims.postfix != NULL)         stream << delims.postfix;
        return stream;
    }

    /// Prints a variant to the stream.
    template<typename Arg, typename ...Args, typename TChar, typename TCharTraits>
    auto& operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const std::variant<Arg, Args...>& var)
    {
        const auto delims = utils::print::delimiters<std::variant<Arg, Args...>, TChar>::values;
        if (delims.prefix != NULL)          stream << delims.prefix;

        std::visit([&](const auto& value) { stream << value; }, var);

        if (delims.postfix != NULL)         stream << delims.postfix;
        return stream;
    }

    /**
     *  \brief  Generic operator <<
     *
     *          if T == is_pointer<T>
     *              => prints %p format if void*, "nullptr" if nullptr or else dereference
     *          if T == __gnu_cxx::__normal_iterator<T, Container>
     *                  or another iterator type
     *              => prints dereference
     *          if T == is_array<T>
     *              => prints with array_wrapper
     *          if T == is_container<T>
     *              => prints print_container_helpter<T> using default delimiters
     *          else prints value itself.
     *              => prints <Object type2name(value)>
     *                 or ASSERT(false)
     *
     *  \param  stream
     *      The stream to write to.
     *  \param  value
     *      The possible pointer/iterator/array/container value.
     *  \return Returns the stream reference.
     */
    template<typename T, typename TChar, typename TCharTraits>
    auto& operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const T& value)
    {
        if constexpr (std::is_pointer_v<T>) {
            if constexpr (std::is_void_v<std::remove_pointer_t<T>>) {
                stream << (value == nullptr ? "nullptr" : utils::string::format("%p", value));
            } else {
                stream << *value;
            }
        } else if constexpr (utils::traits::is_iterator_v<T>) {
            stream << *value;
        } else if constexpr (std::is_array_v<T>) {
            stream << utils::print::array_wrapper(value);
        } else if constexpr (utils::traits::is_container<T>::value) {
            (utils::print::print_container_helper<T, TChar, TCharTraits>(value))(stream);
        } else {
            stream << "<Object " << utils::print::type2name(value) << ">";
            // ASSERT(false && "Warning: Unprintable value");
        }

        return stream;
    }
}

#undef COMMA
#undef MAKE_CONTAINER
#undef MAKE_CONTAINER_NL
#undef DELIM_CHAR_CONT
#undef DELIM_WCHAR_CONT
#undef DELIM_CHAR_CONT_NL
#undef DELIM_WCHAR_CONT_NL
#undef DELIM_CHAR_SET
#undef DELIM_WCHAR_SET
#undef DELIM_CHAR_SET_NL
#undef DELIM_WCHAR_SET_NL
#undef DELIM_CHAR_PAIR
#undef DELIM_WCHAR_PAIR
#undef DELIM_CHAR_TUPLE
#undef DELIM_WCHAR_TUPLE
#undef DELIM_CHAR_OPTIONAL
#undef DELIM_WCHAR_OPTIONAL
#endif // UTILS_PRINT_HPP
