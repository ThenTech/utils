#ifndef UTILS_MISC_HPP
#define UTILS_MISC_HPP

#include "utils_exceptions.hpp"
#include "utils_memory.hpp"
#include "utils_print.hpp"
#include "utils_traits.hpp"

#include <iomanip>


namespace utils::misc {
    /** \brief
     *      Convert the given char* to a variable of type T.
     *      Use this method instead of the raw C functions: atoi, atof, atol, atoll.
     *
     *      Also check for hex number.
     *
     *  \tparam T
     *      The type of object to cast to.
     *  \param  buffer
     *      The character buffer to convert.
     *  \return
     *      Returns a variable of type T with the value as given in buffer.
     */
    template <class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static T lexical_cast(const std::string_view& buffer) {
        T out;
        std::stringstream cast;

        if (buffer[0] == '0' && buffer[1] != '.') {
            if (buffer[1] == 'x' || buffer[1] == 'X') {
                // Hex literal
                cast << std::hex << buffer;
            } else if (buffer[1] == 'b' || buffer[1] == 'B') {
                // Binary literal
                try {
                    cast << std::stoll(std::string(buffer.substr(2)), nullptr, 2);
                } catch (std::invalid_argument const&) {
                    throw utils::exceptions::CastingException(std::string(buffer), utils::print::type2name(out));
                }
            } else {
                // Octal literal
                cast << std::oct << buffer;
            }
        } else if (buffer[0] == '#') {
            // Hex colour string
            cast << std::hex << buffer.substr(1);
        } else {
            // Decimal/float/...
            cast << buffer;
        }

        if (!(cast >> out))
            throw utils::exceptions::CastingException(std::string(buffer), utils::print::type2name(out));

        return out;
    }
}

#endif // UTILS_MISC_HPP
