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
#define UTILS_CPP_VERSION_23 202002L

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
        #define _CRT_SECURE_NO_WARNINGS
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
    #error A C++17 compiler or higher is required!
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
 *  Align bit fields by packing the bytes.
 */
#define UTILS_PACK_BITS __attribute__((__packed__))

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

/**
 *  Macro to insert demangled function signature at compile time.
 *  Adapted from: https://stackoverflow.com/a/48229856/6608855
 */
#if defined(HEDLEY_GNUC_VERSION) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__) || HEDLEY_DMC_VERSION_CHECK(0, 2, 64)
    #define UTILS_FUNCTION_NAME __PRETTY_FUNCTION__
#elif defined(UTILS_COMPILER_MSVC) || defined(HEDLEY_MSVC_VERSION)
    #if defined(__FUNCDNAME__)
        // TODO Test if existence of __FUNCDNAME__ is testable MSVC, else default to this branch on MSVC.
        #define UTILS_FUNCTION_NAME __FUNCDNAME__
    #elif defined(__FUNCSIG__)
        #define UTILS_FUNCTION_NAME __FUNCSIG__
    #else
        #define UTILS_FUNCTION_NAME "(unknown)"
    #endif
#elif HEDLEY_INTEL_VERSION_CHECK(0, 0, 600)
    #define UTILS_FUNCTION_NAME __FUNCTION__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
    #define UTILS_FUNCTION_NAME __func__
#elif UTILS_CPP_LANG_CHECK(UTILS_CPP_VERSION_11)
    #define UTILS_FUNCTION_NAME __func__
#else
    #define UTILS_FUNCTION_NAME "(unknown)"
#endif

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
    #define UNUSED(...) (decltype(utils::compiler::unused_variable(__VA_ARGS__))())
#endif


////////////////////////////////////////////////////////////////////////////
/// Test above macros
////////////////////////////////////////////////////////////////////////////
//#pragma message(UTILS_VAR_NAME_VALUE(__GNUC__))
//#pragma message(UTILS_VAR_NAME_VALUE(__cplusplus))
//#pragma message(UTILS_VAR_NAME_VALUE(UTILS_BUILD_STAMP))

#endif // UTILS_COMPILER_HPP
