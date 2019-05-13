#ifndef UTILS_TIME_HPP
#define UTILS_TIME_HPP

#include <chrono>
#include <iomanip>
#include <ctime>

namespace utils::time {
    /**
     *  Chrono::time_point alias.
     */
    using timepoint_t = std::chrono::time_point<std::chrono::steady_clock>;

    namespace Timer {
        /**
         *  \brief  Return a timepoint at the current time.
         */
        [[maybe_unused]]
        static inline timepoint_t Start(void) {
            return std::chrono::steady_clock::now();
        }

        /**
         *  \brief  Return the time in ns that elepsed from start.
         */
        [[maybe_unused]]
        static inline int64_t Duration_ns(const timepoint_t& start) {
            const timepoint_t end = std::chrono::steady_clock::now();
            return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        }

        /**
         *  \brief  Return the time in ms that elepsed from start.
         */
        [[maybe_unused]] static inline double Duration_ms(const timepoint_t& start) {
            return double(utils::time::Timer::Duration_ns(start)) / 1.0e6;
        }

        /**
         *  \brief  Return the time in s that elepsed from start.
         */
        [[maybe_unused]]
        static inline double Duration_s(const timepoint_t& start) {
            return double(utils::time::Timer::Duration_ns(start)) / 1.0e9;
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
    [[maybe_unused]]
    static inline auto Timestamp(const char* frmt="%Y-%m-%d %H:%M:%S", const std::time_t *epoch_time = nullptr) {
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
}

#endif // UTILS_TIME_HPP
