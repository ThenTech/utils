#ifndef UTILS_CSV_HPP
#define UTILS_CSV_HPP
/*
 *  Other possibilities:
 *      https://github.com/vincentlaucsb/csv-parser
 *
 *  External includes
 *  WARNING external/csv is a cmake project and as such would normally be installed
 *          on the system. Since this is not the case as a git submodule,
 *          the inclusions of its own files must be corrected to use quotes
 *          instead of angle-brackets.
 *
 *      e.g. #include <csv/dialect.hpp>   -->   "dialect.hpp"
 */

#include "utils_compiler.hpp"

// Ignore warnings
HEDLEY_DIAGNOSTIC_PUSH
HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#if HEDLEY_MSVC_VERSION_CHECK(15,0,0)
    // TODO Ignore MSVC warnings
    //#pragma warning(disable:xxxx)
#else
    #pragma GCC diagnostic ignored "-Wpragmas"
    #pragma GCC diagnostic ignored "-Wreorder"
    #pragma GCC diagnostic ignored "-Wpessimizing-move"
    #pragma GCC diagnostic ignored "-Warray-bounds"
    #pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

#include "external/csv/include/csv/reader.hpp"
#include "external/csv/include/csv/writer.hpp"
#include "external/csv/include/csv/robin_hood.hpp"

HEDLEY_DIAGNOSTIC_POP

namespace utils {
    /**
     *  Wrap csv as ::csv
     *  Reference: http://github.com/p-ranav/csv (04/08/2019)
     */
    namespace csv = ::csv;
}

#include "utils_traits.hpp"
namespace utils::traits {
    // Mark csv::detail::unordered_map as a container
    template<bool b, size_t n, typename TKey, typename TValue, typename THash, typename KEqual>
    struct is_container<utils::csv::detail::Table<b, n, TKey, TValue, THash, KEqual>> : public std::true_type { };
}

#include "utils_print.hpp"
namespace utils::print {
    // Delimiters for csv map {col: val} pairs
    UTILS_PRINT_MAKE_CONTAINER(KEYVAL,
        UTILS_ARGS(typename T1, typename T2),
        utils::csv::pair<UTILS_ARGS(T1, T2)>)

    // Delimiters for csv map
    UTILS_PRINT_MAKE_CONTAINER(SET_NL,
        UTILS_ARGS(bool b, size_t n, typename TKey, typename TVal, typename THash, typename KEqual),
        utils::csv::detail::Table<UTILS_ARGS(b, n, TKey, TVal, THash, KEqual)>)
}

namespace std {
    /// Prints a utils::csv::pair to the stream using delimiters from delimiters<utils::csv::pair<T1, T2>>.
    template<typename T1, typename T2, typename TChar, typename TCharTraits>
    auto& operator<<(
            std::basic_ostream<TChar, TCharTraits>& stream,
            const utils::csv::pair<T1, T2>& value)
    {
        const auto delims = utils::print::delimiters<utils::csv::pair<T1, T2>, TChar>::values;
        if (HEDLEY_UNLIKELY(delims.prefix != NULL))
            stream << delims.prefix;

        utils::print::print_quoted_helper(stream, value.first);

        if (HEDLEY_LIKELY(delims.delimiter != NULL))
            stream << delims.delimiter;

        utils::print::print_quoted_helper(stream, value.second);

        if (HEDLEY_UNLIKELY(delims.postfix != NULL))
            stream << delims.postfix;

        return stream;
    }
}

#endif // UTILS_CSV_HPP
