#ifndef UTILS_LOGGER_HPP
#define UTILS_LOGGER_HPP

#include <iostream>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <ctime>

#include "utils_string.hpp"

namespace utils {
    /**
     *  @brief  A class with static methods for logging to std::cout and a log file.
     *          Create the Loging instance with Logger::Create(),
     *          and destroy it by calling Logger::Destroy()
     */
    class Logger {
        private:
            bool screen_enabled;
            bool screen_paused;
            bool file_enabled;
            bool file_paused;

            std::ostream&  screen_output;
            std::ofstream  log_file;

            static Logger instance;

            inline bool canLogScreen(void) {
                return this->screen_enabled && !this->screen_paused;
            }
            inline bool canLogFile(void) {
                return this->file_enabled && !this->file_paused;
            }
            inline bool canLog(void) {
                return this->canLogScreen() || this->canLogFile();
            }

            inline void write_to_screen(const std::string &text) {
                if (utils::Logger::instance.canLogScreen()) {
                    utils::Logger::instance.screen_output << text;
                }
            }

            inline void write_to_file(const std::string &text) {
                if (utils::Logger::instance.canLogFile()) {
                    utils::Logger::instance.log_file << text;
                }
            }

        public:
            Logger()
                : screen_enabled(true)
                , screen_paused(false)
                , file_enabled(false)
                , file_paused(false)
                , screen_output(std::cout)
            {
                // Empty
            }

            Logger(Logger const&)         = delete;
            void operator=(Logger const&) = delete;

            /**
             *  @brief  Create the Logger instance from a filename.
             *          If the file does not exist, it will be created, text will be appended otherwise.
             *
             *          Disable logging if fileName is an empty string or on error.
             *
             *          Writes version and author info to the log on start.
             *
             *  @param  fileName
             *      The log file to use.
             */
            static void Create(const std::string &fileName = "") {
                if (fileName.length() > 0) {
                    try {
                        utils::Logger::instance.log_file.open(fileName, std::ios_base::app | std::ios_base::out);
                        utils::Logger::instance.file_enabled = true;
                        return;
                    } catch (std::exception const& e) {
                        std::cerr << "[Logger] " << e.what() << std::endl;
                    }
                }

                utils::Logger::instance.file_enabled = false;
            }

            /**
             *  @brief  Detroy the Logging instance by closing the output file.
             */
            static void Destroy() {
                static const std::string
                        end_line = utils::Logger::CRLF
                                 + std::string(80, '-')
                                 + utils::Logger::CRLF
                                 + utils::Logger::CRLF;

                if (utils::Logger::instance.file_enabled) {
                    utils::Logger::instance.write_to_file(end_line);
                    utils::Logger::instance.log_file.close();
                    utils::Logger::instance.file_enabled = false;
                }

                utils::Logger::instance.write_to_screen(end_line);
            }

            /**
             *  @brief  Format the given args and write to the streams.
             *
             *  @param  format
             *      The text to format in and write.
             *  @param  args
             */
            template<typename ... Type>
            static void Writef(const std::string& format, Type ...args) {
                if (utils::Logger::instance.canLog()) {
                    const std::string text = utils::string::format(format, args...);

                    utils::Logger::instance.write_to_file(text);
                    utils::Logger::instance.write_to_screen(text);
                }
            }

            /**
             *  @brief  Write the text <text> to the console and the log file.
             *
             *  @param  text
             *      The text to write.
             *  @param  timestamp
             *      Whether to include a timestamp (in the file only).
             */
            static void Write(const std::string &text, bool timestamp=true) {
                utils::Logger::instance.write_to_screen(text);

                try {
                    if (utils::Logger::instance.canLogFile()) {
                        if (timestamp) {
                            auto t  = std::time(nullptr);

                            #ifdef _MSC_VER
                                tm tm_l;
                                localtime_s(&tm_l, &t);
                                tm *tm = &tm_l;
                            #else
                                auto tm = std::localtime(&t);
                            #endif

                            utils::Logger::instance.log_file << std::put_time(tm, "[%Y-%m-%d %H:%M:%S] ")
                                                             << text;
                        }

                        utils::Logger::instance.log_file << text;
                    }
                } catch (std::exception const& e) {
                    std::cerr << "[Logger] " << e.what() << std::endl;
                    utils::Logger::Destroy();
                }
            }

            static inline void WriteLn(const std::string &text, bool timestamp=true) {
                utils::Logger::Write(text + utils::Logger::CRLF, timestamp);
            }

            static void WriteProgress(const size_t& iteration, const size_t& total) {
                static constexpr size_t LEN = 55u;
                static size_t stepu = 0u;

                if (!utils::Logger::instance.canLogScreen()) return;

                const bool done = (iteration == total);

                if (iteration == 0) {
                    stepu = size_t(float(total) / std::min(LEN, total));
                } else if (done || iteration % stepu == 0) {
                    const float progress = float(iteration) / total;

                    const size_t filled_len = std::min(LEN, size_t(LEN * progress));

                    utils::Logger::instance.screen_output
                        << "\rProgress |"
                        << std::string(filled_len, utils::Logger::FILL[0])
                        << std::string(LEN - filled_len, '-')
                        << "| "
                        << utils::string::format("%6.2f%%", progress * 100.0f)
                        << std::flush;

                    if (done) {
                        utils::Logger::instance.screen_output << std::endl;
                    }
                }
            }

            static inline void PauseScreen(void) {
                utils::Logger::instance.screen_paused = true;
            }
            static inline void PauseFile(void) {
                utils::Logger::instance.file_paused = true;
            }

            static inline void Pause(void) {
                utils::Logger::instance.PauseScreen();
                utils::Logger::instance.PauseFile();
            }

            static inline void ResumeScreen(void) {
                utils::Logger::instance.screen_paused = false;
            }

            static inline void ResumeFile(void) {
                utils::Logger::instance.file_paused = false;
            }

            static inline void Resume(void) {
                utils::Logger::instance.ResumeScreen();
                utils::Logger::instance.ResumeFile();
            }

            static const std::string FILL;
            static const std::string EMPTY;
            static const std::string CRLF;
    };
}

#endif // UTILS_LOGGER_HPP
