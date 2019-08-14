#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/utils_catch.hpp"

#include "../utils_lib/utils_crc.hpp"


TEST_CASE("Test utils::crc::Calculate", "[utils][utils::crc]" ) {
    constexpr uint32_t crc_result = 2496742018;

    const std::string dd = "1AF20BC6-8BAB-4B38-8DAA-11FB029C3668";
    uint32_t crc;

    crc = utils::CRC::Calculate(dd.data(), dd.size(), utils::CRC::CRC_32());
    REQUIRE(crc == crc_result);

    crc = utils::CRC::Calculate(dd.data(), 10, utils::CRC::CRC_32());
    crc = utils::CRC::Calculate(dd.data() + 10, dd.size() - 10, utils::CRC::CRC_32(), crc);
    REQUIRE(crc == crc_result);

    static const utils::CRC::Table<uint32_t, 32> table(utils::CRC::CRC_32());
    crc = utils::CRC::Calculate(dd.data(), dd.size(), table);
    REQUIRE(crc == crc_result);
}

#endif
