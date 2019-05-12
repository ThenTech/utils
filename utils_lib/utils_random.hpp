#ifndef UTILS_RANDOM_HPP
#define UTILS_RANDOM_HPP

#include "external/random.hpp"
#include "utils_catch.hpp"

/*
 *  NOTE: Does adding inline to static methods increase performance?
 *

#ifdef _MSC_VER
    #define RND_INLINE __forceinline
#else
    #define RND_INLINE inline
#endif

 */

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
     *          Items are chosen at random.
     *
     *  \param  amount
     *      The amount of items to pick out.
     *  \param  container
     *      The container to pick items from.
     *      This class must have begin and end iterators, as well as
     *      the reserve and push_back methods implemented.
     *  \return Returns a Container instance with the picked items.
     */
    template<typename Container> [[maybe_unused]]
    static auto pick_x_from(size_t amount, const Container& container) ->
        typename std::enable_if<effolkronium::details::is_iterator<
                    decltype(std::begin(container))>::value
                  , Container
        >::type
    {
        Container picked;
        picked.reserve(amount);

        const auto size = std::distance(std::begin(container), std::end(container));
        if( 0 == size ) return picked;

        using diff_t = typename std::iterator_traits<decltype(std::begin(container))>::difference_type;

        for (size_t i = 0; i < amount; ++i) {
            picked.push_back(*std::next(std::begin(container), Random::get<diff_t>( 0, size - 1)));
        }

        return picked;
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
        typename Container = std::vector<T>
    > [[maybe_unused]]
    static typename std::enable_if<
        (   effolkronium::details::is_supported_number<T>::value
         || effolkronium::details::is_supported_character<T>::value
         || std::is_same<T, bool>::value
        ), Container>::type
    generate_x(
        const size_t amount,
        T from = std::numeric_limits<T>::min(),
        T to   = std::numeric_limits<T>::max())
    {
        Container picked;
        picked.reserve(amount);

        if (to < from)  // Allow range from higher to lower
            std::swap(from, to);

        if constexpr (effolkronium::details::is_uniform_real<T>::value) {
            auto dist = Random::real_dist_t<T> {
                static_cast<T>(from),
                static_cast<T>(to)
            };

            for (size_t i = 0; i < amount; ++i) {
                picked.push_back(static_cast<T>(dist(Random::engine())));
            }
        } else if constexpr (std::is_same<T, bool>::value) {
            auto dist = Random::bool_dist_t{ 0.5 };

            for (size_t i = 0; i < amount; ++i) {
                picked.push_back(static_cast<T>(dist(Random::engine())));
            }
        } else {
            auto dist = Random::integer_dist_t<T> {
                static_cast<T>(from),
                static_cast<T>(to)
            };

            for (size_t i = 0; i < amount; ++i) {
                picked.push_back(static_cast<T>(dist(Random::engine())));
            }
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
        typename Container = typename std::enable_if<effolkronium::details::is_supported_character<T>::value, std::basic_string<T>>::type
    > [[maybe_unused]]
    static inline Container generate_string(
            const size_t amount,
            const T from = std::numeric_limits<T>::min(),
            const T to   = std::numeric_limits<T>::max())
    {
        return utils::random::generate_x<T, Container>(amount, from, to);
    };

    /**
     *  \brief  Generate a random vector with length \p amount and containing
     *          booleans with the probability of them being true given by
     *          \p probability.
     *          Every char-type is supported and will create a `std::basic_string<T>`
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
        typename Container = typename std::enable_if<std::is_same<T, bool>::value, std::vector<T>>::type
    > [[maybe_unused]]
    static inline Container generate_bool(
            const size_t amount,
            const double probability = 0.5)
    {
        ASSERT(0 <= probability && 1 >= probability);

        Container picked;
        picked.reserve(amount);

        auto dist = Random::bool_dist_t{ probability };

        for (size_t i = 0; i < amount; ++i) {
            picked.push_back(static_cast<T>(dist(Random::engine())));
        }

        return picked;
    };
}


#endif // UTILS_RANDOM_HPP
