#ifndef UTILS_MISC_HPP
#define UTILS_MISC_HPP

#include <type_traits>
#include <cmath>

#ifdef _MSC_VER
    #include <intrin.h>
    #include <typeinfo>
#else
    #include <cxxabi.h>
#endif

#include "utils_exceptions.hpp"
#include "utils_string.hpp"
#include "utils_memory.hpp"

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

    /**	\brief	Returns the internal actual class name of the given object o.
     *
     *	**Uses __abi::__cxa_demangle__ which is part of <cxxabi.h> included in all GCC compilers.**
     *
     *	If GCC is not used, type2name will revert to typeid(o).name() instead.
     *
     *	\tparam	T
     *		The type of object to get the name demangled from.
     *	\param	o
     *		The object to demangle the name from.
     *	\return
     *		Returns the class name of o.
     */
    template <
        class T, typename ... Strings,
        std::enable_if_t<(std::is_convertible_v<Strings const&, std::string_view> && ...), int> = 0
    >
    [[maybe_unused]]
    static const std::string type2name(T const& o, Strings&& ...filter) {
        #ifdef _CXXABI_H
            // Valgrind warning: abi::__cxa_demangle uses malloc so needs free
            utils::memory::unique_t<char, decltype (&std::free)>
                    demang(abi::__cxa_demangle(typeid(o).name(), nullptr, nullptr, nullptr),
                           &std::free);
            std::string s(demang.get());
        #else
            std::string s(typeid(o).name());
        #endif

        // Remove every occurence in filter... list from output (e.g. std::out)
        if constexpr (sizeof...(filter) > 0) {
            (utils::string::strReplaceAll(s, filter, ""), ...);
        }

        return s;
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

    /**	\brief	Check if the given doubles are equal.
     *
     *	\param	x
     *		The first value to compare.
     *	\param	y
     *		The second value to compare.
     *	\param	epsilon
     *		The precision to compare with (standard deviation of 1e-4 or 0.0001).
     *
     *	\return	bool
     *		Returns whether x equals y within the given epsilon precision.
     */
    [[maybe_unused]]
    static inline bool epsilon_equals(double x, double y, double epsilon = 1e-4) {
        return std::abs(x - y) < epsilon;
    }
}

#endif // UTILS_MISC_HPP
