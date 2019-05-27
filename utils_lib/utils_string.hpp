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
    static inline void strToLower(std::string &str) {
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
    [[maybe_unused]]
    static void strExtractQuotedStrings(std::vector<std::string> &v, const std::string& s, const char str_char='\'') {
        const size_t len = s.length() - 1u;
        v.clear();

        if (len < 2) return;

        size_t start = 0, end;

        do {
            if (start = s.find(str_char, start);
                start != std::string::npos)
            {
                if (end = s.find(str_char, ++start);
                    end != std::string::npos)
                {
                    v.emplace_back(s.substr(start, end-start));
                    start = end + 1u;
                }
            }
        } while (start != std::string::npos && start < len);
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
    [[maybe_unused]]
    static std::string strJoin(const std::vector<std::string> &v, const std::string& join_with=",") {
        std::string joined;
        const auto end = v.end();

        if (auto start = v.begin(); start != end) {
            joined += *start;

            while(++start != end) {
                joined += join_with + *start;
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
    [[maybe_unused]]
    static inline std::string strJoin(const std::vector<std::string> &v, const char join_with) {
        return utils::string::strJoin(v, std::string(1, join_with));
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
    > [[maybe_unused]]
    static std::string strJoin(const std::vector<CharT> &v, const char join_with='\0') {
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
     *  \return Returns a list of seperate strings that were delimited by \p delim.
     */
    [[maybe_unused]]
    static void strSplit(std::vector<std::string> &v, const std::string &s, const char delim = ',') {
        std::stringstream ss(s);
        v.clear();

        for(std::string item; std::getline(ss, item, delim); ) {
            v.emplace_back(item);
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
    template<typename ... Type> [[maybe_unused]]
    static std::string format(const std::string& format, Type&& ...args) {
        if constexpr(sizeof...(Type) != 0) {
            const size_t size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1; // Extra space for '\0'
            auto buf = utils::memory::new_unique_array<char>(size);

            std::snprintf(buf.get(), size, format.c_str(), args...);

            return std::string(buf.get(), buf.get() + size - 1 ); // Strip '\0'
        } else {
            return format;
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
    [[maybe_unused]]
    static inline bool is_base64(uint8_t c) {
        return (std::isalnum(c) || (c == '+') || (c == '/'));
    }

    /**
     *  \brief  Check if given buffer contains a valid base64
     *          encoded string.
     *
     *  \param  buffer
     *      The buffer to check.
     *  \param  buffer
     *      The length of the buffer.
     *  \return
     *      Returns true if buffer contains a valid base64 encoded string.
     */
    [[maybe_unused]]
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
    [[maybe_unused]]
    static inline bool is_base64(const std::string& str) {
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
    [[maybe_unused]]
    static std::string base64_encode(const uint8_t *buffer, size_t length) {
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
    [[maybe_unused]]
    static inline std::string base64_encode(const std::string& str) {
        return utils::string::base64_encode(reinterpret_cast<const uint8_t*>(str.data()), str.length());
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
    [[maybe_unused]]
    static std::string base64_decode(const uint8_t *buffer, size_t length) {
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
    [[maybe_unused]]
    static inline std::string base64_decode(const std::string& str) {
        return utils::string::base64_decode(reinterpret_cast<const uint8_t*>(str.data()), str.length());
    }
}

#endif // UTILS_STRING_HPP
