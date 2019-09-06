#ifndef UTILS_HTTP_HPP
#define UTILS_HTTP_HPP

#include "utils_compiler.hpp"

// Ignore warnings
HEDLEY_DIAGNOSTIC_PUSH
#if HEDLEY_MSVC_VERSION_CHECK(15,0,0)
    // TODO Ignore MSVC warnings
    #pragma warning(disable:xxxx)
#else
    #pragma GCC diagnostic ignored "-Wcpp"
#endif

#include "external/HTTPRequest/include/HTTPRequest.hpp"

HEDLEY_DIAGNOSTIC_POP

namespace utils {
    /*
     *  Reference: https://github.com/elnormous/HTTPRequest (2019-08-16)
     */
//    using namespace http;
    namespace http = ::http;
}

#endif // UTILS_HTTP_HPP
