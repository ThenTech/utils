#ifndef UTILS_PRINT_HPP
#define UTILS_PRINT_HPP
/**
 *  Generic printing to an outputstream.
 *  Reference: https://stackoverflow.com/questions/4850473/pretty-print-c-stl-containers
 */

#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>
#include <algorithm>

#ifdef _MSC_VER
    #include <intrin.h>
    #include <typeinfo>
#else
    #include <cxxabi.h>
#endif

#include "utils_string.hpp"

#define UTILS_PRINT_VECTOR_STRINGS_QUOTED              1
#define UTILS_PRINT_VECTOR_STRINGS_QUOTED_ON_NEW_LINES 1


namespace std {
    // Pre-declarations of container types so we don't actually have to include the relevant headers if not needed, speeding up compilation time.
    // These aren't necessary if you do actually include the headers.
    template<typename T, typename TAllocator> class vector;
    template<typename T, typename TAllocator> class list;
    template<typename T, typename TTraits, typename TAllocator> class set;
    template<typename TKey, typename TValue, typename TTraits, typename TAllocator> class map;
    template<typename TKey, typename TValue, typename THash, typename TPred, typename TAllocator> class unordered_map;
    template<typename T, std::size_t N> struct array;
}


namespace utils::printer {
    ////////////////////////////////////////////////////////////////////////////
    /// Mark std:: types as container
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

                (*_stream) << value;
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
                : public std::tr1::true_type
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
    template<typename T>
    struct delimiters<T, char> {
            static inline constexpr delimiters_values<char> values = { "[ ", ", ", " ]" };
    };

    template<typename T>
    struct delimiters<T, wchar_t> {
        static inline constexpr delimiters_values<wchar_t> values = { L"[ ", L", ", L" ]" };
    };

    ////////////////////////////////////////////////////////////////////////////
    /// vector<string> newline
    ////////////////////////////////////////////////////////////////////////////
    #if (UTILS_PRINT_VECTOR_STRINGS_QUOTED_ON_NEW_LINES || UTILS_PRINT_VECTOR_STRINGS_QUOTED)
        template<>
        struct delimiters<std::vector<std::string>, char> {
            static inline constexpr delimiters_values<char> values = {
                #if UTILS_PRINT_VECTOR_STRINGS_QUOTED_ON_NEW_LINES
                    "[\n    \"", "\",\n    \"", "\"\n]"
                #elif PRETTY_PRINT_VECTOR_STRINGS_QUOTED
                    "[\"", "\", \"", "\"]"
                #endif
            };
        };

        template<>
        struct delimiters<std::vector<std::string>, wchar_t> {
            static inline constexpr delimiters_values<wchar_t> values = {
                #if UTILS_PRINT_VECTOR_STRINGS_QUOTED_ON_NEW_LINES
                    L"[\n    \"", L"\",\n    \"", L"\"\n]"
                #elif PRETTY_PRINT_VECTOR_STRINGS_QUOTED
                    L"[\"", L"\", \"", L"\"]"
                #endif
            };
        };
    #endif

    ////////////////////////////////////////////////////////////////////////////
    // Delimiters for set
    template<typename T, typename TTraits, typename TAllocator>
    struct delimiters<std::set<T, TTraits, TAllocator>, char> {
            static inline constexpr delimiters_values<char> values = { "{ ", ", ", " }" };
    };

    template<typename T, typename TTraits, typename TAllocator>
    struct delimiters<std::set<T, TTraits, TAllocator>, wchar_t> {
            static inline constexpr delimiters_values<wchar_t> values = { L"{ ", L", ", L" }" };
    };

    ////////////////////////////////////////////////////////////////////////////
    // Delimiters for pair
    template<typename T1, typename T2>
    struct delimiters<std::pair<T1, T2>, char> {
            static inline constexpr delimiters_values<char> values = { "(", ", ", ")" };
    };

    template<typename T1, typename T2>
    struct delimiters<std::pair<T1, T2>, wchar_t> {
            static inline constexpr delimiters_values<wchar_t> values = { L"(", L", ", L")" };
    };

    ////////////////////////////////////////////////////////////////////////////
    // Delimiters for tuple
    template<typename ...Args>
    struct delimiters<std::tuple<Args...>, char> {
            static inline constexpr delimiters_values<char> values = { "(", ", ", ")" };
    };

    template<typename ...Args>
    struct delimiters<std::tuple<Args...>, wchar_t> {
            static inline constexpr delimiters_values<wchar_t> values = { L"(", L", ", L")" };
    };

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
                if constexpr(N == 1) {
                    stream << std::get<0>(_tuple);
                } else {
                    (print_tuple_helper<Tuple, N - 1, TChar, TCharTraits>(_tuple))(stream);

                    if (delimiters_type::values.delimiter != NULL)
                        stream << delimiters_type::values.delimiter;

                    stream << std::get<N - 1>(_tuple);
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

    // Mark array_wrapper as a container
    template<typename T, size_t N>
    struct is_container<utils::printer::array_wrapper<T, N>> : public std::true_type { };


    template <typename T, size_t N>
    utils::printer::array_wrapper<T, N> print_array_helper(const T (&a)[N]) {
        return utils::printer::array_wrapper<T, N>(a);
    }

    template <typename T, size_t N = 0>
    utils::printer::array_wrapper<T, N> print_array_helper(const T* a) {
        static_assert(N > 0, "print_array_helper requires N!");
        return utils::printer::array_wrapper<T, N>(a);
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
     *	\return
     *		Returns the class name of o.
     */
    template <
        class T, typename ... Strings,
        std::enable_if_t<(std::is_convertible_v<Strings const&, std::string_view> && ...), int> = 0
    >
    [[maybe_unused]]
    static const std::string type2name(T const& o, Strings&& ...filter) {
        #ifdef _CXXABI_H
            // Valgrind warning: abi::__cxa_demangle uses malloc so needs free
            utils::memory::unique_t<char, decltype (&std::free)>
                    demang(abi::__cxa_demangle(typeid(o).name(), nullptr, nullptr, nullptr),
                           &std::free);
            std::string s(demang.get());
        #else
            std::string s(typeid(o).name());
        #endif

        // Remove every occurence in filter... list from output (e.g. std::out)
        if constexpr (sizeof...(filter) > 0) {
            (utils::string::strReplaceAll(s, filter, ""), ...);
        }

        return s;
    }
}

namespace std {
    ////////////////////////////////////////////////////////////////////////////
    /// operator<<() overloads
    ////////////////////////////////////////////////////////////////////////////

    // Prints a print_container_helper to the specified stream.
    template<typename T, typename TChar, typename TCharTraits, typename TDelimiters>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::printer::print_container_helper<T, TChar, TCharTraits, TDelimiters>& helper)
    {
        helper(stream);
        return stream;
    }

    // Prints a container to the stream using default delimiters
    template<typename T, typename TChar, typename TCharTraits>
    typename std::enable_if<utils::printer::is_container<T>::value, std::basic_ostream<TChar, TCharTraits>&>::type
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const T& container)
    {
        stream << utils::printer::print_container_helper<T, TChar, TCharTraits>(container);
        return stream;
    }

    // Prints a pair to the stream using delimiters from delimiters<std::pair<T1, T2>>.
    template<typename T1, typename T2, typename TChar, typename TCharTraits>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const std::pair<T1, T2>& value)
    {
        if (utils::printer::delimiters<std::pair<T1, T2>, TChar>::values.prefix != NULL)
            stream << utils::printer::delimiters<std::pair<T1, T2>, TChar>::values.prefix;

        stream << value.first;

        if (utils::printer::delimiters<std::pair<T1, T2>, TChar>::values.delimiter != NULL)
            stream << utils::printer::delimiters<std::pair<T1, T2>, TChar>::values.delimiter;

        stream << value.second;

        if (utils::printer::delimiters<std::pair<T1, T2>, TChar>::values.postfix != NULL)
            stream << utils::printer::delimiters<std::pair<T1, T2>, TChar>::values.postfix;

        return stream;
    }

    template<typename ...Args, typename TChar, typename TCharTraits, typename TDelimiters>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::printer::print_tuple_helper<std::tuple<Args...>, sizeof...(Args), TChar, TDelimiters>& helper)
    {
        helper(stream);
        return stream;
    }

    // Prints a tuple to the stream using delimiters from delimiters<std::pair<T1, T2>>.
    template<typename ...Args, typename TChar, typename TCharTraits>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const std::tuple<Args...>& value)
    {
        if (utils::printer::delimiters<std::tuple<Args...>, TChar>::values.prefix != NULL)
            stream << utils::printer::delimiters<std::tuple<Args...>, TChar>::values.prefix;

        stream << utils::printer::print_tuple_helper<std::tuple<Args...>, sizeof...(Args), TChar, TCharTraits>(value);

        if (utils::printer::delimiters<std::tuple<Args...>, TChar>::values.postfix != NULL)
            stream << utils::printer::delimiters<std::tuple<Args...>, TChar>::values.postfix;

        return stream;
    }

    template<typename T, size_t N, typename TChar, typename TCharTraits, typename TDelimiters>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::printer::array_wrapper<T, N>& helper)
    {
        stream << utils::printer::print_container_helper<utils::printer::array_wrapper<T, N>, TChar, TCharTraits>(helper);
        return stream;
    }

    // WARNING: Hacky dereference iterator
    template<typename T, typename Container, typename TChar, typename TCharTraits>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const __gnu_cxx::__normal_iterator<T, Container>& it_value)
    {
        stream << *it_value;
        return stream;
    }
}

#endif // UTILS_PRINT_HPP
