#ifndef UTILS_PRINT_HPP
#define UTILS_PRINT_HPP
/**
 *  Generic printing to an outputstream.
 *  Reference: https://stackoverflow.com/questions/4850473/pretty-print-c-stl-containers
 *
 *  TODO Also look at https://github.com/p-ranav/pprint
 */

#include "utils_traits.hpp"
#include "utils_catch.hpp"
#include "utils_string.hpp"

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


#define UTILS_PRINT_VECTOR_STRINGS_QUOTED              1
#define UTILS_PRINT_VECTOR_STRINGS_QUOTED_ON_NEW_LINES 1


namespace utils::print {
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

        template<>
        struct delimiters<std::vector<std::string_view>, char> {
            static inline constexpr delimiters_values<char> values = {
                #if UTILS_PRINT_VECTOR_STRINGS_QUOTED_ON_NEW_LINES
                    "[\n    \"", "\",\n    \"", "\"\n]"
                #elif PRETTY_PRINT_VECTOR_STRINGS_QUOTED
                    "[\"", "\", \"", "\"]"
                #endif
            };
        };

        template<>
        struct delimiters<std::vector<std::string_view>, wchar_t> {
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
    // Delimiters for optional
    template<typename T>
    struct delimiters<std::optional<T>, char> {
            static inline constexpr delimiters_values<char> values = { "", "nullopt", "" };
    };

    template<typename T>
    struct delimiters<std::optional<T>, wchar_t> {
            static inline constexpr delimiters_values<wchar_t> values = { L"", L"nullopt", L"" };
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
                if constexpr (N == 1) {
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
}

namespace utils::traits {
    // Mark array_wrapper as a container
    template<typename T, size_t N>
    struct is_container<utils::print::array_wrapper<T, N>> : public std::true_type { };
}

namespace utils::print {
    template <typename T, size_t N>
    utils::print::array_wrapper<T, N> print_array_helper(const T (&a)[N]) {
        return utils::print::array_wrapper<T, N>(a);
    }

    template <typename T, size_t N = 0>
    utils::print::array_wrapper<T, N> print_array_helper(const T* a) {
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
        std::enable_if_t<(std::is_convertible_v<Strings const&, std::string_view> && ...), int> = 0
    >
    ATTR_MAYBE_UNUSED
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
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::print::print_container_helper<T, TChar, TCharTraits, TDelimiters>& helper)
    {
        helper(stream);
        return stream;
    }

    template<typename T, size_t N, typename TChar, typename TCharTraits, typename TDelimiters>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::print::array_wrapper<T, N>& helper)
    {
        stream << utils::print::print_container_helper<utils::print::array_wrapper<T, N>, TChar, TCharTraits>(helper);
        return stream;
    }

    /// Prints a pair to the stream using delimiters from delimiters<std::pair<T1, T2>>.
    template<typename T1, typename T2, typename TChar, typename TCharTraits>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const std::pair<T1, T2>& value)
    {
        if (utils::print::delimiters<std::pair<T1, T2>, TChar>::values.prefix != NULL)
            stream << utils::print::delimiters<std::pair<T1, T2>, TChar>::values.prefix;

        stream << value.first;

        if (utils::print::delimiters<std::pair<T1, T2>, TChar>::values.delimiter != NULL)
            stream << utils::print::delimiters<std::pair<T1, T2>, TChar>::values.delimiter;

        stream << value.second;

        if (utils::print::delimiters<std::pair<T1, T2>, TChar>::values.postfix != NULL)
            stream << utils::print::delimiters<std::pair<T1, T2>, TChar>::values.postfix;

        return stream;
    }

    template<typename ...Args, typename TChar, typename TCharTraits, typename TDelimiters>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::print::print_tuple_helper<std::tuple<Args...>, sizeof...(Args), TChar, TDelimiters>& helper)
    {
        helper(stream);
        return stream;
    }

    /// Prints a tuple to the stream using delimiters from delimiters<std::tuple<Args...>>.
    template<typename ...Args, typename TChar, typename TCharTraits>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const std::tuple<Args...>& value)
    {
        if (utils::print::delimiters<std::tuple<Args...>, TChar>::values.prefix != NULL)
            stream << utils::print::delimiters<std::tuple<Args...>, TChar>::values.prefix;

        stream << utils::print::print_tuple_helper<std::tuple<Args...>, sizeof...(Args), TChar, TCharTraits>(value);

        if (utils::print::delimiters<std::tuple<Args...>, TChar>::values.postfix != NULL)
            stream << utils::print::delimiters<std::tuple<Args...>, TChar>::values.postfix;

        return stream;
    }

    /// Prints an optional to the stream using delimiters from delimiters<std::optional<T>>.
    template<typename T, typename TChar, typename TCharTraits>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const std::optional<T>& optional)
    {
        if (optional.has_value())
            stream << optional.value();
        else
            stream << utils::print::delimiters<std::optional<T>, TChar>::values.delimiter;
        return stream;
    }

    /**
     *  \brief  Generic operator <<
     *
     *          if T == is_pointer<T>
     *              => prints %p format
     *          if T == __gnu_cxx::__normal_iterator<T, Container>
     *                  or another iterator type
     *              => prints dereference
     *          if T == is_container<T>
     *              => prints print_container_helpter<T> using default delimiters
     *          else prints value itself.
     *
     *  \param  stream
     *      The stream to write to.
     *  \param  it_value
     *      The possible iterator value.
     *  \return Returns the stream reference.
     */
    template<typename T, typename TChar, typename TCharTraits>
    std::basic_ostream<TChar, TCharTraits>&
    operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const T& it_value)
    {
        if constexpr (std::is_pointer<T>::value) {
            if constexpr (std::is_void<T>::value) {
                stream << (it_value == nullptr ? "nullptr" : utils::string::format("%p", it_value));
            } else {
                stream << *it_value;
            }
        } else if constexpr (utils::traits::is_iterator_v<T>) {
            stream << *it_value;
        } else if constexpr (utils::traits::is_container<T>::value) {
            stream << utils::print::print_container_helper<T, TChar, TCharTraits>(it_value);
        } else {
            stream << it_value; // Warning: Should be unreachable
            ASSERT(false);
        }

        return stream;
    }
}

#undef UTILS_PRINT_VECTOR_STRINGS_QUOTED
#undef UTILS_PRINT_VECTOR_STRINGS_QUOTED_ON_NEW_LINES
#endif // UTILS_PRINT_HPP
