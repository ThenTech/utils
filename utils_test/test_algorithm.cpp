#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/catch.hpp"

#include "../utils_lib/utils_algorithm.hpp"

#include "../utils_lib/utils_random.hpp"

TEST_CASE("Test utils::algorithm::all", "[utils][utils::algorithm]") {
    REQUIRE(utils::algorithm::all());

    REQUIRE(utils::algorithm::all(true));
    REQUIRE(utils::algorithm::all(-1));
    REQUIRE(utils::algorithm::all(1));
    REQUIRE(utils::algorithm::all(1.125f));
    REQUIRE(utils::algorithm::all(3.625));

    REQUIRE_FALSE(utils::algorithm::all(false));
    REQUIRE_FALSE(utils::algorithm::all(0));

    REQUIRE_FALSE(utils::algorithm::all( true, true, true, false, true, true,  true, true, true, true));
    REQUIRE_FALSE(utils::algorithm::all( true, true, true, false, true, true,  true, true, true, true));
    REQUIRE_FALSE(utils::algorithm::all(false, true, true,  true, true, true,  true, true, true, true));
    REQUIRE_FALSE(utils::algorithm::all( true, true, true, false, true, true, false, true, true, true));
    REQUIRE_FALSE(utils::algorithm::all( true, true, true, false, true, true,  true, true, true, false));

    REQUIRE(utils::algorithm::all(true, true, true, true, true, true,  true, true, true, true));
}

TEST_CASE("Test utils::algorithm::any", "[utils][utils::algorithm]") {
    REQUIRE_FALSE(utils::algorithm::any());

    REQUIRE(utils::algorithm::any(true));
    REQUIRE(utils::algorithm::any(-1));
    REQUIRE(utils::algorithm::any(1));
    REQUIRE(utils::algorithm::any(1.125f));
    REQUIRE(utils::algorithm::any(3.625));

    REQUIRE_FALSE(utils::algorithm::any(false));
    REQUIRE_FALSE(utils::algorithm::any(0));

    REQUIRE(utils::algorithm::any( true, true, true, false, true, true,  true, true, true, true));
    REQUIRE(utils::algorithm::any( true, true, true, false, true, true,  true, true, true, true));
    REQUIRE(utils::algorithm::any(false, true, true,  true, true, true,  true, true, true, true));
    REQUIRE(utils::algorithm::any( true, true, true, false, true, true, false, true, true, true));
    REQUIRE(utils::algorithm::any( true, true, true, false, true, true,  true, true, true, false));
    REQUIRE(utils::algorithm::any(false, false, false, true, false, false, false, false, false, false));

    REQUIRE_FALSE(utils::algorithm::any(false, false, false, false, false, false, false, false, false, false));
}

TEST_CASE("Test utils::algorithm::none", "[utils][utils::algorithm]") {
    REQUIRE(utils::algorithm::none());

    REQUIRE_FALSE(utils::algorithm::none(true));
    REQUIRE_FALSE(utils::algorithm::none(-1));
    REQUIRE_FALSE(utils::algorithm::none(1));
    REQUIRE_FALSE(utils::algorithm::none(1.125f));
    REQUIRE_FALSE(utils::algorithm::none(3.625));

    REQUIRE(utils::algorithm::none(false));
    REQUIRE(utils::algorithm::none(0));

    REQUIRE_FALSE(utils::algorithm::none( true, true, true, false, true, true,  true, true, true, true));
    REQUIRE_FALSE(utils::algorithm::none( true, true, true, false, true, true,  true, true, true, true));
    REQUIRE_FALSE(utils::algorithm::none(false, true, true,  true, true, true,  true, true, true, true));
    REQUIRE_FALSE(utils::algorithm::none( true, true, true, false, true, true, false, true, true, true));
    REQUIRE_FALSE(utils::algorithm::none( true, true, true, false, true, true,  true, true, true, false));
    REQUIRE_FALSE(utils::algorithm::none(false, false, false, true, false, false, false, false, false, false));

    REQUIRE(utils::algorithm::none(false, false, false, false, false, false, false, false, false, false));
}

TEST_CASE("Test utils::algorithm::all_equal", "[utils][utils::algorithm]") {
    REQUIRE(utils::algorithm::all_equal());

    REQUIRE(utils::algorithm::all_equal(true));
    REQUIRE(utils::algorithm::all_equal(-1));
    REQUIRE(utils::algorithm::all_equal(1));
    REQUIRE(utils::algorithm::all_equal(1.125f));
    REQUIRE(utils::algorithm::all_equal(3.625));

    REQUIRE(utils::algorithm::all_equal(false));
    REQUIRE(utils::algorithm::all_equal(0));

    REQUIRE_FALSE(utils::algorithm::all_equal( true, true, true, false, true, true,  true, true, true, true));
    REQUIRE_FALSE(utils::algorithm::all_equal( true, true, true, false, true, true,  true, true, true, true));
    REQUIRE_FALSE(utils::algorithm::all_equal(false, true, true,  true, true, true,  true, true, true, true));
    REQUIRE_FALSE(utils::algorithm::all_equal( true, true, true, false, true, true, false, true, true, true));
    REQUIRE_FALSE(utils::algorithm::all_equal( true, true, true, false, true, true,  true, true, true, false));
    REQUIRE_FALSE(utils::algorithm::all_equal(false, false, false, true, false, false, false, false, false, false));

    REQUIRE(utils::algorithm::all_equal(false, false, false, false, false, false, false, false, false, false));
    REQUIRE(utils::algorithm::all_equal(true, true, true, true, true, true, true, true, true, true));

    REQUIRE(utils::algorithm::all_equal(321123.7774, 321123.7774, 321123.7774, 321123.7774));
    REQUIRE_FALSE(utils::algorithm::all_equal(321123.7774, 321123.777400001, 321123.7774, 321123.7774));

    REQUIRE(utils::algorithm::all_equal(42, 42, 42, 42, 42, 42, 42, 42, 42));
    REQUIRE_FALSE(utils::algorithm::all_equal(42, 42, 42.4, 42, 42, 42, 42, 42));
    REQUIRE_FALSE(utils::algorithm::all_equal(42, 42, 42, 42, 42, 42, 43, 42, 52));
}

TEST_CASE("Test utils::algorithm::sum", "[utils][utils::algorithm]") {
    REQUIRE(utils::algorithm::sum( ) == 0);
    REQUIRE(utils::algorithm::sum(0) == 0);
    REQUIRE(utils::algorithm::sum(1) == 1);
    REQUIRE(utils::algorithm::sum(0.0) == 0.0);

    const double rnd = utils::random::Random::get<double>();
    REQUIRE(utils::algorithm::sum(rnd)      == Approx(rnd));
    REQUIRE(utils::algorithm::sum(rnd, rnd) == Approx(rnd + rnd));

    const double div = rnd / 5.0;
    REQUIRE(utils::algorithm::sum(div, div, div, div, div) == Approx(rnd));

    REQUIRE(utils::algorithm::sum(10, 20, 30, 40, 50) == 150);
}

TEST_CASE("Test utils::algorithm::multiply", "[utils][utils::algorithm]") {
    REQUIRE(utils::algorithm::multiply( ) == 1);
    REQUIRE(utils::algorithm::multiply(0) == 0);
    REQUIRE(utils::algorithm::multiply(1) == 1);
    REQUIRE(utils::algorithm::multiply(0.0) == 0.0);

    const double rnd = utils::random::Random::get<double>();
    REQUIRE(utils::algorithm::multiply(rnd)      == Approx(rnd));
    REQUIRE(utils::algorithm::multiply(rnd, rnd) == Approx(rnd * rnd));

    const double div = rnd / utils::random::Random::get<double>();
    REQUIRE(utils::algorithm::multiply(div, div, div, div, div) == Approx(std::pow(div, 5.0)));

    REQUIRE(utils::algorithm::multiply(10, 20, 30, 40, 50) == 12000000);
}

#endif
