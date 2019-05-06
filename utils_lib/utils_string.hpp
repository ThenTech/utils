#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include "utils_exceptions.hpp"
#include "utils_memory.hpp"

#include <string>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <vector>

namespace utils::string {
    /**	\brief	Trim whitespace from the start of the given string (in-place).
     *
     *	\param	s
     *		A reference to the string to perform the operation.
     */
    [[maybe_unused]]
    static inline void ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                [](int c) {return !std::isspace(c);}));
    }

    /**	\brief	Trim whitespace from the end of the given string (in-place).
     *
     *	\param	s
     *		A reference to the string to perform the operation.
     */
    [[maybe_unused]]
    static inline void rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                [](int c) {return !std::isspace(c);}).base(), s.end());
    }

    /**	\brief	Trim whitespace from both start and end of the given string (in-place).
     *
     *	\param	s
     *		A reference to the string to perform the operation.
     */
    [[maybe_unused]]
    static inline void trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    /**	\brief	Trim whitespace from both start and end of the given string (copy).
     *
     *	\param	s
     *		A string to perform the operation.
     */
    [[maybe_unused]]
    static inline std::string trimmed(std::string s) {
        trim(s);
        return s;
    }

    /**
     *  \brief  Erase everything starting from erase_from (inclusive; in place).
     *
     *  \param  str
     *      The string to erase characters from.
     *  \param  erase_from
     *      The substring to look for and start erasing from.
     */
    [[maybe_unused]]
    static inline void strEraseFrom(std::string &str, const std::string& erase_from) {
        const size_t pos = str.find(erase_from);

        if (pos != std::string::npos) {
            str = str.substr(0, pos);
        }
    }

    /**
     *  \brief  Erase everything starting from the beginning of the string
     *          to erase_to (exclusive; in place).
     *
     *  \param  str
     *      The string to erase characters from.
     *  \param  erase_to
     *      The substring to look for and erase to.
     */
    [[maybe_unused]]
    static inline void strEraseTo(std::string &str, const std::string& erase_to) {
        const size_t pos = str.find(erase_to);

        if (pos != std::string::npos) {
            str = str.substr(pos);
        }
    }

    /**
     *  \brief  Erase everything starting from erase_from (inclusive; copy).
     *
     *  \param  str
     *      The string to erase characters from.
     *  \param  erase_from
     *      The substring to look for and start erasing from.
     *  \return Returns a copy where the appropriate parts were erased.
     */
    [[maybe_unused]]
    static inline std::string strErasedFrom(std::string str, const std::string& erase_from) {
        strEraseFrom(str, erase_from);
        return str;
    }

    /**
     *  \brief  Erase everything starting from the beginning of the string
     *          to erase_to (exclusive; copy).
     *
     *  \param  str
     *      The string to erase characters from.
     *  \param  erase_to
     *      The substring to look for and erase to.
     */
    [[maybe_unused]]
    static inline std::string strErasedTo(std::string str, const std::string& erase_to) {
        strEraseTo(str, erase_to);
        return str;
    }

    /**	\brief	Transform the string contents to uppercase (within the current locale) (in-place).
     *
     *          WARNING: ASCII only
     *
     *	\param	str
     *		A reference to the string to perform the operation.
     */
    [[maybe_unused]]
    static inline void strToUpper(std::string &str) {
        std::transform(str.begin(), str.end(), str.begin(),
            [](std::string::value_type ch) {
                return std::use_facet<std::ctype<std::string::value_type>>(std::locale()).toupper(ch);
            }
        );
    }

    /**	\brief	Transform the string contents to uppercase (within the current locale) (copying).
     *
     *          WARNING: ASCII only
     *
     *	\param	str
     *		A copy of the string to perform the operation.
     */
    [[maybe_unused]]
    static inline std::string strToUppercase(std::string str) {
        strToUpper(str);
        return str;
    }

    /**	\brief	Transform the string contents to lowercase (within the current locale) (in-place).
     *
     *          WARNING: ASCII only
     *
     *	\param	str
     *		A reference to the string to perform the operation.
     */
    [[maybe_unused]]
    static void strToLower(std::string &str) {
        std::transform(str.begin(), str.end(), str.begin(),
            [](std::string::value_type ch) {
                return std::use_facet<std::ctype<std::string::value_type>>(std::locale()).tolower(ch);
            }
        );
    }

    /**	\brief	Transform the string contents to lowercase (within the current locale) (copying).
     *
     *          WARNING: ASCII only
     *
     *	\param	str
     *		A copy of the string to perform the operation.
     */
    [[maybe_unused]]
    static inline std::string strToLowercase(std::string str) {
        strToLower(str);
        return str;
    }

    [[maybe_unused]]
    static inline bool strHasChar(const std::string &str, const char ch) {
        // string.h : strchr(str.c_str(), ch)
        return str.find(ch) != std::string::npos;
    }

    /**	\brief	Erase all consecutive occurrences of the given char
     *          within the given string (in-place).
     *          Only erase consecutive and repeating chars, keep unique.
     *
     *	\param	str
     *		A reference to the string to replace the character.
     *	\param	ch
     *		The characters to replace.
     */
    [[maybe_unused]]
    static inline void strEraseConsecutive(std::string &str, const char ch) {
        str.erase(std::unique(str.begin(), str.end(),
                                [&](const char lhs, const char rhs) {
                                    return (lhs == ch) && (lhs == rhs);
                                }
                             ), str.end());
    }

    /**	\brief	Replace all occurrences of from with to in the given
     *          std::string str.
     *
     *	\param	str
     *		A reference to the string to replace a substring.
     *	\param	from
     *		A reference to a string to replace.
     *	\param	to
     *		A reference to a string to replace with.
     */
    [[maybe_unused]]
    static inline void strReplaceAll(std::string &str, const std::string& from, const std::string& to) {
        if (from.size() == 0) return;

        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
    }

    /**	\brief	Replace all occurrences of from with to in the given
     *          std::string str.
     *
     *	\param	str
     *		A reference to the string to replace a substring.
     *	\param	from
     *		A char to replace.
     *	\param	to
     *		A reference to a string to replace with.
     */
    [[maybe_unused]]
    static inline void strReplaceAll(std::string &str, const char from, const std::string& to = "") {
        utils::string::strReplaceAll(str, std::string(1, from), to);
    }

    /**	\brief	Replace all occurrences of from with to in the given
     *          std::string str.
     *
     *	\param	str
     *		A reference to the string to replace a substring.
     *	\param	from
     *		A char to replace.
     *	\param	to
     *		A char to replace with.
     */
    [[maybe_unused]]
    static inline void strReplaceAll(std::string &str, const char from, const char to) {
        size_t start_pos = 0;
        while((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, 1, 1, to);
            start_pos++;
        }
    }

    /**	\brief	Erase all occurrences of erase in the given std::string str.
     *
     *	\param	str
     *		A reference to the string to erase a char.
     *	\param	erase
     *		A char to erase.
     */
    [[maybe_unused]]
    static inline void strEraseAll(std::string &str, const char erase) {
        str.erase(std::remove(str.begin(), str.end(), erase), str.end());
    }

    /**	\brief	Erase all occurrences of erase in the given std::string str.
     *
     *	\param	str
     *		A reference to the string to erase a string.
     *	\param	erase
     *		A string to erase.
     */
    [[maybe_unused]]
    static inline void strEraseAll(std::string &str, const std::string& erase) {
        if (erase.size() == 0) return;

        size_t start_pos = 0;
        while((start_pos = str.find(erase, start_pos)) != std::string::npos) {
            str.erase(start_pos, erase.length());
        }
    }

    /**	\brief	Convert the given char* to a wchar_t* variable.
     *
     *	\param	buffer
     *		The character buffer to convert.
     *	\return
     *		Returns L##buffer : each char is replaced with its wide version.
     */
    [[maybe_unused]]
    static inline wchar_t* convert2WSTR(const char* buffer) {
        const size_t size = std::strlen(buffer) * 2 + 1;
        wchar_t* WSTRbuff = new wchar_t[size];

        #ifdef _MSC_VER
            swprintf_s(WSTRbuff, size, L"%hs", buffer);
        #else
            swprintf(WSTRbuff, size, L"%hs", buffer);
        #endif

        return WSTRbuff;
    }

    /**
     * @brief addStringArrayToVector
     * @param v
     * @param s
     * @param str_char
     */
    [[maybe_unused]]
    static void strExtractQuotedStrings(std::vector<std::string> &v, const std::string& s, const char str_char='\'') {
        const size_t len = s.length() - 1u;
        v.clear();

        if (len < 2) return;

        size_t start = 0, end;

        do {
            start = s.find(str_char, start);
            if (start != std::string::npos) {
                end = s.find(str_char, ++start);
                if (end != std::string::npos) {
                    v.emplace_back(s.substr(start, end-start));
                    start = end + 1u;
                }
            }
        } while (start != std::string::npos && start < len);
    }

    /**
     *  \brief  Format the given args into the format string.
     *
     *  \tparam ...Type
     *      Variable argument list of params to expand in the format string.
     *  \param  format
     *      The format string to expand.
     *  \param  args
     *      The args tro fill in.
     *  \return
     *      Returns the format expanded with the args.
     */
    template<typename ... Type> [[maybe_unused]]
    static std::string format(const std::string& format, Type&& ...args) {
        const size_t size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
        auto buf = utils::memory::new_unique_array<char>(size);

        std::snprintf(buf.get(), size, format.c_str(), args...);

        return std::string(buf.get(), buf.get() + size - 1 ); // Strip '\0'
    }

    /**
     *  \brief  Format the given args into the format string and write to stream.
     *
     *  \tparam ...Type
     *      Variable argument list of params to expand in the format string.
     *  \param  format
     *      The format string to expand.
     *  \param  args
     *      The args tro fill in.
     */
    template<typename ... Type> [[maybe_unused]]
    static void print_format(std::ostream& stream, const std::string& format, Type&& ...args) {
        stream << utils::string::format(format, args...);
    }
}

#endif // UTILS_STRING_HPP
