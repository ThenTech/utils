#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/utils_catch.hpp"

#include "../utils_lib/utils_misc.hpp"


TEST_CASE("Test utils::misc::lexical_cast", "[utils][utils::misc]") {
    SECTION("Normal") {
        CHECK_THROWS_AS(utils::misc::lexical_cast<int>(""), utils::exceptions::CastingException);
        CHECK(utils::misc::lexical_cast<int>   ("123") == 123);
        CHECK(utils::misc::lexical_cast<float> ("1.3") == Approx(1.3));
        CHECK(utils::misc::lexical_cast<double>("1.3") == Approx(1.3));
        CHECK(utils::misc::lexical_cast<int>   ("1a")  == 1);

        // Defined behaviour, but probalby unintended
        CHECK(utils::misc::lexical_cast<char>("256")      == '2');
        CHECK(utils::misc::lexical_cast<uint8_t>("0xFFF") == '0');

        const uint64_t too_large = std::numeric_limits<int>::max();
        CHECK(utils::misc::lexical_cast<int>(std::to_string(too_large)) == too_large);
        CHECK_THROWS_AS(utils::misc::lexical_cast<int>(std::to_string(too_large + 1)), utils::exceptions::CastingException);
    }

    SECTION("Hexadecimal") {
        CHECK(utils::misc::lexical_cast<int>     ("0xAA")       == 0xAA);
        CHECK(utils::misc::lexical_cast<uint32_t>("0XDEADBEEF") == 0xDEADBEEF);
        CHECK(utils::misc::lexical_cast<uint32_t>("#FF55AA")    == 0xFF55AA);
        CHECK(utils::misc::lexical_cast<int>     ("0xAAG")      == 0xAA);
    }

    SECTION("Binary") {
        CHECK(utils::misc::lexical_cast<int>     ("0b10101010") == 0xAA);
        CHECK(utils::misc::lexical_cast<uint32_t>("0B01010101") == 0x55);
        CHECK(utils::misc::lexical_cast<int>     ("0b12")       == 1);
        CHECK_THROWS_AS(utils::misc::lexical_cast<int>("0b2"), utils::exceptions::CastingException);
    }

    SECTION("Octal") {
        CHECK(utils::misc::lexical_cast<int>     ("01234567") == 342391);
        CHECK(utils::misc::lexical_cast<uint32_t>("0100")     == 64);
        CHECK(utils::misc::lexical_cast<int>     ("08")       == 0);
    }

    SECTION("Nothrow") {
        const uint64_t too_large = std::numeric_limits<int>::max();

        CHECK(utils::misc::try_lexical_cast<int>("2").has_value());
        CHECK_FALSE(utils::misc::try_lexical_cast<int>("").has_value());
        CHECK_FALSE(utils::misc::try_lexical_cast<int>(std::to_string(too_large + 1)).has_value());
        CHECK_FALSE(utils::misc::try_lexical_cast<int>("0b2").has_value());
    }
}

#endif
