#ifndef UTILS_RANDOM_HPP
#define UTILS_RANDOM_HPP

#include "external/random.hpp"

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
}


#endif // UTILS_RANDOM_HPP
