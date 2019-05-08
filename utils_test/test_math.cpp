#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/catch.hpp"

#include "../utils_lib/utils_math.hpp"


TEST_CASE("Test utils::math::sign", "[utils][utils::math]" ) {
    REQUIRE(utils::math::sign(-1) == -1);
    REQUIRE(utils::math::sign( 1) ==  1);
    REQUIRE(utils::math::sign( 0) ==  0);
    REQUIRE(utils::math::sign(int16_t(0x8001)) == -1);
}

#endif
