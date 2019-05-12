#ifndef UTILS_MISC_HPP
#define UTILS_MISC_HPP

#include "utils_exceptions.hpp"
#include "utils_memory.hpp"

#include <iomanip>

#ifdef THREADING_ENABLED
    #include <mutex>
    #define LOCK_BLOCK(MTX) std::unique_lock<std::mutex> lock_##MTX(MTX)
#endif

namespace utils::misc {
    /**
     *  \brief   Cast enum type to underlining data type.
     *  \param   e
     *      The enum value to cast.
     */
    template <typename E>
    constexpr inline auto to_underlying(E e) noexcept {
        return static_cast<std::underlying_type_t<E>>(e);
    }

    /**	\brief
     *		Convert the given char* to a variable of type T.
     *		Use this method instead of the raw C functions: atoi, atof, atol, atoll.
     *
     *      Also check for hex number.
     *
     *	\tparam	T
     *		The type of object to cast to.
     *	\param	buffer
     *		The character buffer to convert.
     *	\return
     *		Returns a variable of type T with the value as given in buffer.
     */
    template <class T> [[maybe_unused]]
    static T lexical_cast(const char* buffer) {
        T out;
        std::stringstream cast;

        if (buffer[0] == '0' && buffer[1] != '.'){
            if (buffer[1] == 'x' || buffer[1] == 'X') {
                // Hex literal
                cast << std::hex << buffer;
            } else if (buffer[1] == 'b' || buffer[1] == 'B') {
                // Binary literal
                try {
                    cast << std::stoll(buffer + 2, nullptr, 2);
                } catch(std::invalid_argument const&) {
                    throw utils::exceptions::CastingException(buffer, type2name(out));
                }
            } else {
                // Octal literal
                cast << std::oct << buffer;
            }
        } else {
            // Decimal/float/...
            cast << buffer;
        }

        if (!(cast >> out))
            throw utils::exceptions::CastingException(buffer, type2name(out));

        return out;
    }
}

#endif // UTILS_MISC_HPP
