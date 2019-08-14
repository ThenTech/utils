#ifndef UTILS_CRC_HPP
#define UTILS_CRC_HPP

#define CRCPP_BRANCHLESS
#define CRCPP_USE_CPP11
#define CRCPP_USE_NAMESPACE
#include "external/crc/inc/CRC.h"

namespace utils {
    /*
     *  Reference: https://github.com/d-bahr/CRCpp (2019-08-02)
     */
    using CRCPP::CRC;
}

#undef CRCPP_BRANCHLESS
#undef CRCPP_USE_CPP11
#undef CRCPP_USE_NAMESPACE
#endif // UTILS_CRC_HPP
