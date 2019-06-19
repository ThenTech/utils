#ifndef UTILS_OS_HPP
#define UTILS_OS_HPP

#include "utils_bits.hpp"
#include "utils_string.hpp"
#include "utils_traits.hpp"

#if defined(UTILS_TRAITS_OS_WIN)
    #include <windows.h>
    #define UTILS_OS_ENABLE_VIRTUAL_TERMINAL 0x0004
#endif

#include <iostream>


namespace utils::os {
    namespace internal {
        enum _Console_commands {
            _CC_CLS       = 1L << 0,
            _CC_CURSOR    = 1L << 1,
            _CC_RESET     = 1L << 2,

            _CC_FG        = 1L << 3,
            _CC_BG        = 1L << 4,
            _CC_BRIGHT    = 1L << 5,
            _CC_UNDERLINE = 1L << 6,
            _CC_BOLD      = 1L << 7,
            _CC_ITALIC    = 1L << 8,
            _CC_REVERSED  = 1L << 9,

            _CC_BLACK     = 1L << 10,
            _CC_RED       = 1L << 11,
            _CC_GREEN     = 1L << 12,
            _CC_YELLOW    = 1L << 13,
            _CC_BLUE      = 1L << 14,
            _CC_MAGENTA   = 1L << 15,
            _CC_CYAN      = 1L << 16,
            _CC_WHITE     = 1L << 17,
        };

        inline constexpr _Console_commands
        operator&(const _Console_commands __a, const _Console_commands __b) {
            return _Console_commands(utils::traits::to_underlying(__a) & utils::traits::to_underlying(__b));
        }

        inline constexpr _Console_commands
        operator|(const _Console_commands __a, const _Console_commands __b) {
            return _Console_commands(utils::traits::to_underlying(__a) | utils::traits::to_underlying(__b));
        }

        inline constexpr _Console_commands
        operator^(const _Console_commands __a, const _Console_commands __b) {
            return _Console_commands(utils::traits::to_underlying(__a) ^ utils::traits::to_underlying(__b));
        }

        inline constexpr _Console_commands
        operator~(const _Console_commands __a) {
            return _Console_commands(~utils::traits::to_underlying(__a));
        }

        inline const _Console_commands&
        operator|=(_Console_commands& __a, const _Console_commands __b) {
            return __a = __a | __b;
        }

        inline const _Console_commands&
        operator&=(_Console_commands& __a, const _Console_commands __b) {
            return __a = __a & __b;
        }

        inline const _Console_commands&
        operator^=(_Console_commands& __a, const _Console_commands __b) {
            return __a = __a ^ __b;
        }
    }

    /**
     *  \brief  Public console command variable type.
     */
    typedef utils::os::internal::_Console_commands command_t;

    /**
     *  Namespace withe the concole commands types.
     *  These can be aggregated with the common bitwise operators.
     */
    namespace Console {
        static constexpr command_t CLS       = utils::os::internal::_CC_CLS;       ///< Clear entire screen
        static constexpr command_t CURSOR    = utils::os::internal::_CC_CURSOR;    ///< Set cursor to start
        static constexpr command_t RESET     = utils::os::internal::_CC_RESET;     ///< Reset formatting to black bg with gray fg

        static constexpr command_t FG        = utils::os::internal::_CC_FG;        ///< Set foreground colour options
        static constexpr command_t BG        = utils::os::internal::_CC_BG;        ///< Set background colour options
        static constexpr command_t BRIGHT    = utils::os::internal::_CC_BRIGHT;    ///< Make text brighter
        static constexpr command_t UNDERLINE = utils::os::internal::_CC_UNDERLINE; ///< Make text underlined
        static constexpr command_t BOLD      = utils::os::internal::_CC_BOLD;      ///< Make text bold
        static constexpr command_t ITALIC    = utils::os::internal::_CC_ITALIC;    ///< Make text italic
        static constexpr command_t REVERSED  = utils::os::internal::_CC_REVERSED;  ///< Reverse FG/BG colours

        static constexpr command_t BLACK     = utils::os::internal::_CC_BLACK;     ///< Set colour
        static constexpr command_t RED       = utils::os::internal::_CC_RED;       ///< Set colour
        static constexpr command_t GREEN     = utils::os::internal::_CC_GREEN;     ///< Set colour
        static constexpr command_t YELLOW    = utils::os::internal::_CC_YELLOW;    ///< Set colour
        static constexpr command_t BLUE      = utils::os::internal::_CC_BLUE;      ///< Set colour
        static constexpr command_t MAGENTA   = utils::os::internal::_CC_MAGENTA;   ///< Set colour
        static constexpr command_t CYAN      = utils::os::internal::_CC_CYAN;      ///< Set colour
        static constexpr command_t WHITE     = utils::os::internal::_CC_WHITE;     ///< Set colour
    };

    /**
     *  \brief  Process the given commands \p cmd and write the appropriate
     *          virtual terminal codes to the given stream.
     *
     *          For Windows, call EnableVirtualConsole() once (standard output handle only).
     *
     *  \param  cmd
     *      The commands to process.
     *  \param out
     *      The stream to write the virtual codes to. (default: std::cout)
     */
    ATTR_MAYBE_UNUSED
    static void Command(const command_t cmd, std::ostream& out = std::cout) {
        #define BASE_ "\033["
        out.flush();

        if (cmd & Console::CLS) {
            out << BASE_ "2J";
        }

        if (cmd & Console::RESET) {
            out << BASE_ "0m";
        }

        if (cmd & Console::CURSOR) {
            out << BASE_ "H";
        }

        if (cmd & Console::BOLD) {
            out << BASE_ "1m";
        }

        if (cmd & Console::UNDERLINE) {
            out << BASE_ "4m";
        }

        if (cmd & Console::ITALIC) {
            out << BASE_ "3m";
        }

        if (cmd >= Console::BLACK) {
            out << BASE_;
            // A colour will be set
            if (cmd & Console::BG) {
                out << "4"; // Background
            } else {
                out << "3"; // Foreground
            }

            out << (utils::bits::msb(uint64_t(cmd) / uint64_t(Console::BLACK)) - 1);

            if (cmd & Console::BRIGHT) {
                out << ";1";
            }

            out << "m";
        }
    }

    /**
     *  \brief  Set the title of the current terminal window
     *          by calling the corresponding virtual code.
     *
     *  \param  title
     *      The value to set the title to.
     *  \param  out
     *      The stream to write the virtual codes to. (default: std::cout)
     */
    ATTR_MAYBE_UNUSED
    static void SetScreenTitle(const std::string_view& title, std::ostream& out = std::cout) {
        out << "\033]2;" << title << "\007";
    }

    /**
     *  \brief  Enable virtual commands processing for the default output handle
     *          on Windows. Unix is supported by default.
     */
    ATTR_MAYBE_UNUSED
    static void EnableVirtualConsole() {
        #ifdef UTILS_OS_ENABLE_VIRTUAL_TERMINAL
            // Set-up Windows terminal
            int error = 0;
            do {
                HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
                if (hOut == INVALID_HANDLE_VALUE) {
                    error = int(GetLastError());
                    break;
                }

                DWORD dwMode = 0;
                if (!GetConsoleMode(hOut, &dwMode)) {
                    error = int(GetLastError());
                    break;
                }

                dwMode |= UTILS_OS_ENABLE_VIRTUAL_TERMINAL;
                if (!SetConsoleMode(hOut, dwMode)) {
                    error = int(GetLastError());
                    break;
                }
            } while(false);

            if (error) {
                std::cerr << "[utils::os::EnableVirtualConsole] Windows console error: " << error << std::endl;
            }
        #else
            // Unix is already good.
        #endif
    }
}

#undef UTILS_OS_ENABLE_VIRTUAL_TERMINAL
#endif // UTILS_OS_HPP
