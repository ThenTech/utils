#ifndef UTILS_OS_HPP
#define UTILS_OS_HPP

#include <iostream>

#include "utils_bits.hpp"
#include "utils_string.hpp"
#include "utils_misc.hpp"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
    #include <windows.h>
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

namespace utils::os {
    enum _Console_commands {
        _CC_CLS       = 1L << 0,
        _CC_CURSOR    = 1L << 1,
        _CC_RESET     = 1L << 2,

        _CC_FG        = 1L << 3,
        _CC_BG        = 1L << 4,
        _CC_BRIGHT    = 1L << 5,
        _CC_UNDERLINE = 1L << 6,
        _CC_BOLD      = 1L << 7,
        _CC_REVERSED  = 1L << 8,

        _CC_BLACK     = 1L << 9,
        _CC_RED       = 1L << 10,
        _CC_GREEN     = 1L << 11,
        _CC_YELLOW    = 1L << 12,
        _CC_BLUE      = 1L << 13,
        _CC_MAGENTA   = 1L << 14,
        _CC_CYAN      = 1L << 15,
        _CC_WHITE     = 1L << 16,
    };

    inline constexpr _Console_commands
    operator&(_Console_commands __a, _Console_commands __b) {
        return _Console_commands(utils::misc::to_underlying(__a) & utils::misc::to_underlying(__b));
    }

    inline constexpr _Console_commands
    operator|(_Console_commands __a, _Console_commands __b) {
        return _Console_commands(utils::misc::to_underlying(__a) | utils::misc::to_underlying(__b));
    }

    inline constexpr _Console_commands
    operator^(_Console_commands __a, _Console_commands __b) {
        return _Console_commands(utils::misc::to_underlying(__a) ^ utils::misc::to_underlying(__b));
    }

    inline constexpr _Console_commands
    operator~(_Console_commands __a) {
        return _Console_commands(~utils::misc::to_underlying(__a));
    }

    inline const _Console_commands&
    operator|=(_Console_commands& __a, _Console_commands __b) {
        return __a = __a | __b;
    }

    inline const _Console_commands&
    operator&=(_Console_commands& __a, _Console_commands __b) {
        return __a = __a & __b;
    }

    inline const _Console_commands&
    operator^=(_Console_commands& __a, _Console_commands __b) {
        return __a = __a ^ __b;
    }

    typedef _Console_commands Command;

    namespace Console {
        static constexpr Command CLS       = _CC_CLS;       ///< Clear entire screen
        static constexpr Command CURSOR    = _CC_CURSOR;    ///< Set cursor to start
        static constexpr Command RESET     = _CC_RESET;     ///< Reset formatting to black bg with gray fg

        static constexpr Command FG        = _CC_FG;        ///< Set foreground colour options
        static constexpr Command BG        = _CC_BG;        ///< Set background colour options
        static constexpr Command BRIGHT    = _CC_BRIGHT;    ///< Make text brighter
        static constexpr Command UNDERLINE = _CC_UNDERLINE; ///< Make text underlined
        static constexpr Command BOLD      = _CC_BOLD;      ///< Make text bold
        static constexpr Command REVERSED  = _CC_REVERSED;  ///< Reverse FG/BG colours

        static constexpr Command BLACK     = _CC_BLACK;     ///< Set colour
        static constexpr Command RED       = _CC_RED;       ///< Set colour
        static constexpr Command GREEN     = _CC_GREEN;     ///< Set colour
        static constexpr Command YELLOW    = _CC_YELLOW;    ///< Set colour
        static constexpr Command BLUE      = _CC_BLUE;      ///< Set colour
        static constexpr Command MAGENTA   = _CC_MAGENTA;   ///< Set colour
        static constexpr Command CYAN      = _CC_CYAN;      ///< Set colour
        static constexpr Command WHITE     = _CC_WHITE;     ///< Set colour
    };

    [[maybe_unused]]
    static void command(Command cmd, std::ostream& out) {
        #define BASE_ "\033["
        std::string cmd_str = "";

        if (cmd & Console::CLS) {
            cmd_str += BASE_ "2J";
        }

        if (cmd & Console::RESET) {
            cmd_str += BASE_ "0m";
        }

        if (cmd & Console::CURSOR) {
            cmd_str += BASE_ "H";
        }

        if (cmd > Console::BOLD) {
            // A colour will be set
            if (cmd & Console::BOLD) {
                cmd_str += BASE_ "1m";
            }

            if (cmd & Console::UNDERLINE) {
                cmd_str += BASE_ "4m";
            }

            if (cmd & Console::BG) {
                cmd_str += BASE_ "4";
            } else {
                // Foreground
                cmd_str += BASE_ "3";
            }

            cmd_str += utils::string::format("%d", utils::bits::ffs(uint64_t(cmd) / uint64_t(Console::BLACK)) - 1);

            if (cmd & Console::BRIGHT) {
                cmd_str += ";1";
            }

            cmd_str += "m";
        }

        out << cmd_str;
    }

    [[maybe_unused]]
    static void SetScreenTitle(const std::string& title, std::ostream& out) {
        out << "\033]2;" << title << "\007";
    }

    [[maybe_unused]]
    static void EnableVirtualConsole() {
        #ifdef ENABLE_VIRTUAL_TERMINAL_PROCESSING
            // Set-up Windows terminal
            int error = 0;

            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE) {
                error = int(GetLastError());
            }

            if (!error) {
                DWORD dwMode = 0;
                if (!GetConsoleMode(hOut, &dwMode)) {
                    error = int(GetLastError());
                }

                if (!error) {
                    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                    if (!SetConsoleMode(hOut, dwMode)) {
                        error = int(GetLastError());
                    }
                }
            }

            if (error) {
                std::cerr << "[utils::os::EnableVirtualConsole] Windows console error: " << error << std::endl;
            }
        #else
            // Unix is already good.
        #endif
    }
}

#endif // UTILS_OS_HPP
