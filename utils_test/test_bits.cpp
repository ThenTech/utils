#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/utils_catch.hpp"

#include "../utils_lib/utils_bits.hpp"


TEST_CASE("Test utils::bits::ffs", "[utils][utils::bits]" ) {
    REQUIRE(utils::bits::ffs<int>(-1)     == 1);
    REQUIRE(utils::bits::ffs<int>(1)      == 1);
    REQUIRE(utils::bits::ffs<int>(0)      == 0);
    REQUIRE(utils::bits::ffs<int>(0x8001) == 1);

    for (uint64_t i = 65, mask = 1ull << 63; i--; mask >>= 1) {
        CHECK(utils::bits::ffs(mask) == i);
    }
}

TEST_CASE("Test utils::bits::msb", "[utils][utils::bits]" ) {
    REQUIRE(utils::bits::msb<int>(1)      == 1);
    REQUIRE(utils::bits::msb<int>(-1)     == utils::bits::size_of<int>());
    REQUIRE(utils::bits::msb<int>(0)      == 0);
    REQUIRE(utils::bits::msb<int>(0x8001) == 16);

    for (uint64_t i = 65, mask = 1ull << 63; i--; mask >>= 1) {
        CHECK(utils::bits::msb(mask) == i);
    }
}

TEST_CASE("Test utils::bits::is_power_of_2", "[utils][utils::bits]" ) {
    REQUIRE(utils::bits::is_power_of_2<int>(1)      == true);
    REQUIRE(utils::bits::is_power_of_2<int>(-1)     == false);
    REQUIRE(utils::bits::is_power_of_2<int>(0)      == false);
    REQUIRE(utils::bits::is_power_of_2<int>(0x8000) == true);

    for (uint64_t i = 65, mask = 1ull << 63; --i; mask >>= 1) {
        CHECK(utils::bits::is_power_of_2(mask));
    }
}

TEST_CASE("Test utils::bits::round_to_multiple", "[utils][utils::bits]" ) {
    REQUIRE(utils::bits::round_to_multiple(-1, 10)     == 0);
    REQUIRE(utils::bits::round_to_multiple(0 , 10)     == 0);
    REQUIRE(utils::bits::round_to_multiple(0x8000, 16) == 0x8000);

    REQUIRE_FUNCTION_ABORTS_FALSE(utils::bits::round_to_multiple, 1, 1);
    REQUIRE_FUNCTION_ABORTS(utils::bits::round_to_multiple, 1, 0);

//    REQUIRE_FALSE(Function_Aborts(utils::bits::round_to_multiple, 1, 1));
//    REQUIRE(Function_Aborts(utils::bits::round_to_multiple, 1, 0));

    for (int64_t i = 1; i < 10; i++) {
        CHECK(utils::bits::round_to_multiple(i, 10) == 10);
        CHECK(utils::bits::round_to_multiple(-i, 10) == 0);
    }

    CHECK(utils::bits::round_to_multiple(10, 10) == 10);
    CHECK(utils::bits::round_to_multiple(-10, 10) == -10);
}

TEST_CASE("Test utils::bits::round_to_byte", "[utils][utils::bits]" ) {
    REQUIRE(utils::bits::round_to_byte(    0) == 0);
    REQUIRE(utils::bits::round_to_byte(  4*8) == 4);
    REQUIRE(utils::bits::round_to_byte(  255) == 32);
    REQUIRE(utils::bits::round_to_byte(255*8) == 255);

    for (size_t i = 1; i <= 8; i++) {
        CHECK(utils::bits::round_to_multiple(int64_t(i), 8) == 8);
        CHECK(utils::bits::round_to_byte(i)                 == 1);
    }
}

TEST_CASE("Test utils::bits::shift_signed", "[utils][utils::bits]" ) {
    REQUIRE(utils::bits::shift_signed<int8_t>(0, 0) == 0);
    REQUIRE(utils::bits::shift_signed<int8_t>(0, 8) == 0);
    REQUIRE(utils::bits::shift_signed<int8_t>((-1) & 0xFF, 8) == -1);
    REQUIRE(utils::bits::shift_signed<int8_t>((-1) & 0xFF, 8) == -1);

    REQUIRE(utils::bits::shift_signed<uint8_t>(0, 0) == 0);
    REQUIRE(utils::bits::shift_signed<uint8_t>(0, 8) == 0);
    REQUIRE(utils::bits::shift_signed<uint8_t>((-1) & 0xFF, 8) == 0xFF);
    REQUIRE(utils::bits::shift_signed<uint8_t>((-1) & 0xFF, 8) == 0xFF);

    for (size_t i = 1, mask = 1; i <= 8; i++, mask = (mask << 1) | 1) {
        CHECK(utils::bits::shift_signed<int8_t>((-1) & 0xFF, i) == -1);
        CHECK(utils::bits::shift_signed<int8_t>(size_t(-1) & mask, i) == -1);
        CHECK(utils::bits::shift_signed<int8_t>(0, i) == 0);
        CHECK(utils::bits::shift_signed<int8_t>(1, i) == (i > 1 ? 1 : -1));
        CHECK(utils::bits::shift_signed<int16_t>(1, i + 1) == 1);

        CHECK(utils::bits::shift_signed<uint8_t>((-1) & 0xFF, i) == mask);
        CHECK(utils::bits::shift_signed<uint8_t>(size_t(-1) & mask, i) == mask);
        CHECK(utils::bits::shift_signed<uint8_t>(0, i) == 0);
        CHECK(utils::bits::shift_signed<uint8_t>(1, i) == 1);
        CHECK(utils::bits::shift_signed<uint16_t>(1, i + 1) == 1);
    }

    CHECK(utils::bits::shift_signed<int16_t>(0b1000,  4) == -8);
    CHECK(utils::bits::shift_signed<int16_t>(0b1000,  5) ==  8);
    CHECK(utils::bits::shift_signed<int16_t>(0xFFFF, 16) == -1);
    CHECK(utils::bits::shift_signed<int16_t>(0x7FFF, 16) == 32767);
    CHECK(utils::bits::shift_signed<int16_t>(0x7FFF, 15) == -1);
    CHECK(utils::bits::shift_signed<int16_t>(0xDEAD, 16) == -8531);

    CHECK(utils::bits::shift_signed<uint16_t>(0b1000,  4) == 0b1000);
    CHECK(utils::bits::shift_signed<uint16_t>(0b1000,  5) == 0b1000);
    CHECK(utils::bits::shift_signed<uint16_t>(0xFFFF, 16) == 0xFFFF);
    CHECK(utils::bits::shift_signed<uint16_t>(0x7FFF, 16) == 0x7FFF);
    CHECK(utils::bits::shift_signed<uint16_t>(0x7FFF, 15) == 0x7FFF);
    CHECK(utils::bits::shift_signed<uint16_t>(0xDEAD, 16) == 0xDEAD);

    if (utils::Catch::Function_Aborts(utils::bits::shift_signed<int16_t>, 0, 17)) {
        CHECK_FUNCTION_ABORTS_FALSE(utils::bits::shift_signed<int8_t>, 0, 8);
        CHECK_FUNCTION_ABORTS      (utils::bits::shift_signed<int8_t>, 0, 9);
        CHECK_FUNCTION_ABORTS_FALSE(utils::bits::shift_signed<int32_t>, 0, 32);
        CHECK_FUNCTION_ABORTS      (utils::bits::shift_signed<int32_t>, 0, 33);

        CHECK_FUNCTION_ABORTS_FALSE(utils::bits::shift_signed<uint8_t>, 0, 8);
        CHECK_FUNCTION_ABORTS      (utils::bits::shift_signed<uint8_t>, 0, 9);
        CHECK_FUNCTION_ABORTS_FALSE(utils::bits::shift_signed<uint32_t>, 0, 32);
        CHECK_FUNCTION_ABORTS      (utils::bits::shift_signed<uint32_t>, 0, 33);
    } else {
        CHECK(utils::bits::shift_signed< int16_t>(0xBEEF, 17) ==  int16_t(0xBEEF));
        CHECK(utils::bits::shift_signed<uint16_t>(0xBEEF, 17) == uint16_t(0xBEEF));
    }
}

TEST_CASE("Test utils::bits::extend_sign", "[utils][utils::bits]" ) {
    REQUIRE(utils::bits::extend_sign<int8_t, 1>(0) == 0);
    REQUIRE(utils::bits::extend_sign<int8_t, 8>(0) == 0);
    REQUIRE(utils::bits::extend_sign<int8_t, 8>((-1) & 0xFF) == -1);
    REQUIRE(utils::bits::extend_sign<int8_t, 8>((-1) & 0xFF) == -1);

    CHECK(utils::bits::extend_sign<int16_t,  4>(0b1000) == -8);
    CHECK(utils::bits::extend_sign<int16_t,  5>(0b1000) ==  8);
    CHECK(utils::bits::extend_sign<int16_t, 16>(0xFFFF) == -1);
    CHECK(utils::bits::extend_sign<int16_t, 16>(0x7FFF) == 32767);
    CHECK(utils::bits::extend_sign<int16_t, 15>(0x7FFF) == -1);
    CHECK(utils::bits::extend_sign<int16_t, 16>(0xDEAD) == -8531);
}

TEST_CASE("Test utils::bits::bits_needed", "[utils][utils::bits]" ) {
    REQUIRE(utils::bits::bits_needed<uint8_t >(0) == 1);
    REQUIRE(utils::bits::bits_needed<uint8_t >(1) == 1);
    REQUIRE(utils::bits::bits_needed<uint8_t >(0xFF) == 8);
    REQUIRE(utils::bits::bits_needed<uint16_t>( 8) == 4);
    REQUIRE(utils::bits::bits_needed< int16_t>( 8) == 5);
    REQUIRE(utils::bits::bits_needed<uint16_t>(uint16_t(-8)) == 16);
    REQUIRE(utils::bits::bits_needed< int16_t>(-8) == 4);

    for (uint64_t i = 65, mask = 1ull << 63; --i; mask >>= 1) {
        CHECK(utils::bits::bits_needed<uint64_t>(mask) == i);
        CHECK(utils::bits::bits_needed<int64_t>(int64_t(mask)) == (i < 63 ? i + 1 : 64));
    }
}

#endif
