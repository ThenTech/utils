#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/utils_catch.hpp"

#include "../utils_lib/utils_math.hpp"

#include "../utils_lib/utils_random.hpp"


TEST_CASE("Test utils::math::sign", "[utils][utils::math]" ) {
    REQUIRE(utils::math::sign(-1) == -1);
    REQUIRE(utils::math::sign( 1) ==  1);
    REQUIRE(utils::math::sign( 0) ==  0);
    REQUIRE(utils::math::sign(int16_t(0x8001)) == -1);

    const int val_neg = utils::random::Random::get<int>(std::numeric_limits<int>::min(), -1);
    const int val_zer = utils::random::Random::get<int>(0, 0);
    const int val_one = utils::random::Random::get<int>(1);
    REQUIRE(utils::math::sign(val_neg) == -1);
    REQUIRE(utils::math::sign(val_zer) ==  0);
    REQUIRE(utils::math::sign(val_one) ==  1);
}

TEST_CASE("Test utils::math::gcd", "[utils][utils::math]" ) {
    REQUIRE(utils::math::gcd(10,  0) == 10);
    REQUIRE(utils::math::gcd( 0, 10) == 10);
    REQUIRE(utils::math::gcd(10,  1) ==  1);
    REQUIRE(utils::math::gcd( 1, 10) ==  1);
    REQUIRE(utils::math::gcd( 5, 15) ==  5);

    constexpr int imax = std::numeric_limits<int>::max();
    const int64_t irnd = utils::random::Random::get<int>(1);

    REQUIRE(utils::math::gcd(2, 3) == 1);
    REQUIRE(utils::math::gcd(0, imax) == imax);
    REQUIRE(utils::math::gcd(imax, 0) == imax);
    REQUIRE(utils::math::gcd(1, imax) == 1);
    REQUIRE(utils::math::gcd(imax, 1) == 1);
    REQUIRE(utils::math::gcd(0, 0) == 0);
    REQUIRE(utils::math::gcd(1, 1) == 1);
    REQUIRE(utils::math::gcd(imax, imax) == imax);
    REQUIRE(utils::math::gcd(irnd, irnd) == irnd);
    REQUIRE(utils::math::gcd(-irnd, irnd) == irnd);
    REQUIRE(utils::math::gcd(irnd, -irnd) == irnd);
    REQUIRE(utils::math::gcd(-irnd, -irnd) == irnd);

    REQUIRE(utils::math::gcd(11 * 2, 17 * 2) == 2);
    REQUIRE(utils::math::gcd(irnd, irnd * 3) == irnd);

    REQUIRE(utils::math::gcd(11 * 2, 17 * 3) == 1);
    REQUIRE(utils::math::gcd(irnd, double(irnd * 3.0)) == Approx(irnd));
    REQUIRE(utils::math::gcd(-irnd, double(irnd)) == Approx(irnd));
}

TEST_CASE("Test utils::math::lcm", "[utils][utils::math]" ) {
    REQUIRE_FUNCTION_ABORTS(utils::math::lcm<int, int>,  0,  0);
    REQUIRE_FUNCTION_ABORTS(utils::math::lcm<int, int>,  0, 10);
    REQUIRE_FUNCTION_ABORTS(utils::math::lcm<int, int>, 10,  0);

    REQUIRE(utils::math::lcm(1, 1) == 1);
    REQUIRE(utils::math::lcm(10,  1) == 10);
    REQUIRE(utils::math::lcm( 1, 10) == 10);
    REQUIRE(utils::math::lcm( 5, 15) == 15);
    REQUIRE(utils::math::lcm( 2,  3) == (2 * 3));

    constexpr int imax = std::numeric_limits<int>::max();
    const int64_t irnd = utils::random::Random::get<int>(1);

    REQUIRE_FUNCTION_ABORTS(utils::math::lcm<int, int>,    0, imax);
    REQUIRE_FUNCTION_ABORTS(utils::math::lcm<int, int>, imax,    0);
    REQUIRE(utils::math::lcm(1, imax) == imax);
    REQUIRE(utils::math::lcm(imax, 1) == imax);
    REQUIRE(utils::math::lcm(imax, imax) == imax);
    REQUIRE(utils::math::lcm(irnd, irnd) == irnd);
    REQUIRE(utils::math::lcm(-irnd, irnd) == irnd);
    REQUIRE(utils::math::lcm(irnd, -irnd) == irnd);
    REQUIRE(utils::math::lcm(-irnd, -irnd) == irnd);

    REQUIRE(utils::math::lcm(irnd, irnd * 3) == (irnd * 3));
    REQUIRE(utils::math::lcm(irnd, double(irnd * 3.0)) == Approx(irnd * 3));
    REQUIRE(utils::math::lcm(-irnd, double(irnd)) == Approx(irnd));

    REQUIRE(utils::math::lcm_chain(1, 2, 3, 4, 5)    ==  60);
    REQUIRE(utils::math::lcm_chain(2, 7, 11)         == 154);
    REQUIRE(utils::math::lcm_chain(irnd, irnd, irnd) == irnd);
    REQUIRE_FUNCTION_ABORTS([&](int) {
        return utils::math::lcm_chain(irnd, 2, 0, 3, 1);
    }, 0);
}

TEST_CASE("Test utils::math::epsilon_equals", "[utils][utils::math]" ) {
    constexpr double eps = 1e-4;

    REQUIRE(utils::math::epsilon_equals(0.0f  , 0.0f   , eps));
    REQUIRE(utils::math::epsilon_equals(1.0   , 1.0    , eps));
    REQUIRE(utils::math::epsilon_equals(1     , 1      , eps));
    REQUIRE(utils::math::epsilon_equals(0.0001, 0.0001 , eps));
    REQUIRE(utils::math::epsilon_equals(0.0001, 0.00015, eps));
    REQUIRE(utils::math::epsilon_equals(0.0001, 0.0002 , 1e-3));

    REQUIRE_FALSE(utils::math::epsilon_equals(1     , 0     , eps));
    REQUIRE_FALSE(utils::math::epsilon_equals(0.0   , 1.0   , eps));
    REQUIRE_FALSE(utils::math::epsilon_equals(0.0001, 0.0002, eps));
}


#endif
