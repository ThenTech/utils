#ifndef CATCH_ABORT_HPP
#define CATCH_ABORT_HPP

#include "utils_compiler.hpp"
#include "utils_traits.hpp"

#if UTILS_HAS_INCLUDE("../utils_test/test_settings.hpp")
    #include "../utils_test/test_settings.hpp"
#endif

#include <iostream>
#include <functional>
#include <csetjmp>
#include <csignal>


#ifdef ENABLE_TESTS
    #define CATCH_CONFIG_CONSOLE_WIDTH 100
    #define CATCH_CONFIG_FAST_COMPILE
    #include "../utils_lib/external/catch.hpp"

    /**
     *  Defines for catching aborts when calling a function.
     */
    #define REQUIRE_FUNCTION_ABORTS(F, ...)         REQUIRE(utils::Catch::Function_Aborts(F, __VA_ARGS__))
    #define REQUIRE_FUNCTION_ABORTS_FALSE(F, ...)   REQUIRE_FALSE(utils::Catch::Function_Aborts(F, __VA_ARGS__))
    #define CHECK_FUNCTION_ABORTS(F, ...)           CHECK(utils::Catch::Function_Aborts(F, __VA_ARGS__))
    #define CHECK_FUNCTION_ABORTS_FALSE(F, ...)     CHECK_FALSE(utils::Catch::Function_Aborts(F, __VA_ARGS__))
#else
    #define REQUIRE_FUNCTION_ABORTS(F, ...)
    #define REQUIRE_FUNCTION_ABORTS_FALSE(F, ...)
    #define CHECK_FUNCTION_ABORTS(F, ...)
    #define CHECK_FUNCTION_ABORTS_FALSE(F, ...)
#endif

/**
 *  Whether to print noticication to std::cerr if signal was handled.
 */
#define UTILS_CATCH_REPORT_SIG_HANDLER 0


/**
 *  Macro to insert demangled function signature at compile time.
 */
#if defined(UTILS_COMPILER_MSVC)
    #define UTILS_CATCH_FUNCTION_NAME __FUNCDNAME__  // or __FUNCSIG__?
#else
    #define UTILS_CATCH_FUNCTION_NAME __PRETTY_FUNCTION__
#endif

/**
 *  Macro to insert current file, line and function at compile time.
 */
#define UTILS_TRACE_LOCATION __FILE__,__LINE__,UTILS_CATCH_FUNCTION_NAME

/**
 *  Macro to print an error trace.
 */
#define LOG_ERROR_TRACE(E) \
    do {                                                                     \
        std::cerr << "\033[31;1m" "[ERROR] Exception thrown:\n" "\033[33m  " \
                  << E.what()                                                \
                  << "\033[0m" "\n    at " "\033[36;1m"                      \
                  << __FILE__                                                \
                  << "\033[0m" ":" "\033[36;1m"                              \
                  << __LINE__                                                \
                  << "\033[0m" "\n    inside: " "\033[35;1m"                 \
                  << UTILS_CATCH_FUNCTION_NAME                               \
                  << "\033[0m" << std::flush << std::endl;                   \
    } while (false)

/**
 *  Macro to catch an exception, print an error trace
 *  and execute other code afterwards.
 */
#if defined(ENABLE_TESTS)
    #define CATCH_AND_LOG_ERROR_TRACE(...)                                     \
        catch (const std::exception& e) {                                      \
            __VA_ARGS__                                                        \
            throw e;                                                           \
        }
#else
    #define CATCH_AND_LOG_ERROR_TRACE(...)                                     \
        catch (const std::exception& e) {                                      \
            LOG_ERROR_TRACE(e);                                                \
            __VA_ARGS__                                                        \
        }
#endif

/**
 *  Assertion macro.
 */
#if defined(ENABLE_TESTS)
    #define ASSERT(condition) do { if (!(condition)) std::abort(); } while (false)
#elif !defined(NDEBUG)
    #define ASSERT(condition) \
        do {                                                                     \
            if (!(condition)) {                                                  \
                std::cerr << "\n\033[31;1m"                                      \
                          << "Assertion '" #condition "' failed"                 \
                             "\033[0m" " in" "\033[36;1m\n    "                  \
                          << __FILE__                                            \
                          << "\033[0m" ":" "\033[36;1m"                          \
                          << __LINE__ << "\033[0m" "\n    inside: " "\033[35;1m" \
                          << UTILS_CATCH_FUNCTION_NAME                           \
                          << "\033[0m" << std::endl;                             \
                std::abort();                                                    \
            }                                                                    \
        } while (false)
#else
    #define ASSERT(condition) do { } while (false)
#endif


namespace utils::Catch {
    namespace internal {
        /**
         *  \brief  The variable to save the environment to
         *          for jumping back after a function abort.
         */
        static jmp_buf _signal_handler_env;

        /**
         *  \brief  Custom signal handler to detect if a function aborted.
         *
         *  \param  signal
         *      The signal id to check.
         */
        ATTR_MAYBE_UNUSED
        static void _function_abort_signal_handler(int signal)  {
            if (signal == SIGABRT) {
                #if UTILS_CATCH_REPORT_SIG_HANDLER
                    std::cerr << "SIGABRT received\n";
                #endif
                std::longjmp(_signal_handler_env, 1);
            } else {
                #if UTILS_CATCH_REPORT_SIG_HANDLER
                    std::cerr << "Unexpected signal " << signal << " received\n";
                #endif
            }
        };
    }

    /**
     *  @brief  Function_Aborts
     *          Determine if given function calls abort when passing args.
     *          Use for catching ASSERT during tests.
     *
     *  @link   https://stackoverflow.com/a/34675789/6608855
     *  @link   https://wiki.sei.cmu.edu/confluence/display/c/SIG30-C.+Call+only+asynchronous-safe+functions+within+signal+handlers#SIG30-C.Callonlyasynchronous-safefunctionswithinsignalhandlers-CompliantSolution.1
     *
     *  @param  f
     *          The function to invoke.
     *  @param  args
     *          The args to pass on invocation.
     *  @return Returns whether the function called abort or not.
     */
    template<class F, class... Args> ATTR_NODISCARD
    bool Function_Aborts(F&& f, Args&& ... args) {
        static_assert(std::is_invocable_v<F, Args...>, "Function_Aborts: Callable function required.");

        auto previous_handler = std::signal(SIGABRT, utils::Catch::internal::_function_abort_signal_handler);

        if (HEDLEY_UNLIKELY(previous_handler == SIG_ERR)) {
            return true;
        }

        bool aborted = false;

        if (!setjmp(utils::Catch::internal::_signal_handler_env)) {
            std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
        } else {
            aborted = true;
        }

        std::signal(SIGABRT, previous_handler);
        return aborted;
    }
}

#undef UTILS_CATCH_REPORT_SIG_HANDLER
#endif // CATCH_ABORT_HPP
