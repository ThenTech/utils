#ifndef UTILS_RANDOM_HPP
#define UTILS_RANDOM_HPP

#include "external/random.hpp"
#include "utils_compiler.hpp"
#include "utils_traits.hpp"
#include "utils_test.hpp"
#include "utils_string.hpp"

#include <sstream>
#include <iomanip>


namespace utils::random {
    /**
     *  \brief  Get base random alias which is auto seeded and
     *          has static API and internal state.
     */
    using Random = effolkronium::random_static;

    /**
     *  \brief  Get base random alias which is auto seeded and
     *          has local (stack) API and internal state.
     */
    using RandomLocal = effolkronium::random_local;

    /**
     *  \brief  Get base random alias which is auto seeded and
     *          has thread local API and internal state.
     */
    using RandomThreadLocal = effolkronium::random_thread_local;

    /**
     *  \brief  Pick a certain amount of items from the given container,
     *          and copy them to a new Container of the same type.
     *          Items are chosen at random (does not preserve relative order).
     *
     *  \param  amount
     *      The amount of items to pick out.
     *  \param  container
     *      The container to pick items from.
     *      This class must have begin and end iterators, as well as
     *      the resize and push_back methods implemented.
     *  \return Returns a Container instance with the picked items.
     */
    template<
        typename Container,
        typename = typename std::enable_if_t<utils::traits::is_iterable_v<Container>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static auto pick_x_from(size_t amount, const Container& container) {
        Container picked;

        const auto size = std::distance(std::begin(container), std::end(container));
        if(HEDLEY_UNLIKELY(0 == size)) return picked;
        picked.resize(amount);

        using diff_t = typename std::iterator_traits<decltype(std::begin(container))>::difference_type;

        std::generate_n(std::begin(picked), amount,
        [&container, &size]() {
            return *std::next(std::begin(container), Random::get<diff_t>( 0, size - 1 ));
        });

        return picked;
    }

    /**
     *  \brief  Sample a certain amount of items from the given container,
     *          and copy them to a new Container of the same type.
     *          Items are chosen by sampling (preserves relative order).
     *          The sample can contain at most \p container.size() values.
     *
     *  \param  amount
     *      The amount of items to sample.
     *  \param  container
     *      The container to sample items from.
     *      This class must have begin and end iterators, as well as
     *      the reserve and push_back methods implemented.
     *  \return Returns a Container instance with the sampled items.
     */
    template<
        typename Container,
        typename = typename std::enable_if_t<utils::traits::is_iterable_v<Container>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static auto sample(size_t amount, const Container& container) {
        Container sampled;

        const auto size = std::distance(std::begin(container), std::end(container));
        if(HEDLEY_UNLIKELY(0 == size)) return sampled;
        sampled.reserve(std::min(amount, size_t(size)));

        std::sample(std::begin(container), std::end(container),
                    std::back_inserter(sampled), amount,
                    Random::engine());

        return sampled;
    }

    /**
     *  \brief  Generate a container (default: vector<T>) with \p amount items,
     *          randomly distributed between \p from and \p to.
     *
     *          Only fundamental data types are supported (numbers, chars and bools).
     *          These are randomly chosen between between \p from and \p to,
     *          while booleans are chosen with a 1/2 probability.
     *
     *          To generate booleans with other probabilities, use generate_bool,
     *          to generate a string (instead of vector) when choosing chars,
     *          use generate_string.
     *
     *          For other data types, the function will not be enabled.
     *
     *  \param  amount
     *      The amount of items to generate.
     *  \param  from
     *      The lower bound for the distribution. (default: min T)
     *  \param  to
     *      The upper bound for the destribution. (default: max T)
     *      If upper < lower, these are swapped.
     *  \return Returns a new Container containing \p amount items of type T.
     */
    template<
        typename T,
        typename Container = std::vector<T>,
        typename = typename std::enable_if_t<utils::traits::is_iterable_v<Container>
                                         && (   effolkronium::details::is_supported_number<T>::value
                                             || effolkronium::details::is_supported_character<T>::value
                                             || std::is_same<T, bool>::value
                                            )>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static auto generate_x(const size_t amount,
                           T from = std::numeric_limits<T>::min(),
                           T to   = std::numeric_limits<T>::max())
    {
        Container picked;
        picked.resize(amount);

        if (HEDLEY_UNLIKELY(to < from))  // Allow range from higher to lower
            std::swap(from, to);

        const auto dist_generate = [&picked, &amount](auto& dist){
            std::generate_n(std::begin(picked), amount,
                [&dist, &eng = Random::engine()]() {
                    return static_cast<T>(dist(eng));
            });
        };

        if constexpr (effolkronium::details::is_uniform_real<T>::value) {
            auto dist = Random::real_dist_t<T> {
                static_cast<T>(from),
                static_cast<T>(to)
            };

            dist_generate(dist);
        } else if constexpr (std::is_same<T, bool>::value) {
            auto dist = Random::bool_dist_t{ 0.5 };

            dist_generate(dist);
        } else {
            auto dist = Random::integer_dist_t<int64_t> {
                static_cast<int64_t>(from),
                static_cast<int64_t>(to)
            };

            dist_generate(dist);
        }

        return picked;
    }

    /**
     *  \brief  Generate a random string with length \p amount and chars chosen
     *          between \p from and \p to.
     *          Every char-type is supported and will create a `std::basic_string<T>`
     *          containter.
     *
     *          Only enabled if T is a character type.
     *
     *  \param  amount
     *      The amount of items to generate.
     *  \param  from
     *      The lower bound for the distribution. (default: min T)
     *  \param  to
     *      The upper bound for the destribution. (default: max T)
     *      If upper < lower, these are swapped.
     *  \return Returns a new Container containing \p amount items of type T.
     */
    template<
        typename T = char,
        typename Container = typename std::enable_if_t<effolkronium::details::is_supported_character<T>::value, std::basic_string<T>>,
        typename = typename std::enable_if_t<utils::traits::is_iterable_v<Container>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static inline Container generate_string(const size_t length,
                                            const T from = std::numeric_limits<T>::min(),
                                            const T to   = std::numeric_limits<T>::max())
    {
        return utils::random::generate_x<T, Container>(length, from, to);
    };

    /**
     *  \brief  Generate a random string with length \p amount and chars chosen
     *          in `utils::string::_base64_chars[0:62]` to make the name path safe.
     *          Every char-type is supported and will create a `std::basic_string<T>`
     *          containter.
     *
     *          Only enabled if T is a character type.
     *
     *  \param  amount
     *      The amount of items to generate.
     *  \return Returns a new Container containing \p amount items of type T.
     */
    template<
        typename T = char,
        typename Container = typename std::enable_if_t<effolkronium::details::is_supported_character<T>::value, std::basic_string<T>>,
        typename = typename std::enable_if_t<utils::traits::is_iterable_v<Container>>
    > ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static Container generate_safe_string(const size_t length)
    {
        Container picked;
        picked.resize(length);

        auto dist = Random::integer_dist_t<size_t> {
            0ull, utils::string::_base64_chars.size() - 3ull
        };

        std::generate_n(std::begin(picked), length,
        [&dist]() {
            const char gen = utils::string::_base64_chars[dist(Random::engine())];
            return static_cast<T>(gen);
        });

        return picked;
    };

    /**
     *  \brief  Generate a random vector with length \p amount and containing
     *          booleans with the probability of them being true given by
     *          \p probability.
     *          Every boolean-type is supported and will create a `std::vector<T>`
     *          containter.
     *
     *          Only enabled if T is a boolean.
     *
     *  \param  amount
     *      The amount of items to generate.
     *  \param  probability
     *      The probability foor the boolean distribution,
     *      i.e. the chance of the generated bool being true. (default: 0.5)
     *  \return Returns a new Container containing \p amount items of type T.
     */
    template<
        typename T = bool,
        typename Container = typename std::enable_if<std::is_same<T, bool>::value, std::vector<T>>::type,
        typename = typename std::enable_if_t<utils::traits::is_iterable_v<Container>>
    > ATTR_MAYBE_UNUSED
    static Container generate_bool(const size_t amount,
                                   const double probability = 0.5)
    {
        ASSERT(0 <= probability && 1 >= probability);

        Container picked;
        picked.resize(amount);

        auto dist = Random::bool_dist_t{ probability };

        std::generate_n(std::begin(picked), amount,
        [&dist]() {
            return static_cast<T>(dist(Random::engine()));
        });

        return picked;
    };

    /**
     *  \brief  Generate Universal Unique IDentifier v4 (UUID)
     *          In the form: 550e8400-e29b-41d4-a716-446655440000
     *
     *  \return Returns a formatted string (groups with `-`) with a randon UUID.
     */
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static std::string generate_uuid(void) {
        static auto dist = Random::integer_dist_t<uint16_t> {
            static_cast<uint16_t>(0x00),
            static_cast<uint16_t>(0xFF)
        };

        #define FX      "%02X"
        #define SEP     "-"
        #define GET_HEX (dist(Random::engine()))

        return utils::string::format(
            FX FX FX FX SEP FX FX SEP FX FX SEP FX FX SEP FX FX FX FX FX FX,
            GET_HEX, GET_HEX, GET_HEX, GET_HEX,
            GET_HEX, GET_HEX,
            static_cast<uint8_t>((GET_HEX & 0x0F) | 0x40), GET_HEX,
            static_cast<uint8_t>((GET_HEX & 0x3F) | 0x80), GET_HEX,
            GET_HEX, GET_HEX, GET_HEX, GET_HEX, GET_HEX, GET_HEX
        );
    }
}

#endif // UTILS_RANDOM_HPP
