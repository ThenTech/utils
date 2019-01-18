#ifndef UTILS_TIME_HPP
#define UTILS_TIME_HPP

#include <chrono>

namespace utils::time {
    /**
     *  Chrono::time_point alias.
     */
    using timepoint_t = std::chrono::time_point<std::chrono::steady_clock>;

    /**
     *  @brief  Return a timepoint at the current time.
     */
    [[maybe_unused]] static inline timepoint_t TimerStart(void) {
        return std::chrono::steady_clock::now();
    }

    /**
     *  @brief  Return the time in ns that elepsed from start.
     */
    [[maybe_unused]]
    static inline int64_t TimerDuration_ns(const timepoint_t& start) {
        const timepoint_t end = std::chrono::steady_clock::now();
        return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    }

    /**
     *  @brief  Return the time in ms that elepsed from start.
     */
    [[maybe_unused]] static inline double TimerDuration_ms(const timepoint_t& start) {
        return double(utils::time::TimerDuration_ns(start)) / 1.0e6;
    }

    /**
     *  @brief  Return the time in s that elepsed from start.
     */
    [[maybe_unused]] static inline double TimerDuration_s(const timepoint_t& start) {
        return double(utils::time::TimerDuration_ns(start)) / 1.0e9;
    }
}

#endif // UTILS_TIME_HPP
