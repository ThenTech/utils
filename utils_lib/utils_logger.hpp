#ifndef UTILS_LOGGER_HPP
#define UTILS_LOGGER_HPP

#include <iostream>
#include <ostream>
#include <fstream>
#include <iomanip>
#include <ctime>

#include "utils_string.hpp"
#include "utils_os.hpp"

namespace utils {
    /**
     *  @brief  A class with static methods for logging to std::cout and a log file.
     *          Create the Loging instance with Logger::Create(),
     *          and destroy it by calling Logger::Destroy()
     *
     *      Use https://github.com/gabime/spdlog for a more extensive logger.
     */
    class Logger {
        private:
            bool screen_enabled;
            bool screen_paused;
            bool file_enabled;
            bool file_paused;

            std::ostream&  screen_output;
            std::ofstream  log_file;

            static /*inline*/ Logger& get() {
                static Logger instance;
                return instance;
            }

            inline bool canLogScreen(void) const {
                return this->screen_enabled && !this->screen_paused;
            }
            inline bool canLogFile(void) const {
                return this->file_enabled && !this->file_paused;
            }
            inline bool canLog(void) const {
                return this->canLogScreen() || this->canLogFile();
            }

            inline void write_to_screen(const std::string &text) const {
                if (utils::Logger::get().canLogScreen()) {
                    utils::Logger::get().screen_output << text;
                }
            }

            inline void write_to_file(const std::string &text) const {
                if (utils::Logger::get().canLogFile()) {
                    utils::Logger::get().log_file << text;
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
            Logger(Logger&&)              = delete;
            void operator=(Logger const&) = delete;
            Logger& operator=(Logger&&)   = delete;

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
                // Console
                utils::os::EnableVirtualConsole();

                // File
                if (fileName.length() > 0) {
                    try {
                        utils::Logger::get().log_file.open(fileName, std::ios_base::app | std::ios_base::out);
                        utils::Logger::get().file_enabled = true;
                        return;
                    } catch (std::exception const& e) {
                        std::cerr << "[Logger] " << e.what() << std::endl;
                    }
                }

                utils::Logger::get().file_enabled = false;
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

                if (utils::Logger::get().file_enabled) {
                    utils::Logger::get().write_to_file(end_line);
                    utils::Logger::get().log_file.close();
                    utils::Logger::get().file_enabled = false;
                }

                utils::Logger::get().write_to_screen(end_line);
            }

            static inline std::ostream& GetConsoleStream() {
                return utils::Logger::get().screen_output;
            }

            static inline std::ofstream& GetFileStream() {
                return utils::Logger::get().log_file;
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
                if (utils::Logger::get().canLog()) {
                    if constexpr(sizeof...(args) > 0) {
                        const std::string text = utils::string::format(format, args...);
                        utils::Logger::get().write_to_file(text);
                        utils::Logger::get().write_to_screen(text);
                        return;
                    }

                    utils::Logger::Write(format, false);
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
                utils::Logger::get().write_to_screen(text);

                try {
                    if (utils::Logger::get().canLogFile()) {
                        if (timestamp) {
                            auto t  = std::time(nullptr);

                            #ifdef _MSC_VER
                                tm tm_l;
                                localtime_s(&tm_l, &t);
                                tm *tm = &tm_l;
                            #else
                                auto tm = std::localtime(&t);
                            #endif

                            utils::Logger::get().log_file << std::put_time(tm, "[%Y-%m-%d %H:%M:%S] ");
                        }

                        utils::Logger::get().log_file << text;
                    }
                } catch (std::exception const& e) {
                    std::cerr << "[Logger] " << e.what() << std::endl;
                    utils::Logger::Destroy();
                }
            }

            static inline void WriteLn(const std::string &text, bool timestamp=true) {
                utils::Logger::Write(text + utils::Logger::CRLF, timestamp);
            }

            template<typename ... Type>
            static void Success(const std::string& format, Type ...args) {
                if (utils::Logger::get().canLog()) {
                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::BOLD
                                           | utils::os::Console::GREEN);
                    utils::Logger::Write("[Success] ");

                    utils::Logger::Command(utils::os::Console::RESET);
                    utils::Logger::Writef(format + "\n", args...);
                }
            }

            template<typename ... Type>
            static void Info(const std::string& format, Type ...args) {
                if (utils::Logger::get().canLog()) {
                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::BOLD
                                           | utils::os::Console::CYAN);
                    utils::Logger::Write("[Info] ");

                    utils::Logger::Command(utils::os::Console::RESET);
                    utils::Logger::Writef(format + "\n", args...);
                }
            }

            template<typename ... Type>
            static void Warn(const std::string& format, Type ...args) {
                if (utils::Logger::get().canLog()) {
                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::BOLD
                                           | utils::os::Console::YELLOW);
                    utils::Logger::Write("[Warning] ");

                    utils::Logger::Command(utils::os::Console::RESET);
                    utils::Logger::Writef(format + "\n", args...);
                }
            }

            template<typename ... Type>
            static void Error(const std::string& format, Type ...args) {
                if (utils::Logger::get().canLog()) {
                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::BOLD
                                           | utils::os::Console::RED);
                    utils::Logger::Write("[Error] ");

                    utils::Logger::Command(utils::os::Console::RESET);
                    utils::Logger::Writef(format + "\n", args...);
                }
            }

            static void WriteProgress(const size_t& iteration, const size_t& total) {
                static constexpr size_t LEN = 55u;
                static size_t stepu = 0u;

                if (!utils::Logger::get().canLogScreen()) return;

                const bool done = (iteration == total);

                if (iteration == 0) {
                    stepu = size_t(float(total) / std::min(LEN, total));
                } else if (done || iteration % stepu == 0) {
                    const float progress = float(iteration) / total;

                    const size_t filled_len = std::min(LEN, size_t(LEN * progress));

                    utils::Logger::get().screen_output
                        << "\rProgress |"
                        << std::string(filled_len, utils::Logger::FILL[0])
                        << std::string(LEN - filled_len, '-')
                        << "| "
                        << utils::string::format("%6.2f%%", progress * 100.0f)
                        << std::flush;

                    if (done) {
                        utils::Logger::get().screen_output << std::endl;
                    }
                }
            }

            static inline void Command(utils::os::command_t cmd) {
                if (utils::Logger::get().canLogScreen()) {
                    utils::os::command(cmd, utils::Logger::get().screen_output);
                }
            }

            static inline void SetScreenTitle(const std::string& title) {
                utils::os::SetScreenTitle(title, utils::Logger::get().screen_output);
            }

            static inline void PauseScreen(void) {
                utils::Logger::get().screen_paused = true;
            }
            static inline void PauseFile(void) {
                utils::Logger::get().file_paused = true;
            }

            static inline void Pause(void) {
                utils::Logger::get().PauseScreen();
                utils::Logger::get().PauseFile();
            }

            static inline void ResumeScreen(void) {
                utils::Logger::get().screen_paused = false;
            }

            static inline void ResumeFile(void) {
                utils::Logger::get().file_paused = false;
            }

            static inline void Resume(void) {
                utils::Logger::get().ResumeScreen();
                utils::Logger::get().ResumeFile();
            }

            /**
             *  Symbols for printing an image to the console.
             *
             *  █
             *  std::string_format("%c", 219)
             *  219
             */
            static inline const std::string FILL  = "#";
            static inline const std::string EMPTY = " ";
            static inline const std::string CRLF  = "\r\n";
    };
}

#endif // UTILS_LOGGER_HPP
