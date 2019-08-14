#ifndef UTILS_COMPILER_HPP
#define UTILS_COMPILER_HPP

#include "external/hedley/hedley.h"

////////////////////////////////////////////////////////////////////////////
/// Macro helpers
////////////////////////////////////////////////////////////////////////////
#define UTILS_CPP_VERSION_11 201103L
#define UTILS_CPP_VERSION_14 201402L
#define UTILS_CPP_VERSION_17 201703L
#define UTILS_CPP_VERSION_20 201704L

/**
 *  Indicate platform with UTILS_OS_*
 */
#if defined(__unix__) || defined(__unix) || defined(__linux__) || defined(__CYGWIN__)
    #define UTILS_OS_LINUX 1
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN64)
    #define UTILS_OS_WIN 1

    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #if defined(_MSVC_LANG)
        #define UTILS_COMPILER_MSVC 1
    #endif
#elif defined(__APPLE__) || defined(__MACH__)
    #define UTILS_OS_MAC 1
#else
    #error Unknown Platform
#endif

/**
 *  Indicate C++ languange version with UTILS_CPP_LANG
 */
#if defined(UTILS_COMPILER_MSVC)
    #define UTILS_CPP_LANG (_MSVC_LANG)
#elif defined(__cplusplus) && (__cplusplus)
    #define UTILS_CPP_LANG (__cplusplus)
#else
    #define UTILS_CPP_LANG (0)
#endif

/**
 *  Check cpp languange version with any of UTILS_CPP_VERSION_*
 */
#define UTILS_CPP_LANG_CHECK(major) (UTILS_CPP_LANG >= major)

/**
 *  Abort when current C++ version < c++17
 */
#if !UTILS_CPP_LANG_CHECK(UTILS_CPP_VERSION_17)
    #error A C++17 compiler is required!
#endif

/**
 *  Indicate platform architecture with UTILS_OS_BITS
 */
#if _WIN64 || __x86_64__ || __ppc64__
    #define UTILS_OS_BITS (64)
#elif _WIN32 || __GNUC__
    #define UTILS_OS_BITS (32)
#else
    #define UTILS_OS_BITS (00)
#endif

/**
 *  Datatime stamp at compile time
 */
#define UTILS_BUILD_STAMP __DATE__ " " __TIME__

/**
 *  Stringify variable as "var = <var's value>"
 */
#define UTILS_VAR_NAME_VALUE(var)     #var " = " HEDLEY_STRINGIFY(var)


/**
 *  Add padding to class or struct members.
 *  TYPE should be the type of the last member variable, while
 *  SIZE should be its size (in bytes).
 *
 *  This will add a char[] with a size to pad the struct/class to the next byte boundary.
 */
#define UTILS_ADD_PADDING(TYPE, SIZE) char __utils_padding[alignof(TYPE) * SIZE - 1];


/**
 *  Return the arguments as-is with commas in between.
 *  Use for recursive MACRO expansion where a comma separated list is needed.
 *  e.g. a template args list
 */
#define UTILS_ARGS(...)   __VA_ARGS__


/**
 *  Check if S can be included as a file.
 */
#define UTILS_HAS_INCLUDE(S) __has_include(S)


////////////////////////////////////////////////////////////////////////////
/// Attributes
////////////////////////////////////////////////////////////////////////////
/**
 *  Indicates that the function does not return.
 */
#define ATTR_NORETURN       HEDLEY_NO_RETURN

/**
 *  ATTR_DEPRECATED indicates that the use of the name or entity declared
 *  with this attribute is allowed, but discouraged for some reason.
 */
#define ATTR_DEPRECATED(since)                  HEDLEY_DEPRECATED(since)
#define ATTR_DEPRECATED_FOR(since, replacement) HEDLEY_DEPRECATED_FOR(since, replacement)

/**
 *  ATTR_FALLTHROUGH indicates that the fall through from the previous
 *  case label is intentional and should not be diagnosed by a compiler that
 *  warns on fall-through.
 */
#define ATTR_FALLTHROUGH    HEDLEY_FALL_THROUGH;

/**
 *  ATTR_NODISCARD encourages the compiler to issue a warning
 *  if the return value is discarded.
 */
#define ATTR_NODISCARD      HEDLEY_WARN_UNUSED_RESULT

/**
 *  ATTR_MAYBE_UNUSED suppresses compiler warnings on unused entities, if any.
 */
#define ATTR_MAYBE_UNUSED   [[maybe_unused]]


#if !defined(HEDLEY_FLAGS)
    #define HEDLEY_FLAGS
#endif

////////////////////////////////////////////////////////////////////////////
/// Function with varying number of arguments to avoid "unused variable" warnings.
/// Use as UNUSED(var list) or add ATTR_MAYBE_UNUSED to args
////////////////////////////////////////////////////////////////////////////
namespace utils::compiler {
    template <typename... A>
    #if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L
        constexpr
    #else
        inline
    #endif
    void unused_variable(const A& ...) noexcept {}
}

#if defined(UTILS_COMPILER_MSVC)
    // Macro with varying number of arguments to avoid "unused variable" warnings.
    #define UNUSED(...) ((void)(__VA_ARGS__));
#else
    // Macro with varying number of arguments to avoid "unused variable" warnings.
    #define UNUSED(...) (decltype(utils::compiler::unused_variable(__VA_ARGS__))());
#endif


////////////////////////////////////////////////////////////////////////////
/// Test above macros
////////////////////////////////////////////////////////////////////////////
//#pragma message(UTILS_VAR_NAME_VALUE(__GNUC__))
//#pragma message(UTILS_VAR_NAME_VALUE(__cplusplus))
//#pragma message(UTILS_VAR_NAME_VALUE(UTILS_BUILD_STAMP))

#endif // UTILS_COMPILER_HPP
