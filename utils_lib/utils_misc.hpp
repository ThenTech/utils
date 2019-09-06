#ifndef UTILS_MISC_HPP
#define UTILS_MISC_HPP

#include "utils_compiler.hpp"
#include "utils_traits.hpp"
#include "utils_exceptions.hpp"
#include "utils_print.hpp"

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

        if (HEDLEY_UNLIKELY(buffer.size() == 0))
            throw utils::exceptions::CastingException(std::string(buffer), utils::print::type2name(out));

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

    template <class T> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static std::optional<T> try_lexical_cast(const std::string_view& buffer, utils::exceptions::Exception *error = nullptr) {
        std::optional<T> value;

        try {
            value = utils::misc::lexical_cast<T>(buffer);
        } catch (const utils::exceptions::CastingException& e) {
            if (error) *error = e;
            value = std::nullopt;
        }

        return value;
    }

    template<
        typename F,
        typename = typename std::enable_if_t<utils::traits::is_invocable_v<F>>
    > class Scoped {
        private:
            F callback;
        public:
            Scoped(F&& callback) : callback(std::move(callback)) {}
            ~Scoped() { std::invoke<F>(std::forward<F>(this->callback)); }
    };

    template<typename F> ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline auto MakeScoped(F&& callback) {
        return Scoped<F>(std::forward<F>(callback));
    }

    #define UTILS_MISC_MAKE_SCOPED(callback) \
        auto HEDLEY_CONCAT(scope_exit_, __LINE__) = utils::misc::MakeScoped(callback);
}

#endif // UTILS_MISC_HPP
