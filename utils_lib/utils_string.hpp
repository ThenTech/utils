#ifndef UTILS_STRING_HPP
#define UTILS_STRING_HPP

#include "utils_exceptions.hpp"
#include "utils_memory.hpp"
#include "utils_traits.hpp"

#include <string>
#include <string_view>
#include <locale>
#include <codecvt>
#include <cstring>
#include <algorithm>
#include <sstream>
#include <vector>


namespace utils::string {
    /**
     *  \brief  Test if the given string \p str contains the given char \p ch.
     *
     *  \param  str
     *      The std::string to check.
     *  \param  ch
     *      The char to look for.
     *  \param  start
     *      The starting offset to start looking. (default: 0)
     *  \return Returns an `std:optional<size_t>` holding the offset from
     *          `str.begin()` if the char was found, or nothing if not.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr utils::traits::found_t contains(const std::string_view& str, const char ch, const size_t start = 0) {
        if (const size_t pos = str.find(ch, start);
            pos != std::string::npos)
        {
            return { pos };
        }
        return std::nullopt;
    }

    /**
     *  \brief  Test if the given string \p str contains the other string \p part.
     *
     *  \param  str
     *      The std::string to check.
     *  \param  part
     *      The std::string to look for.
     *  \param  start
     *      The starting offset to start looking. (default: 0)
     *  \return Returns an `std:optional<size_t>` holding the offset from
     *          `str.begin()` if the part was found, or nothing if not.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr utils::traits::found_t contains(const std::string_view& str, const std::string_view& part, const size_t start = 0) {
        if (const size_t pos = str.find(part, start);
            pos != std::string::npos)
        {
            return { pos };
        }
        return std::nullopt;
    }

    /**
     *  \brief  Check if \p str starts with the given char.
     *
     *  \param  str
     *      The std::string to check.
     *  \param  ch
     *      The char to look for.
     *  \return Returns true if str[0] == ch.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool starts_with(const std::string_view& str, const char ch) {
        return ch && str.size() && str.front() == ch;
    }

    /**
     *  \brief  Check if \p str starts with the given string.
     *
     *  \param  str
     *      The std::string to check.
     *  \param  start
     *      The string to look for.
     *  \return Returns true if str[0:start.size()] == start.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool starts_with(const std::string_view& str, const std::string_view& start) {
        return !start.size() || str.size() < start.size()
             ? false
             : std::equal(start.begin(), start.end(),
                          str.begin());
    }

    /**
     *  \brief  Check if \p str ends with the given char.
     *
     *  \param  str
     *      The std::string to check.
     *  \param  ch
     *      The char to look for.
     *  \return Returns true if str[-1] == ch.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool ends_with(const std::string_view& str, const char ch) {
        return ch && str.size() && str.back() == ch;
    }

    /**
     *  \brief  Check if \p str ends with the given string.
     *
     *  \param  str
     *      The std::string to check.
     *  \param  end
     *      The char* to look for.
     *  \return Returns true if str[-end.size():] == end.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline constexpr bool ends_with(const std::string_view& str, const std::string_view& end) {
        return !end.size() || str.size() < end.size()
             ? false
             : std::equal(end.begin(), end.end(),
                          str.end() - static_cast<std::string::difference_type>(end.size()));
    }

    /**	\brief	Trim whitespace from the start of the given string (in-place).
     *
     *	\param	s
     *		A reference to the string to perform the operation.
     */
    ATTR_MAYBE_UNUSED
    static inline void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(),
                [](int c) {return !std::isspace(c);}));
    }

    /**	\brief	Trim whitespace from the end of the given string (in-place).
     *
     *	\param	s
     *		A reference to the string to perform the operation.
     */
    ATTR_MAYBE_UNUSED
    static inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(),
                [](int c) {return !std::isspace(c);}).base(), s.end());
    }

    /**	\brief	Trim whitespace from both start and end of the given string (in-place).
     *
     *	\param	s
     *		A reference to the string to perform the operation.
     */
    ATTR_MAYBE_UNUSED
    static inline void trim(std::string& s) {
        ltrim(s);
        rtrim(s);
    }

    /**	\brief	Trim whitespace from both start and end of the given string (copy).
     *
     *	\param	s
     *		A string to perform the operation.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::string trimmed(std::string s) {
        trim(s);
        return s;
    }

    /**
     *  \brief  Erase everything starting from \p erasefrom (inclusive; in place).
     *
     *  \param  str
     *      The string to erase characters from.
     *  \param  erasefrom
     *      The substring to look for and start erasing from.
     */
    ATTR_MAYBE_UNUSED
    static inline void erase_from(std::string& str, const std::string_view& erasefrom) {
        if (const auto found = utils::string::contains(str, erasefrom)) {
            str = str.substr(0, found.value());
        }
    }

    /**
     *  \brief  Erase everything starting from the beginning of the string
     *          to \p eraseto (exclusive; in place).
     *
     *  \param  str
     *      The string to erase characters from.
     *  \param  eraseto
     *      The substring to look for and erase to.
     */
    ATTR_MAYBE_UNUSED
    static inline void erase_to(std::string& str, const std::string_view& eraseto) {
        if (const auto found = utils::string::contains(str, eraseto)) {
            str = str.substr(found.value());
        }
    }

    /**
     *  \brief  Erase everything starting from \p erasefrom (inclusive; copy).
     *
     *  \param  str
     *      The string to erase characters from.
     *  \param  erasefrom
     *      The substring to look for and start erasing from.
     *  \return Returns a copy where the appropriate parts were erased.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::string erased_from(std::string str, const std::string_view& erasefrom) {
        erase_from(str, erasefrom);
        return str;
    }

    /**
     *  \brief  Erase everything starting from the beginning of the string
     *          to \p eraseto (exclusive; copy).
     *
     *  \param  str
     *      The string to erase characters from.
     *  \param  eraseto
     *      The substring to look for and erase to.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::string erased_to(std::string str, const std::string_view& eraseto) {
        erase_to(str, eraseto);
        return str;
    }

    /**	\brief	Transform the string contents to uppercase (within the current locale) (in-place).
     *
     *	\param	str
     *		A reference to the string to perform the operation.
     */
    template<typename CharT> ATTR_MAYBE_UNUSED
    static inline void to_upper(std::basic_string<CharT>& str) {
        std::transform(str.begin(), str.end(), str.begin(),
            [](CharT ch) {
                return std::use_facet<std::ctype<CharT>>(std::locale()).toupper(ch);
            }
        );
    }

    /**	\brief	Transform the string contents to uppercase (within the current locale) (copying).
     *
     *	\param	str
     *		A copy of the string to perform the operation.
     */
    template<typename CharT> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::basic_string<CharT> to_uppercase(std::basic_string<CharT> str) {
        to_upper(str);
        return str;
    }

    /**	\brief	Transform the string contents to lowercase (within the current locale) (in-place).
     *
     *	\param	str
     *		A reference to the string to perform the operation.
     */
    template<typename CharT> ATTR_MAYBE_UNUSED
    static inline void to_lower(std::basic_string<CharT>& str) {
        std::transform(str.begin(), str.end(), str.begin(),
            [](CharT ch) {
                return std::use_facet<std::ctype<CharT>>(std::locale()).tolower(ch);
            }
        );
    }

    /**	\brief	Transform the string contents to lowercase (within the current locale) (copying).
     *
     *	\param	str
     *		A copy of the string to perform the operation.
     */
    template<typename CharT> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::basic_string<CharT> to_lowercase(std::basic_string<CharT> str) {
        to_lower(str);
        return str;
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
    ATTR_MAYBE_UNUSED
    static inline void erase_consecutive(std::string& str, const char ch) {
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
    ATTR_MAYBE_UNUSED
    static inline void replace_all(std::string& str, const std::string_view& from, const std::string_view& to) {
        if (from.size() == 0) return;

        utils::traits::found_t found = 0ull;
        while ((found = utils::string::contains(str, from, *found))) {
            str.replace(*found, from.length(), to);
            *found += to.length();
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
    ATTR_MAYBE_UNUSED
    static inline void replace_all(std::string& str, const char from, const std::string_view& to = "") {
        utils::string::replace_all(str, std::string_view{&from, 1}, to);
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
    ATTR_MAYBE_UNUSED
    static inline void replace_all(std::string& str, const char from, const char to) {
        utils::traits::found_t found = 0ull;
        while ((found = utils::string::contains(str, from, *found))) {
            str.replace(*found, 1, 1, to);
            (*found)++;
        }
    }

    /**	\brief	Erase all occurrences of erase in the given std::string str.
     *
     *	\param	str
     *		A reference to the string to erase a char.
     *	\param	erase
     *		A char to erase.
     */
    ATTR_MAYBE_UNUSED
    static inline void erase_all(std::string& str, const char erase) {
        str.erase(std::remove(str.begin(), str.end(), erase), str.end());
    }

    /**	\brief	Erase all occurrences of erase in the given std::string str.
     *
     *	\param	str
     *		A reference to the string to erase a string.
     *	\param	erase
     *		A string to erase.
     */
    ATTR_MAYBE_UNUSED
    static inline void erase_all(std::string& str, const std::string_view& erase) {
        if (erase.size() == 0) return;

        utils::traits::found_t found = 0ull;
        while ((found = utils::string::contains(str, erase, *found))) {
            str.erase(*found, erase.length());
        }
    }

    /**
     *  \brief  Convert the given string to an std::wstring.
     *
     *	\param	str
     *		The string to convert.
     *  \return
     *      Returns the converted data as an std::wstring.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::wstring to_wstring(const std::string_view& str) {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>().from_bytes(str.data());
    }

    /**
     *  \brief  Convert the given wide string to an std::string.
     *
     *	\param	str
     *		The character buffer to convert.
     *  \return
     *      Returns the converted data as an std::string.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::string to_string(const std::wstring_view& str) {
        return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>().to_bytes(str.data());
    }

    /**
     *  \brief  Surround the given string with the \p quote char. (in-place)
     *          If it is already surrounded, do nothing.
     *
     *  \param  str
     *      The string to surround.
     *  \param  quote
     *      The character to surround the string with.
     */
    ATTR_MAYBE_UNUSED
    static inline void quote(std::string& str, const char quote='\"') {
        if (utils::string::starts_with(str, quote) && utils::string::ends_with(str, quote)) {
            return;
        }

        std::string out(str.size() + 2, quote);
        str = out.replace(1, str.size(), str);
    }

    /**
     *  \brief  Surround the given string with the \p quote char. (copy)
     *          If it is already surrounded, do nothing.
     *
     *  \param  str
     *      The string to surround.
     *  \param  quote
     *      The character to surround the string with.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::string quoted(std::string str, const char quote='\"') {
        utils::string::quote(str, quote);
        return str;
    }

    /**
     *  \brief  Extract a vector of strings from the given string \p s that have
     *          been quoted with char \p str_char.
     *
     *          e.g. 'str1', 'str2' => extract every string between 2 `'`
     *                  => returns [ "str1", "str2" ]
     *
     *  \param  v
     *      The vector to add the strings to. Will be cleared on start.
     *  \param  s
     *      The string to look for quoted strings in.
     *  \param  str_char
     *      The char the strings are quoted in.
     *  \return Returns the list of quoted strings, without quotes.
     */
    ATTR_MAYBE_UNUSED
    static void extract_quoted(std::vector<std::string_view>& v, const std::string_view& s, const char str_char='\'') {
        const size_t len = s.length() - 1u;
        v.clear();

        if (len < 2) return;

        utils::traits::found_t found_start = 0, found_end = 0;

        do {
            if ((found_start = utils::string::contains(s, str_char, *found_start))) {
                if ((found_end = utils::string::contains(s, str_char, ++(*found_start)))) {
                    v.emplace_back(std::string_view{s.data() + *found_start, *found_end - *found_start});
                    found_start = *found_end + 1u;
                }
            }
        } while (found_start && *found_start < len);
    }

    /**
     *  \brief  Join the strings in \p v with the string \p join_with between them.
     *
     *  \param  v
     *      The list of strings to join.
     *  \param  join_with
     *      The string to join with.
     *  \return Returns one string containing all the strings in \p v appended
     *          to eachother, joined by \p join_with.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static std::string join(const std::vector<std::string>& v, const std::string_view& join_with = ",") {
        std::string joined;
        const auto end = v.end();

        if (auto start = v.begin(); start != end) {
            // Calculate final size beforehand
            size_t size = (*start).size() + v.size() - 1;

            while (++start != end) {
                size += (*start).size();
            }

            joined.reserve(size);

            // Append strings and delimiter
            start = v.begin();
            joined += *start;
            while (++start != end) {
                joined += join_with;
                joined += *start;
            }
        }

        return joined;
    }

    /**
     *  \brief  Join the strings in \p v with the char \p join_with between them.
     *
     *  \param  v
     *      The list of strings to join.
     *  \param  join_with
     *      The char to join with.
     *  \return Returns one string containing all the strings in \p v appended
     *          to eachother, joined by \p join_with.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::string join(const std::vector<std::string>& v, const char join_with) {
        return utils::string::join(v, std::string_view{&join_with, 1});
    }

    /**
     *  \brief  Join the chars in \p v with the char \p join_with between them.
     *
     *  \param  v
     *      The list of chars to join (a compatible char type).
     *  \param  join_with
     *      The char to join with. If \p join_with == '\0', just concatenate all chars.
     *  \return Returns one string containing all the chars in \p v appended
     *          to eachother, joined by \p join_with.
     */
    template<
        class CharT,
        typename std::enable_if_t<   std::is_same<CharT, char>::value
                                  || std::is_same<CharT, int8_t>::value
                                  || std::is_same<CharT, uint8_t>::value
                                  || std::is_same<CharT, wchar_t>::value
                                  || std::is_same<CharT, char16_t>::value
                                  || std::is_same<CharT, char32_t>::value,
                                     int> = 0
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static std::string join(const std::vector<CharT>& v, const char join_with = '\0') {
        std::string joined;
        const auto end = v.end();

        if (auto start = v.begin(); start != end) {
            if (join_with == '\0') {
                joined.reserve(v.size());
                joined.assign(v.begin(), v.end());
            } else {
                joined.reserve(v.size() * 2 - 1);
                joined.push_back(*start);

                while(++start != end) {
                    joined.push_back(join_with);
                    joined.push_back(*start);
                }
            }
        }

        return joined;
    }

    /**
     *  \brief  Split the given string \p s into parts delimited by \p delim.
     *
     *  \param  v
     *      The vector to add the splitted strings to.
     *  \param  s
     *      The string to split.
     *  \param  delim
     *      The character delimiter.
     *  \param  max_splits
     *      The maximum amount of splits to make.
     *      `-1`    split on all delimiters
     *      `0`     don't split: vector will contain the original string \p s
     *      `1`     vector has the first splitted element,
     *              and the rest of the string as second element
     *  \return Returns a list of seperate strings that were delimited by \p delim.
     */
    ATTR_MAYBE_UNUSED
    static void split(std::vector<std::string_view> &v, const std::string_view& s, const char delim = ',', int max_splits = -1) {
        utils::traits::found_t found = 0ull;
        size_t prev_end = 0ull;
        v.clear();

        while (max_splits != 0 && (found = utils::string::contains(s, delim, prev_end))) {
            const size_t size = *found - prev_end;
            v.emplace_back(std::string_view{s.data() + prev_end, size});
            max_splits--;
            prev_end += size + 1;
        }

        if (prev_end < s.size()) {
            v.emplace_back(std::string_view{s.data() + prev_end});
        }
    }

    /**
     *  \brief  Format the given args into the format string.
     *          If no arguments are given, the given format string
     *          will be returned as-is.
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
    template<typename ... Type> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static std::string format(const std::string_view& format, Type&& ...args) {
        if constexpr (sizeof...(Type) != 0) {
            const size_t size = std::snprintf(nullptr, 0, format.data(), args...) + 1; // Extra space for '\0'
            auto buf = utils::memory::new_unique_array<char>(size);

            std::snprintf(buf.get(), size, format.data(), args...);

            return std::string(buf.get(), buf.get() + size - 1 ); // Strip '\0'
        } else {
            return std::string(format);
        }
    }

    /**
     *  \brief  Static buffer with all valid base64 chars.
     */
    static constexpr std::string_view _base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                      "abcdefghijklmnopqrstuvwxyz"
                                                      "0123456789+/";

    /**
     *  \brief  Check if given char is a valid base64 char (without `=` pad).
     *
     *  \param  c
     *      The character to check.
     *  \return
     *      Returns true if _base64_chars.find(c) != npos
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline bool is_base64(uint8_t c) {
        return (std::isalnum(c) || (c == '+') || (c == '/'));
    }

    /**
     *  \brief  Check if given buffer contains a valid base64
     *          encoded string.
     *
     *  \param  buffer
     *      The buffer to check.
     *  \param  length
     *      The length of the buffer.
     *  \return
     *      Returns true if buffer contains a valid base64 encoded string.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static bool is_base64(const uint8_t *buffer, size_t length) {
        if (length % 4) return false;
        const uint8_t *end = --buffer + length + 1;

        while (++buffer != end) {
            if (*buffer == '=') {
                break;
            } else if (!utils::string::is_base64(*buffer)) {
                return false;
            }
        }

        switch (end - buffer) {
            case 0:
                return true;
            case 1:
                return buffer[0] == '=';
            case 2:
                return buffer[0] == '=' && buffer[1] == '=';
            default:
                return false;
        }
    }

    /**
     *  \brief  Check if given string contains is a valid base64
     *          encoded string.
     *
     *  \param  str
     *      The string to check.
     *  \return
     *      Returns true if string contains a valid base64 encoded string.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline bool is_base64(const std::string_view& str) {
        return utils::string::is_base64(reinterpret_cast<const uint8_t*>(str.data()), str.length());
    }

    /**
     *  \brief  Encode the given buffer with base64.
     *
     *  \param  buffer
     *      The buffer to encode.
     *  \param  length
     *      The length of the buffer to encode.
     *  \return Return an std::string containing the encoded buffer.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static std::string to_base64(const uint8_t *buffer, size_t length) {
        std::string encoded;
        const uint8_t mod = length % 3;
        encoded.reserve(((length / 3) + (mod > 0)) * 4);
        uint32_t temp;

        for (const uint8_t *end = --buffer + length - mod; buffer != end;) {
            temp = (uint32_t(buffer[1]) << 16) | (uint32_t(buffer[2]) << 8) | (buffer[3]);
            buffer += 3;

            encoded.push_back(utils::string::_base64_chars[(temp & 0x00FC0000) >> 18]);
            encoded.push_back(utils::string::_base64_chars[(temp & 0x0003F000) >> 12]);
            encoded.push_back(utils::string::_base64_chars[(temp & 0x00000FC0) >> 6]);
            encoded.push_back(utils::string::_base64_chars[(temp & 0x0000003F)]);
        }

        switch (mod) {
            case 1:
                temp = (uint32_t(buffer[1]) << 16);

                encoded.push_back(utils::string::_base64_chars[(temp & 0x00FC0000) >> 18]);
                encoded.push_back(utils::string::_base64_chars[(temp & 0x0003F000) >> 12]);
                encoded.push_back('=');
                encoded.push_back('=');
                break;
            case 2:
                temp = (uint32_t(buffer[1]) << 16) | (uint32_t(buffer[2]) << 8);

                encoded.push_back(utils::string::_base64_chars[(temp & 0x00FC0000) >> 18]);
                encoded.push_back(utils::string::_base64_chars[(temp & 0x0003F000) >> 12]);
                encoded.push_back(utils::string::_base64_chars[(temp & 0x00000FC0) >> 6]);
                encoded.push_back('=');
                break;
        }

        return encoded;
    }

    /**
     *  \brief  Encode the given string with base64.
     *
     *  \param  str
     *      The string to encode.
     *  \return Return an std::string containing the encoded string.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::string to_base64(const std::string_view& str) {
        return utils::string::to_base64(reinterpret_cast<const uint8_t*>(str.data()), str.length());
    }

    /**
     *  \brief  Decode the given buffer with base64.
     *
     *  \param  buffer
     *      The buffer to decode.
     *  \param  length
     *      The length of the buffer to decode.
     *  \return Return an std::string containing the decoded buffer.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static std::string from_base64(const uint8_t *buffer, size_t length) {
        if (length % 4) {
            throw utils::exceptions::ConversionException("utils::string::base64_decode (invalid size)");
        }

        const uint8_t *const buffer_end = buffer + length;
        size_t decoded_length = (length / 4) * 3;

        if (length) {
            decoded_length -= (*(buffer_end - 2) == '=') + (*(buffer_end - 1) == '=');
        }

        std::string decoded;
        decoded.resize(decoded_length);
        char *iter = decoded.data() - 1;

        while (buffer < buffer_end) {
            int32_t temp = 0;

            for (uint8_t i = 0; i < 4; ++i, ++buffer) {
                temp <<= 6;
                if (*buffer >= 'A' && *buffer <= 'Z') {
                    temp |= *buffer - 'A';
                } else if (*buffer >= 'a' && *buffer <= 'z') {
                    temp |= *buffer - 'a' + 26;
                } else if (*buffer >= '0' && *buffer <= '9') {
                    temp |= *buffer - '0' + 2 * 26;
                } else if (*buffer == '+') {
                    temp |= 2 * 26 + 10;
                } else if (*buffer == '/') {
                    temp |= 2 * 26 + 10 + 1;
                } else if (*buffer == '=') {
                    switch (buffer_end - buffer) {
                        case 1:
                            *++iter = char((temp >> 16) & 0xFF);
                            *++iter = char((temp >>  8) & 0xFF);
                            return decoded;
                        case 2:
                            *++iter = char((temp >> 10) & 0xFF);
                            return decoded;
                        default:
                            throw utils::exceptions::ConversionException("utils::string::base64_decode (invalid padding)");
                    }
                } else {
                    throw utils::exceptions::ConversionException("utils::string::base64_decode (invalid character)");
                }
            }

            *++iter = char((temp >> 16) & 0xFF);
            *++iter = char((temp >>  8) & 0xFF);
            *++iter = char((temp      ) & 0xFF);
        }

        return decoded;
    }

    /**
     *  \brief  Decode the given string with base64.
     *
     *  \param  str
     *      The string to decode.
     *  \return Return an std::string containing the decoded string.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline std::string from_base64(const std::string_view& str) {
        return utils::string::from_base64(reinterpret_cast<const uint8_t*>(str.data()), str.length());
    }
}

#endif // UTILS_STRING_HPP
