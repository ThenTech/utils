#ifndef CATCH_ABORT_HPP
#define CATCH_ABORT_HPP

#include <iostream>
#include <functional>
#include <csetjmp>
#include <csignal>

#define REPORT_SIG_HANDLER 0

#if defined(ENABLE_TESTS)
    #define ASSERT(condition) do { if (!(condition)) std::abort(); } while (false)
#elif !defined(NDEBUG)
    #define ASSERT(condition) \
        do { \
            if (!(condition)) { \
                std::cerr << "\033[31;1m" \
                          << "Assertion '" #condition "' failed" \
                             "\033[0m" " in " "\033[36;1m" \
                          << __FILE__ \
                          << "\033[0m" ":" "\033[36;1m" \
                          << __LINE__ << "\033[0m" ":\n    in " "\033[35m"\
                          << __PRETTY_FUNCTION__ \
                          << "\033[0m" << std::endl; \
                std::abort(); \
            } \
        } while (false)
#else
    #define ASSERT(condition) do { } while (false)
#endif

static jmp_buf env;

[[maybe_unused]]
static void signal_handler(int signal)  {
    if (signal == SIGABRT) {
        #if REPORT_SIG_HANDLER
            std::cerr << "SIGABRT received\n";
        #endif
        std::longjmp(env, 1);
    } else {
        #if REPORT_SIG_HANDLER
            std::cerr << "Unexpected signal " << signal << " received\n";
        #endif
    }
};

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
template<class F, class... Args>
bool Function_Aborts(F&& f, Args&& ... args) {
    static_assert(std::is_invocable_v<F, Args...>, "Function_Aborts: Callable function required.");

    auto previous_handler = std::signal(SIGABRT, signal_handler);

    if (previous_handler == SIG_ERR) {
        return true;
    }

    bool aborted = false;

    if (!setjmp(env)) {
        std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
    } else {
        aborted = true;
    }

    std::signal(SIGABRT, previous_handler);
    return aborted;
}

#endif // CATCH_ABORT_HPP
