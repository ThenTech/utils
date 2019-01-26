#ifndef CATCH_ABORT_HPP
#define CATCH_ABORT_HPP

#include <iostream>
#include <functional>
#include <csetjmp>
#include <csignal>

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
                          << "\033[0m" " line " "\033[36;1m" \
                          << __LINE__ << "\033[0m" ":\n    " "\033[35m"\
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
        //std::cerr << "SIGABRT received\n";
        std::longjmp(env, 1);
    } else {
        //std::cerr << "Unexpected signal " << signal << " received\n";
    }
};

/**
 *  @brief  Function_Aborts
 *          Determine if given function calls abort when passing args.
 *          Use for catching ASSERT during tests.
 *  @param  f
 *          The function to invoke.
 *  @param  args
 *          The args to pass on invokation.
 *  @return Returns whether the function called abort or not.
 */
template<class F, class... Args>
bool Function_Aborts(F&& f, Args&& ... args) {
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
