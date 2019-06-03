#ifndef UTILS_TIME_HPP
#define UTILS_TIME_HPP

#include "utils_traits.hpp"

#include <chrono>
#include <iomanip>
#include <ctime>
#include <functional>

namespace utils::time {
    /**
     *  Chrono time ranges aliases
     */
    using nanoseconds  = std::chrono::duration<double, std::nano>;
    using milliseconds = std::chrono::duration<double, std::milli>;
    using seconds      = std::chrono::duration<double>;
    using minutes      = std::chrono::duration<double, std::ratio<60>>;
    using hours        = std::chrono::duration<double, std::ratio<3600>>;
    using days         = std::chrono::duration<double, std::ratio<86400>>;
    using weeks        = std::chrono::duration<double, std::ratio<604800>>;
    using months       = std::chrono::duration<double, std::ratio<2629746>>;
    using years        = std::chrono::duration<double, std::ratio<31556952>>;


    /**
     *  Chrono::time_point alias.
     */
    using timepoint_t = std::chrono::time_point<std::chrono::steady_clock>;

    /**
     *  \brief  Default format for timestamp.
     */
    constexpr std::string_view TIMESTAMP_FORMAT = "%Y-%m-%d %H:%M:%S";

    namespace Timer {
        /**
         *  \brief  Return a timepoint at the current time.
         */
        ATTR_MAYBE_UNUSED
        static inline timepoint_t Start(void) {
            return std::chrono::steady_clock::now();
        }

        /**
         *  \brief  Return the time in ns that elepsed from start.
         */
        struct time_ns {
            static inline int64_t duration(const timepoint_t& start) {
                const timepoint_t end = std::chrono::steady_clock::now();
                return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
            }
        };

        /**
         *  \brief  Return the time in ms that elepsed from start.
         */
        struct time_ms {
            static inline double duration(const timepoint_t& start) {
                return double(utils::time::Timer::time_ns::duration(start)) / 1.0e6;
            }
        };

        /**
         *  \brief  Return the time in s that elepsed from start.
         */
        struct time_s {
            static inline double duration(const timepoint_t& start) {
                return double(utils::time::Timer::time_ns::duration(start)) / 1.0e9;
            }
        };

        /**
         *  \brief  Time the execution of \p f, with \p duration_struct resolution.
         *
         *  \tparam duration_struct
         *      A struct with a `::duration(timepoint_t)` method specifying the
         *      resolution of the measurement.
         *  \param  f
         *      The function to exucute and time.
         *  \param  args
         *      The arguments to pass to \p f.
         *  \return Returns the `duration_struct::duration()` from the start and
         *          end of the execution of \p f.
         */
        template<
            class duration_struct = utils::time::Timer::time_ns,
            class F,
            class... Args
        > ATTR_MAYBE_UNUSED
        static inline double time(F&& f, Args&& ... args) {
            static_assert(std::is_invocable_v<F, Args...>, "utils::time::Timer::time: Callable function required.");

            const auto start = utils::time::Timer::Start();
            std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
            return duration_struct::duration(start);
        }

        /**
         *  \brief  Time the execution of \p f, with \p duration_struct resolution,
         *          \p N times.
         *
         *  \tparam N
         *      The amount of times to execute the function and average the results.
         *  \tparam duration_struct
         *      A struct with a `::duration(timepoint_t)` method specifying the
         *      resolution of the measurement.
         *  \param  f
         *      The function to exucute and time.
         *  \param  args
         *      The arguments to pass to \p f.
         *  \return Returns the average `duration_struct::duration()` from the
         *          sum of start and ends of each execution of \p f.
         */
        template<
            size_t N = 10,
            class duration_struct = utils::time::Timer::time_ns,
            class F,
            class... Args
        > ATTR_MAYBE_UNUSED
        static inline double time_n(F&& f, Args&& ... args) {
            static_assert(std::is_invocable_v<F, Args...>, "utils::time::Timer::time: Callable function required.");
            static_assert(N > 0, "utils::time::Timer::time: Time at least 1 run.");
            double total_time = 0.0;

            for (size_t i = 0; i < N; i++) {
                total_time += utils::time::Timer::time<duration_struct>(
                                  std::forward<F>(f),
                                  std::forward<Args>(args)...);
            }

            return total_time / double(N);
        }
    }

    /**
     *  \brief  Return a formatted timestamp with the given time since epoch,
     *          or the current time if nullptr.
     *
     *  \param  frmt
     *      The format string to format the time in.
     *
     *      %Y  Year
     *      %m  Month (padded)
     *      %d  Day (padded)
     *      %H  Hour (24)
     *      %M  Minutes (padded)
     *      %S  Seconds (padded)
     *
     *      Reference: https://en.cppreference.com/w/cpp/io/manip/put_time
     *
     *  \param  epoch_time
     *      Time since epoch to format.
     *      Will be set to the current time if nullptr.
     *  \return Returns a string type with the formatted time.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline auto Timestamp(const char* frmt = utils::time::TIMESTAMP_FORMAT.data(),
                                 const std::time_t *epoch_time = nullptr)
    {
        const std::time_t stamp = (epoch_time == nullptr
                                  ? std::time(nullptr)
                                  : *epoch_time);

        #ifdef _MSC_VER
            tm tm_l;
            localtime_s(&tm_l, &stamp);
            tm *tm = &tm_l;
        #else
            auto tm = std::localtime(&stamp);
        #endif

        return std::put_time(tm, frmt);
    }

    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline auto Timestamp(const std::time_t stamp,
                                 const char* frmt = utils::time::TIMESTAMP_FORMAT.data())
    {
        return utils::time::Timestamp(frmt, &stamp);
    }
}

#endif // UTILS_TIME_HPP
