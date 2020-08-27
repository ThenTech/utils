#ifndef UTILS_CRC_HPP
#define UTILS_CRC_HPP

#include "utils_compiler.hpp"

// Ignore warnings
HEDLEY_DIAGNOSTIC_PUSH
HEDLEY_DIAGNOSTIC_DISABLE_UNKNOWN_PRAGMAS
#if HEDLEY_MSVC_VERSION_CHECK(15,0,0)
    // TODO Ignore MSVC warnings
    //#pragma warning(disable:xxxx)
#else
    #pragma GCC diagnostic ignored "-Wunknown-pragmas"
#endif

#define CRCPP_BRANCHLESS
#define CRCPP_USE_CPP11
#define CRCPP_USE_NAMESPACE
#include "external/crc/inc/CRC.h"

HEDLEY_DIAGNOSTIC_POP

namespace utils {
    /*
     *  Reference: https://github.com/d-bahr/CRCpp (2020-08-18)
     */
    using CRCPP::CRC;
}

#undef CRCPP_BRANCHLESS
#undef CRCPP_USE_CPP11
#undef CRCPP_USE_NAMESPACE
#endif // UTILS_CRC_HPP
