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

#include "utils_compiler.hpp"
#include "utils_traits.hpp"
#include "utils_test.hpp"
#include "utils_string.hpp"
#include "utils_time.hpp"
#include "utils_os.hpp"
#include "utils_math.hpp"

#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>
#include <variant>
#include <algorithm>

#if defined(UTILS_COMPILER_MSVC)
    #include <intrin.h>
    #include <typeinfo>
#elif UTILS_HAS_INCLUDE(<cxxabi.h>)
    #include <cxxabi.h>
#endif

#define UTILS_PRINT_QUOTE_STRINGS                  1
#define UTILS_PRINT_STRINGS_CONTAINER_ON_NEW_LINES 1
#define UTILS_PRINT_EXPORT_CONTAINER_FACTORIES     1


namespace utils::print {
    template <typename T, typename TChar, typename TCharTraits> ATTR_MAYBE_UNUSED
    static inline auto& print_quoted_helper(std::basic_ostream<TChar, TCharTraits>& stream, const T& value) {
        #if UTILS_PRINT_QUOTE_STRINGS
            if constexpr (utils::traits::is_byte_v<T>) {
                stream << "\'" << value << "\'";
            } else
            if constexpr (std::is_convertible_v<T, std::string_view>) {
                stream << "\"";
                const std::string_view val{value};
                stream.write(val.data(), val.size());
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
                if (HEDLEY_LIKELY(_delim != NULL)) {
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
    #define UTILS_PRINT_DELIM_CHAR_CONT         "[ ", ", ", " ]"
    #define UTILS_PRINT_DELIM_WCHAR_CONT        L"[ ", L", ", L" ]"
    #define UTILS_PRINT_DELIM_CHAR_CONT_NL      "[\n    ", ",\n    ", "\n]"
    #define UTILS_PRINT_DELIM_WCHAR_CONT_NL     L"[\n    ", L",\n    ", L"\n]"

    #define UTILS_PRINT_DELIM_CHAR_SET          "{ ", ", ", " }"
    #define UTILS_PRINT_DELIM_WCHAR_SET         L"{ ", L", ", L" }"
    #define UTILS_PRINT_DELIM_CHAR_SET_NL       "{\n    ", ",\n    ", "\n}"
    #define UTILS_PRINT_DELIM_WCHAR_SET_NL      L"{\n    ", L",\n    ", L"\n}"

    #define UTILS_PRINT_DELIM_CHAR_PAIR         "<", ", ", ">"
    #define UTILS_PRINT_DELIM_WCHAR_PAIR        L"<", L", ", L">"

    #define UTILS_PRINT_DELIM_CHAR_TUPLE        "(", ", ", ")"
    #define UTILS_PRINT_DELIM_WCHAR_TUPLE       L"(", L", ", L")"

    #define UTILS_PRINT_DELIM_CHAR_OPTIONAL     NULL, "nullopt", NULL
    #define UTILS_PRINT_DELIM_WCHAR_OPTIONAL    NULL, L"nullopt", NULL

    #define UTILS_PRINT_DELIM_CHAR_VARIANT      NULL, NULL, NULL
    #define UTILS_PRINT_DELIM_WCHAR_VARIANT     NULL, NULL, NULL

    #define UTILS_PRINT_DELIM_CHAR_KEYVAL       NULL, ": ", NULL
    #define UTILS_PRINT_DELIM_WCHAR_KEYVAL      NULL, L": ", NULL

    #define UTILS_PRINT_MAKE_CONTAINER(DELIM, TEMPL, TYPE) \
        template<TEMPL> \
        struct delimiters<TYPE, char> { \
            static inline constexpr delimiters_values<char> values = { UTILS_PRINT_DELIM_CHAR_ ## DELIM }; \
        }; \
        template<TEMPL> \
        struct delimiters<TYPE, wchar_t> { \
            static inline constexpr delimiters_values<wchar_t> values = { UTILS_PRINT_DELIM_WCHAR_ ## DELIM }; \
        };

    UTILS_PRINT_MAKE_CONTAINER(CONT, typename T, T)

    ////////////////////////////////////////////////////////////////////////////

    // container<T> newline
    #define UTILS_PRINT_MAKE_CONTAINER_NL(DELIM, TEMPL, CONT, ...) \
        template<TEMPL> \
        struct delimiters<std::CONT<std::string __VA_ARGS__>, char> { \
            static inline constexpr delimiters_values<char> values = { UTILS_PRINT_DELIM_CHAR_ ## DELIM ## _NL }; \
        }; \
        template<TEMPL> \
        struct delimiters<std::CONT<std::wstring __VA_ARGS__>, wchar_t> { \
            static inline constexpr delimiters_values<wchar_t> values = { UTILS_PRINT_DELIM_WCHAR_ ## DELIM ## _NL }; \
        }; \
        template<TEMPL> \
        struct delimiters<std::CONT<std::string_view __VA_ARGS__>, char> { \
            static inline constexpr delimiters_values<char> values = { UTILS_PRINT_DELIM_CHAR_ ## DELIM ## _NL }; \
        }; \
        template<TEMPL> \
        struct delimiters<std::CONT<std::wstring_view __VA_ARGS__>, wchar_t> { \
            static inline constexpr delimiters_values<wchar_t> values = { UTILS_PRINT_DELIM_WCHAR_ ## DELIM ## _NL }; \
        };

#if UTILS_PRINT_STRINGS_CONTAINER_ON_NEW_LINES
    UTILS_PRINT_MAKE_CONTAINER_NL(CONT, typename TAllocator, vector,, TAllocator)
    UTILS_PRINT_MAKE_CONTAINER_NL(CONT, typename TAllocator, list,, TAllocator)
    UTILS_PRINT_MAKE_CONTAINER_NL(CONT, size_t N, array,, N)
    UTILS_PRINT_MAKE_CONTAINER_NL(CONT,, initializer_list)
#endif

    ////////////////////////////////////////////////////////////////////////////

    // Delimiters for set
    UTILS_PRINT_MAKE_CONTAINER(SET,
        UTILS_ARGS(typename T, typename TCompare, typename TAllocator),
        std::set<UTILS_ARGS(T, TCompare, TAllocator)>)
    UTILS_PRINT_MAKE_CONTAINER(SET,
        UTILS_ARGS(typename T, typename TCompare, typename TAllocator),
        std::multiset<UTILS_ARGS(T, TCompare, TAllocator)>)

    UTILS_PRINT_MAKE_CONTAINER(SET,
        UTILS_ARGS(typename T, typename THash, typename TPred, typename TAllocator),
        std::unordered_set<UTILS_ARGS(T, THash, TPred, TAllocator)>)
    UTILS_PRINT_MAKE_CONTAINER(SET,
        UTILS_ARGS(typename T, typename THash, typename TPred, typename TAllocator),
        std::unordered_multiset<UTILS_ARGS(T, THash, TPred, TAllocator)>)

#if UTILS_PRINT_STRINGS_CONTAINER_ON_NEW_LINES
    UTILS_PRINT_MAKE_CONTAINER_NL(SET,
        UTILS_ARGS(typename TCompare, typename TAllocator),
        set, UTILS_ARGS(, TCompare, TAllocator))
    UTILS_PRINT_MAKE_CONTAINER_NL(SET,
        UTILS_ARGS(typename TCompare, typename TAllocator),
        multiset, UTILS_ARGS(, TCompare, TAllocator))

    UTILS_PRINT_MAKE_CONTAINER_NL(SET,
        UTILS_ARGS(typename THash, typename TPred, typename TAllocator),
        unordered_set, UTILS_ARGS(, THash, TPred, TAllocator))
    UTILS_PRINT_MAKE_CONTAINER_NL(SET,
        UTILS_ARGS(typename THash, typename TPred, typename TAllocator),
        unordered_multiset, UTILS_ARGS(, THash, TPred, TAllocator))
#endif

    // Delimiters for map
    UTILS_PRINT_MAKE_CONTAINER(SET_NL,
        UTILS_ARGS(typename TKey, typename TVal, typename TCompare, typename TAllocator),
        std::map<UTILS_ARGS(TKey, TVal, TCompare, TAllocator)>)
    UTILS_PRINT_MAKE_CONTAINER(SET_NL,
        UTILS_ARGS(typename TKey, typename TVal, typename TCompare, typename TAllocator),
        std::unordered_map<UTILS_ARGS(TKey, TVal, TCompare, TAllocator)>)

    // Delimiters for pair
    UTILS_PRINT_MAKE_CONTAINER(PAIR,
        UTILS_ARGS(typename T1, typename T2),
        std::pair<UTILS_ARGS(T1, T2)>)

    // Delimiters for tuple
    UTILS_PRINT_MAKE_CONTAINER(TUPLE,
        typename ...Args,
        std::tuple<Args...>)

    // Delimiters for optional
    UTILS_PRINT_MAKE_CONTAINER(OPTIONAL,
        typename T,
        std::optional<T>)

    // Delimiters for variant
    UTILS_PRINT_MAKE_CONTAINER(VARIANT,
        typename ...T,
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
                if (HEDLEY_LIKELY(delimiters_type::values.prefix != NULL))
                    stream << delimiters_type::values.prefix;

                std::copy(_container->begin(),
                          _container->end(),
                          pretty_ostream_iterator<typename T::value_type, TChar, TCharTraits>(
                              stream, delimiters_type::values.delimiter)
                );

                if (HEDLEY_LIKELY(delimiters_type::values.postfix != NULL))
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

                    if (HEDLEY_LIKELY(delimiters_type::values.delimiter != NULL))
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
            char * const demang = abi::__cxa_demangle(typeid(o).name(), nullptr, nullptr, nullptr);
            std::string s(demang);
            std::free(demang);
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
        if (HEDLEY_UNLIKELY(data == nullptr)) {
            return;
        }

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

    /**
     *  \brief  A class representing a Progressbar.
     *          Use this to indicate progress being made in a process or loop.
     *
     *          Printing the progressbar is not threadsafe.
     *
     *      e.g.
     *
     *      utils::print::Progressbar bar(list.size());
     *      for (auto val : list) {
     *          process(val);
     *          utils::Logger::GetConsoleStream() << ++bar;
     *      }   utils::Logger::GetConsoleStream() << bar.done();
     *
     *      or
     *
     *      utils::print::with_progressbar(list,
     *                                     utils::Logger::GetConsoleStream(),
     *                                     process);
     */
    template<char FILL_CHAR = '#', char EMPTY_CHAR = '-'>
    class Progressbar {
        private:
            static constexpr size_t MAX_WIDTH = 80ull;

            size_t iteration;
            const size_t max_iteration;
            const std::string prefix;
            const bool with_bar;
            const bool with_percentage;
            const bool with_elapsed;
            const bool with_remaining;
            const bool with_per_second;
            const bool update_on_step_only;
            const utils::time::timepoint_t start;
            const size_t width;
            bool finished;

            double progress    = 0.0;
            size_t step        = 0ull;
            bool should_redraw = true;

            inline void update_step() {
                this->iteration     = std::min(this->iteration, this->max_iteration);
                this->progress      = double(this->iteration) / this->max_iteration;
                const size_t step   = progress * this->width;
                this->should_redraw = this->step != step;
                this->step = step;
            }

            template<typename TChar, typename TCharTraits>
            void print(std::basic_ostream<TChar, TCharTraits>& stream = std::cout) const {
                if (HEDLEY_UNLIKELY(this->finished)) {
                    return;
                }

                if (HEDLEY_LIKELY(this->update_on_step_only && !this->should_redraw)) {
                    return;
                }

                stream << utils::os::Console::CLLINE << '\r'
                       << this->prefix;

                if (HEDLEY_LIKELY(this->with_bar)) {
                    size_t filled = 0;
                    stream << "[";
                    while (++filled < this->step)   stream << FILL_CHAR;
                    while (filled++ < this->width)  stream << EMPTY_CHAR;
                    stream << "]";
                }

                if (HEDLEY_LIKELY(this->with_percentage)) {
                    stream << utils::string::format(" %5.1f%%", this->progress * 100.0f);
                }

                if (HEDLEY_LIKELY(this->with_elapsed || this->with_remaining || this->with_per_second)) {
                    const auto elapsed_ms          = utils::time::Timer::time_ms::duration(this->start);
                    const double ticks_per_second  = double(this->iteration) / elapsed_ms * 1e3;

                    if (HEDLEY_LIKELY(this->with_elapsed)) {
                        stream << utils::time::Timestamp(std::time_t(elapsed_ms / 1000.0), " %M:%S")
                               << utils::string::format(".%1d", (int(elapsed_ms) % 1000) / 100);
                    }

                    if (HEDLEY_UNLIKELY(this->with_remaining)) {
                        const double remaining_ticks   = this->max_iteration - this->iteration;
                        const double total_remaining_s = remaining_ticks / ticks_per_second;
                        stream << utils::string::format(" %3.1fs left", total_remaining_s);
                    }

                    if (HEDLEY_UNLIKELY(this->with_per_second)) {
                        stream << utils::string::format(" (%1.0f/s)", ticks_per_second);
                    }
                }

                if (HEDLEY_UNLIKELY(this->finished || (this->iteration >= this->max_iteration))) {
                    stream << "\r\n";
                }

                stream << std::flush;
            }

        public:
            Progressbar(const size_t max_iteration,
                        const std::string_view prefix,
                        const bool with_bar,
                        const bool with_percentage,
                        const bool with_elapsed,
                        const bool with_remaining,
                        const bool with_per_second,
                        const bool update_on_step_only)
                : iteration{0}
                , max_iteration{max_iteration}
                , prefix{prefix}
                , with_bar{with_bar}
                , with_percentage{with_percentage}
                , with_elapsed{with_elapsed}
                , with_remaining{with_remaining}
                , with_per_second{with_per_second}
                , update_on_step_only{update_on_step_only}
                , start{utils::time::Timer::Start()}
                , width{MAX_WIDTH - 2                   /* max width - border chars */
                        - (prefix.size())               /* - prefix */
                        - (with_percentage  ?  7 : 0)   /* - percentage */
                        - (with_elapsed     ?  8 : 0)   /* - elapsed */
                        - (with_remaining   ? 10 : 0)   /* - remaining */
                        - (with_per_second  ?  8 : 0)   /* - per_second */
                }
                , finished{false}
            {
                // Empty
            }

            Progressbar(const size_t max_iteration,
                        const bool with_bar,
                        const bool with_percentage,
                        const bool with_elapsed,
                        const bool with_remaining,
                        const bool with_per_second,
                        const bool update_on_step_only)
                : Progressbar(max_iteration, "", with_bar,
                              with_percentage, with_elapsed, with_remaining, with_per_second,
                              update_on_step_only)
            {
                // Empty
            }

            Progressbar(const size_t max_iteration)
                : Progressbar(max_iteration, "", true, true, true, false, false, true)
            {
                // Empty
            }

            inline Progressbar& operator++()
                { ++this->iteration; this->update_step(); return *this; }
            inline Progressbar  operator++(int)
                { Progressbar p = *this;  ++(*this); return p; }
            inline Progressbar& operator+=(size_t increase)
                { this->iteration += increase; this->update_step(); return *this; }

            inline Progressbar& operator--()
                { --this->iteration; this->update_step(); return *this; }
            inline Progressbar  operator--(int)
                { Progressbar p = *this; --(*this); return p;  }
            inline Progressbar& operator-=(size_t decrease)
                { this->iteration -= decrease; this->update_step(); return *this; }

            /**
             *  \brief  Indicate that the process is done and the bar
             *          should not update anymore.
             *  \return Return a reference to itself for inline printing.
             */
            inline const Progressbar& done() {
                if (this->iteration >= this->max_iteration) {
                    this->iteration = this->max_iteration;
                    this->finished = true;
                }
                return *this;
            }

            template<typename TChar, typename TCharTraits>
            friend inline auto& operator<<(
                    std::basic_ostream<TChar, TCharTraits>& stream,
                    const utils::print::Progressbar<FILL_CHAR, EMPTY_CHAR>& bar)
            {
                bar.print(stream);
                return stream;
            }
    };

    /**
     *  \brief  A decorator to process the given iterator range, call \p f
     *          on each element and print a progressbar to \p stream.
     *
     *  \param  start
     *      The start iterator to begin from.
     *  \param  end
     *      The end iterator to stop at.
     *  \param  stream
     *      The ostream to print the progressbar to.
     *  \param  f
     *      The function to call on each element.
     *      Must take exactly one argument of the type Iterator::value_type.
     */
    template <
        typename Iterator,
        typename T = typename std::iterator_traits<Iterator>::value_type,
        typename TChar, typename TCharTraits,
        typename F,
        typename = typename std::enable_if_t<utils::traits::is_iterator_v<Iterator>>
    > ATTR_MAYBE_UNUSED
    static void with_progressbar(Iterator start, Iterator end,
                                 std::basic_ostream<TChar, TCharTraits>& stream,
                                 F&& f)
    {
        static_assert(utils::traits::is_invocable_v<F, T>,
                      "utils::print::with_progressbar: Callable function required.");
        utils::print::Progressbar bar(std::distance(start, end));

        stream << bar;
        for (; start != end; start++) {
            std::invoke(std::forward<F>(f), *start);
            stream << ++bar;
        }

        stream << bar.done();
    }

    /**
     *  \brief  A decorator to process the given container, call \p f
     *          on each element and print a progressbar to \p stream.
     *
     *  \param cont
     *      The container to process.
     *  \param  stream
     *      The ostream to print the progressbar to.
     *  \param  f
     *      The function to call on each element.
     *      Must take exactly one argument of the type Container::value_type.
     */
    template <
        typename Container,
        typename TChar, typename TCharTraits,
        typename F
    > ATTR_MAYBE_UNUSED
    static inline auto with_progressbar(const Container& cont,
                                        std::basic_ostream<TChar, TCharTraits>& stream,
                                        F&& f)
    {
        static_assert (utils::traits::is_iterable_v<Container>,
                       "utils::print::with_progressbar: Container must have iterator support.");
        return utils::print::with_progressbar(std::begin(cont), std::end(cont),
                                              stream, std::forward<F>(f));
    }
}

namespace std {
    ////////////////////////////////////////////////////////////////////////////
    /// operator<<() overloads
    ////////////////////////////////////////////////////////////////////////////

    // Prints a print_container_helper to the specified stream.
    template<typename T, typename TChar, typename TCharTraits, typename TDelimiters>
    inline auto& operator<<(std::basic_ostream<TChar, TCharTraits>& stream,
                            const utils::print::print_container_helper<T, TChar, TCharTraits, TDelimiters>& helper)
    {
        helper(stream);
        return stream;
    }

    // NOTE Redundant?
    template<typename T, size_t N, typename TChar, typename TCharTraits, typename TDelimiters>
    inline auto& operator<<(std::basic_ostream<TChar, TCharTraits>& stream,
                            const utils::print::array_wrapper<T, N>& helper)
    {
        stream << utils::print::print_container_helper<utils::print::array_wrapper<T, N>, TChar, TCharTraits>(helper);
        return stream;
    }

    /// Prints a pair to the stream using delimiters from delimiters<std::pair<T1, T2>>.
    template<typename T1, typename T2, typename TChar, typename TCharTraits>
    auto& operator<<(std::basic_ostream<TChar, TCharTraits>& stream,
                     const std::pair<T1, T2>& value)
    {
        const auto delims = utils::print::delimiters<std::pair<T1, T2>, TChar>::values;
        if (HEDLEY_LIKELY(delims.prefix != NULL))
            stream << delims.prefix;

        utils::print::print_quoted_helper(stream, value.first);

        if (HEDLEY_LIKELY(delims.delimiter != NULL))
            stream << delims.delimiter;

        utils::print::print_quoted_helper(stream, value.second);

        if (HEDLEY_LIKELY(delims.postfix != NULL))
            stream << delims.postfix;

        return stream;
    }

    template<typename ...Args, typename TChar, typename TCharTraits, typename TDelimiters>
    inline auto& operator<<(std::basic_ostream<TChar, TCharTraits>& stream,
                            const utils::print::print_tuple_helper<std::tuple<Args...>,
                            sizeof...(Args), TChar, TDelimiters>& helper)
    {
        helper(stream);
        return stream;
    }

    /// Prints a tuple to the stream using delimiters from delimiters<std::tuple<Args...>>.
    template<typename ...Args, typename TChar, typename TCharTraits>
    auto& operator<<(std::basic_ostream<TChar, TCharTraits>& stream,
                     const std::tuple<Args...>& value)
    {
        const auto delims = utils::print::delimiters<std::tuple<Args...>, TChar>::values;
        if (HEDLEY_LIKELY(delims.prefix != NULL))
            stream << delims.prefix;

        stream << utils::print::print_tuple_helper<std::tuple<Args...>, sizeof...(Args), TChar, TCharTraits>(value);

        if (HEDLEY_LIKELY(delims.postfix != NULL))
            stream << delims.postfix;

        return stream;
    }

    /// Prints an optional to the stream using delimiters from delimiters<std::optional<T>>.
    template<typename T, typename TChar, typename TCharTraits>
    auto& operator<<(std::basic_ostream<TChar, TCharTraits>& stream,
                     const std::optional<T>& optional)
    {
        const auto delims = utils::print::delimiters<std::optional<T>, TChar>::values;
        if (HEDLEY_UNLIKELY(delims.prefix != NULL))
            stream << delims.prefix;

        if (HEDLEY_LIKELY(optional.has_value()))
            stream << optional.value();
        else if (HEDLEY_LIKELY(delims.delimiter != NULL))
            stream << delims.delimiter;

        if (HEDLEY_UNLIKELY(delims.postfix != NULL))
            stream << delims.postfix;

        return stream;
    }

    /// Prints a variant to the stream.
    template<typename Arg, typename ...Args, typename TChar, typename TCharTraits>
    auto& operator<<(std::basic_ostream<TChar, TCharTraits>& stream,
                     const std::variant<Arg, Args...>& var)
    {
        const auto delims = utils::print::delimiters<std::variant<Arg, Args...>, TChar>::values;
        if (HEDLEY_UNLIKELY(delims.prefix != NULL))
            stream << delims.prefix;

        std::visit([&](const auto& value) {
            stream << value;
        }, var);

        if (HEDLEY_UNLIKELY(delims.postfix != NULL))
            stream << delims.postfix;

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
    auto& operator<<(std::basic_ostream<TChar, TCharTraits>& stream,
                     const T& value)
    {
        if constexpr (std::is_pointer_v<T>) {
            if (value == nullptr) {
                stream << "nullptr";
            } else {
                if constexpr (std::is_void_v<std::remove_pointer_t<T>>) {
                    stream << utils::string::format("%p", value);
                } else {
                    stream << *value;
                }
            }
        } else if constexpr (utils::traits::is_iterator_v<T>) {
            stream << *value;
        } else if constexpr (std::is_array_v<T>) {
            stream << utils::print::array_wrapper(value);
        } else if constexpr (utils::traits::is_container<T>::value) {
            (utils::print::print_container_helper<T, TChar, TCharTraits>(value))(stream);
        } else {
            stream << "<"               << utils::print::type2name(value)
                   << " instance at 0x" << utils::string::format("%p", &value)
                   << ">";
            // ASSERT(false && "Warning: Unprintable value");
        }

        return stream;
    }
}

#if UTILS_PRINT_EXPORT_CONTAINER_FACTORIES == 0
    #undef UTILS_PRINT_MAKE_CONTAINER
    #undef UTILS_PRINT_MAKE_CONTAINER_NL
    #define UTILS_PRINT_MAKE_CONTAINER(...)
    #define UTILS_PRINT_MAKE_CONTAINER_NL(...)

    #undef UTILS_PRINT_DELIM_CHAR_CONT
    #undef UTILS_PRINT_DELIM_WCHAR_CONT
    #undef UTILS_PRINT_DELIM_CHAR_CONT_NL
    #undef UTILS_PRINT_DELIM_WCHAR_CONT_NL
    #undef UTILS_PRINT_DELIM_CHAR_SET
    #undef UTILS_PRINT_DELIM_WCHAR_SET
    #undef UTILS_PRINT_DELIM_CHAR_SET_NL
    #undef UTILS_PRINT_DELIM_WCHAR_SET_NL
    #undef UTILS_PRINT_DELIM_CHAR_PAIR
    #undef UTILS_PRINT_DELIM_WCHAR_PAIR
    #undef UTILS_PRINT_DELIM_CHAR_TUPLE
    #undef UTILS_PRINT_DELIM_WCHAR_TUPLE
    #undef UTILS_PRINT_DELIM_CHAR_OPTIONAL
    #undef UTILS_PRINT_DELIM_WCHAR_OPTIONAL
    #undef UTILS_PRINT_DELIM_CHAR_KEYVAL
    #undef UTILS_PRINT_DELIM_WCHAR_KEYVAL
#endif
#endif // UTILS_PRINT_HPP
