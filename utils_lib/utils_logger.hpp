#ifndef UTILS_LOGGER_HPP
#define UTILS_LOGGER_HPP

#include "utils_string.hpp"
#include "utils_print.hpp"
#include "utils_os.hpp"
#include "utils_time.hpp"
#include "utils_catch.hpp"
#include "utils_threading.hpp"
#include "utils_algorithm.hpp"

#include <iostream>
#include <ostream>
#include <fstream>
#include <mutex>


#ifdef LOG_ERROR_TRACE
    #undef LOG_ERROR_TRACE
#endif

/**
 *  Macro to print an error trace to Logger.
 */
#define LOG_ERROR_TRACE(E) utils::Logger::ErrorTrace(UTILS_TRACE_LOCATION, E);


namespace utils {
    /**
     *  \brief  A class with static methods for logging to std::cout and a log file.
     *          Create the Loging instance with Logger::Create(),
     *          and destroy it by calling Logger::Destroy()
     *
     *      Use https://github.com/gabime/spdlog for a more extensive logger.
     */
    class Logger {
        public:
            enum class Level {
                LOG_EMERGENCY = 0, // Emergency     - A "panic" condition usually affecting multiple apps/servers/sites. At this level it would usually notify all tech staff on call.
                LOG_ALERT     = 1, // Alert         - Should be corrected immediately, therefore notify staff who can fix the problem. An example would be the loss of a primary ISP connection.
                LOG_CRITICAL  = 2, // Critical      - Should be corrected immediately, but indicates failure in a secondary system, an example is a loss of a backup ISP connection.
                LOG_ERROR     = 3, // Error         - Non-urgent failures, these should be relayed to developers or admins; each item must be resolved within a given time.
                LOG_WARNING   = 4, // Warning       - Warning messages, not an error, but indication that an error will occur if action is not taken, e.g. file system 85% full - each item must be resolved within a given time.
                LOG_NOTICE    = 5, // Notice        - Events that are unusual but not error conditions - might be summarized in an email to developers or admins to spot potential problems - no immediate action required.
                LOG_INFO      = 6, // Informational - Normal operational messages - may be harvested for reporting, measuring throughput, etc. - no action required.
                LOG_DEBUG     = 7  // Debug         - Info useful to developers for debugging the application, not useful during operations.
            };

        private:
            bool screen_enabled;
            bool screen_paused;
            bool file_enabled;
            bool file_paused;
            bool file_timestamp;

            std::ostream&  screen_output;
            std::ofstream  log_file;
            Logger::Level  level_screen;
            Logger::Level  level_file;

            std::mutex logger_mutex;
            std::mutex file_mutex;
            std::mutex screen_mutex;

            static /*inline*/ Logger& get() {
                static Logger instance;
                return instance;
            }

            inline bool canLogScreen(const Logger::Level level = Logger::Level::LOG_EMERGENCY) const {
                return this->screen_enabled && !this->screen_paused
                    && level <= this->level_screen;
            }
            inline bool canLogFile(const Logger::Level level = Logger::Level::LOG_EMERGENCY) const {
                return this->file_enabled && !this->file_paused
                    && level <= this->level_file;
            }
            inline bool canLog(const Logger::Level level = Logger::Level::LOG_EMERGENCY) const {
                return this->canLogScreen(level) || this->canLogFile(level);
            }

            inline void write_to_screen(const std::string_view& text) {
                LOCK_BLOCK(utils::Logger::get().screen_mutex);

                if (HEDLEY_LIKELY(this->canLogScreen())) {
                    this->screen_output << text;
                }
            }

            inline void write_to_file(const std::string_view& text) {
                LOCK_BLOCK(utils::Logger::get().file_mutex);

                if (HEDLEY_LIKELY(this->canLogFile())) {
                    try {
                        if (HEDLEY_LIKELY(this->GetFileTimestamp())) {
                            this->log_file << utils::time::Timestamp("[%Y-%m-%d %H:%M:%S] ");
                        }

                        this->log_file << text;
                    } catch (std::exception const& e) {
                        std::cerr << "[Logger][ERROR] " << e.what() << std::endl;
                        utils::Logger::DestroyFile();
                    }
                }
            }

            template<typename ...Type>
            void hdr_colour_format(const Logger::Level level,
                                   const utils::os::command_t hdr_colour,
                                   const std::string_view& hdr_str,
                                   const std::string_view& format,
                                   const Type& ...args)
            {
                LOCK_BLOCK(utils::Logger::get().logger_mutex);

                if (HEDLEY_LIKELY(this->canLog(level))) {
                    const bool stamp = this->GetFileTimestamp();
                    this->Command(  utils::os::Console::FG
                                  | utils::os::Console::BOLD
                                  | hdr_colour);
                    this->Write("[" + std::string(hdr_str) + "]", true);

                    this->Command(utils::os::Console::RESET
                                | utils::os::Console::WHITE);
                    this->SetFileTimestamp(false);
                    this->Writef(" " + std::string(format) + utils::Logger::CRLF, args...);
                    this->SetFileTimestamp(stamp);
                    this->Command(utils::os::Console::RESET);
                }
            }

        public:
            Logger()
                : screen_enabled(true)
                , screen_paused(false)
                , file_enabled(false)
                , file_paused(false)
                , file_timestamp(true)
                , screen_output(std::cout)
                , level_screen(Level::LOG_INFO)
                , level_file(Level::LOG_INFO)
            {
                utils::os::EnableVirtualConsole();

                if (HEDLEY_UNLIKELY(this->screen_output.bad())) {
                    this->screen_enabled = false;
                    std::cerr << "[Logger][ERROR] Bad screen output stream!" << std::endl;
                }
            }

            Logger(Logger const&)         = delete;
            Logger(Logger&&)              = delete;
            void operator=(Logger const&) = delete;
            Logger& operator=(Logger&&)   = delete;

            /**
             *  Dtor: write line to outputs and close streams.
             */
            ~Logger() {
                const std::string end_line =
                        utils::Logger::CRLF
                      + utils::Logger::LINE<>
                      + utils::Logger::CRLF;

                if (this->file_enabled) {
                    this->SetFileTimestamp(false);
                    this->write_to_file(end_line);
                    this->log_file.close();
                    this->file_enabled = false;
                }

                this->write_to_screen(end_line);
            }

            static void InitFile(const std::string& fileName = "",
                                 const utils::Logger::Level level = utils::Logger::Level::LOG_INFO)
            {
                utils::Logger::DestroyFile();
                utils::Logger::SetFileLogLevel(level);
                LOCK_BLOCK(utils::Logger::get().file_mutex);
                bool enable_file = false;

                if (HEDLEY_LIKELY(fileName.length() > 0)) {
                    try {
                        utils::Logger::get().log_file.open(fileName, std::ios_base::app | std::ios_base::out);
                        enable_file = true;
                    } catch (std::exception const& e) {
                        std::cerr << "[Logger][ERROR] " << e.what() << std::endl;
                        enable_file = false;
                    }
                }

                utils::Logger::get().file_enabled = enable_file;
            }

            static void InitScreen(std::ostream& console_stream = std::cout,
                                   const utils::Logger::Level level = utils::Logger::Level::LOG_INFO)
            {
                utils::Logger::DestroyScreen();
                utils::Logger::SetScreenLogLevel(level);
                LOCK_BLOCK(utils::Logger::get().screen_mutex);

                if (HEDLEY_UNLIKELY(console_stream.bad())) {
                    utils::Logger::get().screen_output.tie(&std::cerr);
                    utils::Logger::get().screen_output << "[Logger][ERROR] Bad screen output stream!" << std::endl;
                } else {
                    utils::Logger::get().screen_output.tie(&console_stream);
                }

                utils::Logger::get().screen_enabled = true;
            }

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
            static void Create(const std::string& fileName = "",
                               const utils::Logger::Level level = utils::Logger::Level::LOG_INFO,
                               std::ostream& console_stream = std::cout)
            {
                utils::Logger::InitScreen(console_stream, level);
                utils::Logger::InitFile(fileName, level);
            }

            /**
             *  \brief  Destroy the file Logging instance by closing the output file.
             */
            static void DestroyFile() {
                LOCK_BLOCK(utils::Logger::get().file_mutex);

                if (utils::Logger::get().file_enabled) {
                    utils::Logger::get().log_file.close();
                    utils::Logger::get().file_enabled = false;
                }
            }

            /**
             *  \brief  Destroy the screen Logging instance.
             */
            static void DestroyScreen() {
                LOCK_BLOCK(utils::Logger::get().screen_mutex);

                if (utils::Logger::get().screen_enabled) {
                    utils::os::Command(utils::os::Console::RESET, utils::Logger::get().screen_output);
                    utils::Logger::get().screen_output.flush();
                    utils::Logger::get().screen_enabled = false;
                }
            }

            static inline std::ostream& GetConsoleStream() {
                return utils::Logger::get().screen_output;
            }

            static inline std::ofstream& GetFileStream() {
                return utils::Logger::get().log_file;
            }

            /**
             *  \brief  Write a seperator (line) to the stream.
             */
            static void Separator(void) {
                utils::Logger::Write(utils::Logger::LINE<>);
            }

            /**
             *  @brief  Format the given args and write to the streams.
             *
             *  @param  format
             *      The text to format in and write.
             *  @param  args
             */
            template<typename ...Type>
            static void Writef(const std::string& format, const Type& ...args) {
                if constexpr (sizeof...(args) > 0) {
                    utils::Logger::Write(utils::string::format(format, args...), utils::Logger::GetFileTimestamp());
                } else {
                    utils::Logger::Write(format, utils::Logger::GetFileTimestamp());
                }
            }

            /**
             *  @brief  Write the text <text> to the console and the log file.
             *
             *  @param  text
             *      The text to write.
             */
            static void Write(const std::string_view& text, const bool timestamp = false) {
                if (HEDLEY_LIKELY(utils::Logger::get().canLog())) {
                    const bool stamp = utils::Logger::GetFileTimestamp();
                    utils::Logger::SetFileTimestamp(timestamp);

                    utils::Logger::get().write_to_screen(text);
                    utils::Logger::get().write_to_file(text);

                    utils::Logger::SetFileTimestamp(stamp);
                }
            }

            static inline void WriteLn(const std::string& text, const bool timestamp = false) {
                utils::Logger::Write(text + utils::Logger::CRLF, timestamp);
            }

            template<typename ...Type>
            static void Debug(const std::string_view& format, const Type& ...args) {
                utils::Logger::get().hdr_colour_format(
                    utils::Logger::Level::LOG_DEBUG,
                    utils::os::Console::BG | utils::os::Console::BLUE, "DEBUG",
                    format, args...
                );
            }

            template<typename ...Type>
            static void Success(const std::string_view& format, const Type& ...args) {
                utils::Logger::get().hdr_colour_format(
                    utils::Logger::Level::LOG_INFO,
                    utils::os::Console::GREEN, "Success",
                    format, args...
                );
            }

            template<typename ...Type>
            static void Info(const std::string_view& format, const Type& ...args) {
                utils::Logger::get().hdr_colour_format(
                    utils::Logger::Level::LOG_INFO,
                    utils::os::Console::CYAN, "Info",
                    format, args...
                );
            }

            template<typename ...Type>
            static void Notice(const std::string_view& format, const Type& ...args) {
                utils::Logger::get().hdr_colour_format(
                    utils::Logger::Level::LOG_NOTICE,
                    utils::os::Console::BG |
                    utils::os::Console::BRIGHT |
                    utils::os::Console::CYAN,
                    "Notice",
                    format, args...
                );
            }

            template<typename ...Type>
            static void Warn(const std::string_view& format, const Type& ...args) {
                utils::Logger::get().hdr_colour_format(
                    utils::Logger::Level::LOG_WARNING,
                    utils::os::Console::YELLOW, "Warning",
                    format, args...
                );
            }

            template<typename ...Type>
            static void Error(const std::string_view& format, const Type& ...args) {
                utils::Logger::get().hdr_colour_format(
                    utils::Logger::Level::LOG_ERROR,
                    utils::os::Console::RED, "Error",
                    format, args...
                );
            }

            template<typename ...Type>
            static void Critical(const std::string_view& format, const Type& ...args) {
                utils::Logger::get().hdr_colour_format(
                    utils::Logger::Level::LOG_CRITICAL,
                    utils::os::Console::BG | utils::os::Console::RED, "Critical",
                    format, args...
                );
            }

            template<typename ...Type>
            static void Alert(const std::string_view& format, const Type& ...args) {
                utils::Logger::get().hdr_colour_format(
                    utils::Logger::Level::LOG_ALERT,
                    utils::os::Console::MAGENTA, "Alert",
                    format, args...
                );
            }

            template<typename ...Type>
            static void Emergency(const std::string_view& format, const Type& ...args) {
                utils::Logger::get().hdr_colour_format(
                    utils::Logger::Level::LOG_EMERGENCY,
                    utils::os::Console::BG |
                    utils::os::Console::BRIGHT |
                    utils::os::Console::MAGENTA,
                    "Emergency",
                    format, args...
                );
            }

            HEDLEY_NON_NULL(1,3)
            static void ErrorTrace(const char* file, const int line, const char* function, const std::exception& e) {
                LOCK_BLOCK(utils::Logger::get().logger_mutex);

                if (HEDLEY_LIKELY(utils::Logger::get().canLog())) {
                    utils::Logger::Command(  utils::os::Console::BG
                                           | utils::os::Console::BRIGHT
                                           | utils::os::Console::RED);
                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::BRIGHT
                                           | utils::os::Console::WHITE);
                    utils::Logger::Write(utils::Logger::CRLF
                                       + utils::Logger::LINE<'-'>, false);

                    utils::Logger::Command(  utils::os::Console::RESET
                                           | utils::os::Console::FG
                                           | utils::os::Console::BOLD
                                           | utils::os::Console::RED);
                    utils::Logger::Write("[ERROR] Exception thrown:\n  ", true);

                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::YELLOW);
                    utils::Logger::Write(e.what());

                    utils::Logger::Command(  utils::os::Console::RESET);
                    utils::Logger::Write("\n    at ");

                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::BRIGHT
                                           | utils::os::Console::CYAN);
                    utils::Logger::Write(file);

                    utils::Logger::Command(  utils::os::Console::RESET);
                    utils::Logger::Write(":");

                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::BRIGHT
                                           | utils::os::Console::CYAN);
                    utils::Logger::Write(std::to_string(line));

                    utils::Logger::Command(  utils::os::Console::RESET);
                    utils::Logger::Write("\n    inside: ");

                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::BRIGHT
                                           | utils::os::Console::MAGENTA);
                    utils::Logger::Write(function);

                    utils::Logger::Command(  utils::os::Console::BG
                                           | utils::os::Console::BRIGHT
                                           | utils::os::Console::RED);
                    utils::Logger::Command(  utils::os::Console::FG
                                           | utils::os::Console::BRIGHT
                                           | utils::os::Console::WHITE);
                    utils::Logger::Write(utils::Logger::CRLF
                                       + utils::Logger::LINE<'-'>, false);

                    utils::Logger::Command(  utils::os::Console::RESET);
                    utils::Logger::Write(utils::Logger::CRLF);
                } else {
                    std::cerr << ( utils::os::Console::RESET
                                 | utils::os::Console::FG | utils::os::Console::BRIGHT
                                 | utils::os::Console::RED)
                              << "[ERROR] Exception thrown:\n  "
                              << ( utils::os::Console::FG
                                 | utils::os::Console::YELLOW)
                              << e.what()
                              << utils::os::Console::RESET
                              << "\n    at "
                              << ( utils::os::Console::FG | utils::os::Console::BRIGHT
                                 | utils::os::Console::CYAN)
                              << file
                              << utils::os::Console::RESET
                              << ":"
                              << ( utils::os::Console::FG | utils::os::Console::BRIGHT
                                 | utils::os::Console::CYAN)
                              << line
                              << utils::os::Console::RESET
                              << "\n    inside: "
                              << ( utils::os::Console::FG | utils::os::Console::BRIGHT
                                 | utils::os::Console::MAGENTA)
                              << function
                              << utils::os::Console::RESET << std::flush << std::endl;
                }
            }

            template <typename Iterator, typename F>
            static void WriteProgress(Iterator start, Iterator end, F&& f) {
                if (HEDLEY_LIKELY(utils::Logger::get().canLogScreen())) {
                    utils::print::with_progressbar(start, end,
                                                   utils::Logger::GetConsoleStream(),
                                                   std::forward<F>(f));
                } else {
                    utils::algorithm::for_each(start, end, std::forward<F>(f));
                }
            }

            template <typename Container, typename F>
            static void WriteProgress(const Container& cont, F&& f) {
                if (HEDLEY_LIKELY(utils::Logger::get().canLogScreen())) {
                    utils::print::with_progressbar(cont,
                                                   utils::Logger::GetConsoleStream(),
                                                   std::forward<F>(f));
                } else {
                    utils::algorithm::for_each(cont, std::forward<F>(f));
                }
            }

            static const utils::Logger& Stream() {
                return utils::Logger::get();
            }

            template<typename T>
            static const utils::Logger& Stream(const T& arg) {
                if (HEDLEY_LIKELY(utils::Logger::get().canLog())) {
                    std::stringstream ss;
                    ss << arg;
                    utils::Logger::Write(ss.str());
                }

                return utils::Logger::Stream();
            }

            template<typename T, typename ...Type>
            static const utils::Logger& Stream(const T& arg, const Type& ...args) {
                if (HEDLEY_LIKELY(utils::Logger::get().canLog())) {
                    std::stringstream ss;
                    ss << arg;
                    ((ss << args), ...);
                    utils::Logger::Write(ss.str());
                }

                return utils::Logger::Stream();
            }

            template<typename ...Type>
            friend const utils::Logger& operator<<(const utils::Logger&, const Type& ...args)
            {
                if constexpr (sizeof...(Type) > 0) {
                    utils::Logger::Stream(args...);
                }

                return utils::Logger::Stream();
            }

            static inline void Command(const utils::os::command_t cmd) {
                LOCK_BLOCK(utils::Logger::get().screen_mutex);

                if (HEDLEY_LIKELY(utils::Logger::get().canLogScreen())) {
                    utils::os::Command(cmd, utils::Logger::GetConsoleStream());
                }
            }

            static inline void SetScreenTitle(const std::string_view& title) {
                LOCK_BLOCK(utils::Logger::get().screen_mutex);
                utils::os::SetScreenTitle(title, utils::Logger::GetConsoleStream());
            }

            static inline void PauseScreen(void) {
                LOCK_BLOCK(utils::Logger::get().screen_mutex);
                utils::Logger::get().screen_paused = true;
            }
            static inline void PauseFile(void) {
                LOCK_BLOCK(utils::Logger::get().file_mutex);
                utils::Logger::get().file_paused = true;
            }
            static inline void Pause(void) {
                utils::Logger::get().PauseScreen();
                utils::Logger::get().PauseFile();
            }

            static inline void ResumeScreen(void) {
                LOCK_BLOCK(utils::Logger::get().screen_mutex);
                utils::Logger::get().screen_paused = false;
            }
            static inline void ResumeFile(void) {
                LOCK_BLOCK(utils::Logger::get().file_mutex);
                utils::Logger::get().file_paused = false;
            }
            static inline void Resume(void) {
                utils::Logger::get().ResumeScreen();
                utils::Logger::get().ResumeFile();
            }

            static inline void SetFileTimestamp(const bool stamp) {
                LOCK_BLOCK(utils::Logger::get().file_mutex);
                utils::Logger::get().file_timestamp = stamp;
            }
            static inline bool GetFileTimestamp(void) {
                return utils::Logger::get().file_timestamp;
            }

            static inline void SetScreenLogLevel(const utils::Logger::Level level) {
                LOCK_BLOCK(utils::Logger::get().screen_mutex);
                utils::Logger::get().level_screen = level;
            }
            static inline utils::Logger::Level GetScreenLogLevel(void) {
                return utils::Logger::get().level_screen;
            }
            static inline void SetFileLogLevel(const utils::Logger::Level level) {
                LOCK_BLOCK(utils::Logger::get().file_mutex);
                utils::Logger::get().level_file = level;
            }
            static inline utils::Logger::Level GetFileLogLevel(void) {
                return utils::Logger::get().level_file;
            }

            /**
             *  Symbols for printing an image to the console.
             *
             *  █
             *  std::string_format("%c", 219)
             *  219
             */
            static inline constexpr size_t  CONSOLE_WIDTH
                #ifdef CATCH_CONFIG_CONSOLE_WIDTH
                    = (CATCH_CONFIG_CONSOLE_WIDTH - 1);
                #else
                    = 79;
                #endif
            static inline const std::string FILL  = "#";
            static inline const std::string EMPTY = " ";
            static inline const std::string CRLF  = "\r\n";
            template<char line_char = '-'>
            static inline const std::string LINE
                = std::string(utils::Logger::CONSOLE_WIDTH,
                              line_char)
                    + utils::Logger::CRLF;
    };
}

#endif // UTILS_LOGGER_HPP
