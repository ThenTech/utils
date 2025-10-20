#ifndef UTILS_PROFILER_HPP
#define UTILS_PROFILER_HPP

#ifndef UTILS_PROFILER_ENABLE
    #define UTILS_PROFILER_ENABLE 1
#endif

#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <string_view>

#include "utils_compiler.hpp"
#include "utils_time.hpp"
#include "utils_threading.hpp"

namespace utils {

    /**
     *  \brief  The Profiler class to provide Chromium performance trace json output.
     *          Use chrome://tracing/ to load these files and see a timeline with the
     *          starts and durations of scopes.
     *
     *          With UTILS_PROFILE_BEGIN_SESSION(<file name>), a new trace file will be made.
     *          End a session with UTILS_PROFILE_END_SESSION() or by exiting the scope.
     *          Best begin a session from main().
     *
     *          Add UTILS_PROFILE_FUNCTION() at the start of a function, or
     *          UTILS_PROFILE_SCOPE(<name>) within a nested scope to trace its walltime.
     *
     *          Returning from the (function) scope will cause the scoped ProfileTimer
     *          to write the walltime to the trace file.
     */
    class Profiler {
        private:
            struct ProfileTimer {
                const std::string_view name;
                utils::time::timepoint_t start;

                ProfileTimer(std::string_view name)
                    : name{name}
                    , start{utils::time::Timer::Start()}
                {
                    // Empty
                }

                ~ProfileTimer() {
                    const auto elapsed_us = utils::time::Timer::time_ns::duration(this->start) / 1000;
                    utils::Profiler::get().AppendResults(this->name,
                                                         utils::time::microseconds{this->start.time_since_epoch()}.count(),
                                                         elapsed_us,
                                                         std::this_thread::get_id());
                }
            };

            std::mutex    file_mutex;
            bool          session_active;
            std::ofstream out_file;

            static /*inline*/ Profiler& get() {
                static Profiler instance;
                return instance;
            }

            Profiler() : session_active{false} {}

            ~Profiler() {
                this->EndSession();
            }

            void CloseOutput() {
                if (this->session_active) {
                    this->out_file << "]}";
                    this->out_file.flush();
                    this->out_file.close();
                    this->session_active = false;
                }
            }

            void AppendResults(const std::string_view name, double start, int64_t elapsed, std::thread::id tid) {
                std::stringstream repr;

                repr << std::setprecision(3) << std::fixed;
                repr << ",{";
                repr << "\"cat\":\"function\",";
                repr << "\"dur\":" << elapsed << ',';
                repr << "\"name\":\"" << name << "\",";
                repr << "\"ph\":\"X\",";
                repr << "\"pid\":0,";
                repr << "\"tid\":" << tid << ",";
                repr << "\"ts\":" << start;
                repr << "}";

                LOCK_BLOCK(this->file_mutex);
                if (this->session_active) {
                    this->out_file << repr.str();
                    this->out_file.flush();
                }
            }

        public:
            Profiler(Profiler const&)       = delete;
            Profiler(Profiler&&)            = delete;
            void operator=(Profiler const&) = delete;
            Profiler& operator=(Profiler&&) = delete;

            static void BeginSession(const std::string& filepath = "trace.json") {
                Profiler& pr = utils::Profiler::get();
                LOCK_BLOCK(pr.file_mutex);
                if (pr.session_active) {
                    pr.CloseOutput();
                }

                if (HEDLEY_LIKELY(filepath.length() > 0)) {
                    pr.out_file.open(filepath, std::ios_base::out);
                    pr.out_file << "{\"otherData\": {},\"traceEvents\":[{}";
                    pr.out_file.flush();
                    pr.session_active = true;
                }
            }

            static void EndSession() {
                LOCK_BLOCK(utils::Profiler::get().file_mutex);
                utils::Profiler::get().CloseOutput();
            }

            ATTR_NODISCARD ATTR_MAYBE_UNUSED
            static inline ProfileTimer CreateTimer(const std::string_view name) {
                return {name};
            }
    };
}

#if defined(UTILS_PROFILER_ENABLE) && UTILS_PROFILER_ENABLE
    #define UTILS_PROFILE_BEGIN_SESSION(filepath) utils::Profiler::BeginSession(filepath)
    #define UTILS_PROFILE_END_SESSION()           utils::Profiler::EndSession()
    #define UTILS_PROFILE_SCOPE(name)             auto HEDLEY_CONCAT(profile_scope_, __LINE__) = utils::Profiler::CreateTimer(name)
    #define UTILS_PROFILE_FUNCTION()              UTILS_PROFILE_SCOPE(UTILS_FUNCTION_NAME)
#else
    #define UTILS_PROFILE_BEGIN_SESSION(filepath)
    #define UTILS_PROFILE_END_SESSION()
    #define UTILS_PROFILE_SCOPE(name)
    #define UTILS_PROFILE_FUNCTION()
#endif

#endif // UTILS_PROFILER_HPP
