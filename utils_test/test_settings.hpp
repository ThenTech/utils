#ifndef TEST_SETTINGS_HPP
#define TEST_SETTINGS_HPP

//#define ENABLE_TESTS

#ifndef ENABLE_TESTS
    #define DOCTEST_CONFIG_DISABLE
#else
    #define DOCTEST_CONFIG_VOID_CAST_EXPRESSIONS
    #include "../utils_lib/utils_profiler.hpp"
#endif

#endif // TEST_SETTINGS_HPP
