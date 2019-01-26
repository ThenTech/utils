#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/catch.hpp"
#include "../utils_lib/external/catch_abort.hpp"

#include "../utils_lib/utils_bits.hpp"


TEST_CASE("Test utils::bits", "utils::bits" ) {
    SECTION("Test utils::bits::ffs") {
        REQUIRE(utils::bits::ffs<int>(-1)     == 1);
        REQUIRE(utils::bits::ffs<int>(1)      == 1);
        REQUIRE(utils::bits::ffs<int>(0)      == 0);
        REQUIRE(utils::bits::ffs<int>(0x8001) == 1);

        for (uint64_t i = 65, mask = 1ull << 63; i--; mask >>= 1) {
            CHECK(utils::bits::ffs(mask) == i);
        }
    }

    SECTION("Test utils::bits::msb") {
        REQUIRE(utils::bits::msb<int>(1)      == 1);
        REQUIRE(utils::bits::msb<int>(-1)     == utils::bits::size_of<int>());
        REQUIRE(utils::bits::msb<int>(0)      == 0);
        REQUIRE(utils::bits::msb<int>(0x8001) == 16);

        for (uint64_t i = 65, mask = 1ull << 63; i--; mask >>= 1) {
            CHECK(utils::bits::msb(mask) == i);
        }
    }

    SECTION("Test utils::bits::is_power_of_2") {
        REQUIRE(utils::bits::is_power_of_2<int>(1)      == true);
        REQUIRE(utils::bits::is_power_of_2<int>(-1)     == false);
        REQUIRE(utils::bits::is_power_of_2<int>(0)      == false);
        REQUIRE(utils::bits::is_power_of_2<int>(0x8000) == true);

        for (uint64_t i = 65, mask = 1ull << 63; --i; mask >>= 1) {
            CHECK(utils::bits::is_power_of_2(mask));
        }
    }

    SECTION("Test utils::bits::round_to_multiple") {
        REQUIRE(utils::bits::round_to_multiple(-1, 10)     == 0);
        REQUIRE(utils::bits::round_to_multiple(0 , 10)     == 0);
        REQUIRE(utils::bits::round_to_multiple(0x8000, 16) == 0x8000);

        REQUIRE_FALSE(Function_Aborts(utils::bits::round_to_multiple, 1, 1));
        REQUIRE(Function_Aborts(utils::bits::round_to_multiple, 1, 0));

        for (int64_t i = 1; i <= 10; i++) {
            CHECK(utils::bits::round_to_multiple(i, 10) == 10);
        }
    }

    SECTION("Test utils::bits::round_to_multiple") {
        REQUIRE(utils::bits::round_to_byte(    0) == 0);
        REQUIRE(utils::bits::round_to_byte(  4*8) == 4);
        REQUIRE(utils::bits::round_to_byte(  255) == 32);
        REQUIRE(utils::bits::round_to_byte(255*8) == 255);

        for (size_t i = 1; i <= 8; i++) {
            CHECK(utils::bits::round_to_multiple(int64_t(i), 8) == 8);
            CHECK(utils::bits::round_to_byte(i)                 == 1);
        }
    }
}

#endif
