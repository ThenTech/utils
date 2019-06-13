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


TEST_CASE("Test utils::math::sum", "[utils][utils::math]") {
    REQUIRE(utils::math::sum( ) == 0);
    REQUIRE(utils::math::sum(0) == 0);
    REQUIRE(utils::math::sum(1) == 1);
    REQUIRE(utils::math::sum(0.0) == 0.0);

    const double rnd = utils::random::Random::get<double>();
    REQUIRE(utils::math::sum(rnd)      == Approx(rnd));
    REQUIRE(utils::math::sum(rnd, rnd) == Approx(rnd + rnd));

    const double div = rnd / 5.0;
    REQUIRE(utils::math::sum(div, div, div, div, div) == Approx(rnd));

    REQUIRE(utils::math::sum(10, 20, 30, 40, 50) == 150);
}

TEST_CASE("Test utils::math::product", "[utils][utils::math]") {
    REQUIRE(utils::math::product( ) == 1);
    REQUIRE(utils::math::product(0) == 0);
    REQUIRE(utils::math::product(1) == 1);
    REQUIRE(utils::math::product(0.0) == 0.0);

    const double rnd = utils::random::Random::get<double>();
    REQUIRE(utils::math::product(rnd)      == Approx(rnd));
    REQUIRE(utils::math::product(rnd, rnd) == Approx(rnd * rnd));

    const double div = rnd / utils::random::Random::get<double>();
    REQUIRE(utils::math::product(div, div, div, div, div) == Approx(std::pow(div, 5.0)));

    REQUIRE(utils::math::product(10, 20, 30, 40, 50) == 12000000);
}

TEST_CASE("Test utils::math::min", "[utils][utils::math]") {
    REQUIRE(utils::math::min( 0,  0) ==  0);
    REQUIRE(utils::math::min( 1,  0) ==  0);
    REQUIRE(utils::math::min( 0,  1) ==  0);
    REQUIRE(utils::math::min(-1,  0) == -1);
    REQUIRE(utils::math::min( 0, -1) == -1);

    const double smaller = utils::random::Random::get<double>(0.0, 100.0);
    const double bigger  = utils::random::Random::get<double>(smaller + 1.0);
    REQUIRE(utils::math::min(smaller, bigger ) == Approx(smaller));
    REQUIRE(utils::math::min(bigger , smaller) == Approx(smaller));

    REQUIRE(utils::math::min(bigger,  bigger, bigger,  bigger, smaller) == Approx(smaller));
    REQUIRE(utils::math::min(bigger,  bigger, bigger, smaller) == Approx(smaller));
    REQUIRE(utils::math::min(bigger, smaller, bigger,  bigger) == Approx(smaller));
}

TEST_CASE("Test utils::math::max", "[utils][utils::math]") {
    REQUIRE(utils::math::max( 0,  0) ==  0);
    REQUIRE(utils::math::max( 1,  0) ==  1);
    REQUIRE(utils::math::max( 0,  1) ==  1);
    REQUIRE(utils::math::max(-1,  0) ==  0);
    REQUIRE(utils::math::max( 0, -1) ==  0);

    const double smaller = utils::random::Random::get<double>(0.0, 100.0);
    const double bigger  = utils::random::Random::get<double>(smaller + 1.0);
    REQUIRE(utils::math::max(smaller, bigger ) == Approx(bigger));
    REQUIRE(utils::math::max(bigger , smaller) == Approx(bigger));

    REQUIRE(utils::math::max(smaller, smaller, smaller, smaller, bigger) == Approx(bigger));
    REQUIRE(utils::math::max(smaller, smaller, smaller,  bigger) == Approx(bigger));
    REQUIRE(utils::math::max(smaller,  bigger, smaller, smaller) == Approx(bigger));
}

TEST_CASE("Test utils::math::pow", "[utils][utils::math]" ) {
    REQUIRE(utils::math::pow( 0) ==  0);
    REQUIRE(utils::math::pow( 1) ==  1);
    REQUIRE(utils::math::pow<0>(42) == 1);
    REQUIRE(utils::math::pow<1>(42) == 42);
    REQUIRE(utils::math::pow<2>(42) == 1764);
    REQUIRE(utils::math::pow   (42) == 1764);

    REQUIRE(utils::math::pow<10>(2) == (1 << 10));
    REQUIRE(utils::math::pow<12>(4) == (4*4*4*4*4*4*4*4*4*4*4*4));

    REQUIRE(utils::math::pow   (1.9) == Approx(3.61));
    REQUIRE(utils::math::pow<5>(2.5) == Approx(97.65625));
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
    REQUIRE(utils::math::lcm<int, int>( 0,  0) == 0);
    REQUIRE(utils::math::lcm<int, int>( 0, 10) == 0);
    REQUIRE(utils::math::lcm<int, int>(10,  0) == 0);

    REQUIRE(utils::math::lcm(1, 1) == 1);
    REQUIRE(utils::math::lcm(10,  1) == 10);
    REQUIRE(utils::math::lcm( 1, 10) == 10);
    REQUIRE(utils::math::lcm( 5, 15) == 15);
    REQUIRE(utils::math::lcm( 2,  3) == (2 * 3));

    constexpr int imax = std::numeric_limits<int>::max();
    const int64_t irnd = utils::random::Random::get<int>(1);

    REQUIRE(utils::math::lcm<int, int>(   0, imax) == 0);
    REQUIRE(utils::math::lcm<int, int>(imax,    0) == 0);
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
    REQUIRE(utils::math::lcm_chain(irnd, 2, 0, 3, 1) == 0);
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

TEST_CASE("Test utils::math::stats", "[utils][utils::math][utils::math::stats]" ) {
    constexpr size_t len = 9;

    std::vector<int> empty;
    std::vector<int> one(1);
    std::vector<int> two(2);
    std::vector<int> test(len);
    std::iota(test.begin(), test.end(), 0);

    const std::vector<int> test2 = utils::random::generate_x<int>(3, -1000, 1000);

    SECTION("utils::math::stats::mean") {
        const double result = double(0+1+2+3+4+5+6+7+8) / len;
        const double result1 = utils::math::stats::mean(test.begin(), test.end());
        const double result2 = utils::math::stats::mean(test);
        const double result3 = utils::math::stats::mean(test2);

        CHECK(result == Approx(result1));
        CHECK(result == Approx(result2));
        CHECK((double(test2[0]+test2[1]+test2[2]) / 3.0) == Approx(result3));

        CHECK_FUNCTION_ABORTS(utils::math::stats::mean<std::vector<int>>, empty);
        CHECK_FUNCTION_ABORTS_FALSE(utils::math::stats::mean<std::vector<int>>, one);
    }

    SECTION("utils::math::stats::variance") {
        const double result = 7.5;
        const double result1 = utils::math::stats::variance(test.begin(), test.end());
        const double result2 = utils::math::stats::variance(test);
        const double result3 = utils::math::stats::variance(test2);
        const double mean3   = utils::math::stats::mean(test2);
        const double sd3 = (  (test2[0] - mean3) * (test2[0] - mean3)
                            + (test2[1] - mean3) * (test2[1] - mean3)
                            + (test2[2] - mean3) * (test2[2] - mean3)
                           ) / 2.0;

        CHECK(result == Approx(result1));
        CHECK(result == Approx(result2));
        CHECK(sd3    == Approx(result3));

        CHECK_FUNCTION_ABORTS(utils::math::stats::variance<std::vector<int>>, empty);
        CHECK_FUNCTION_ABORTS(utils::math::stats::variance<std::vector<int>>, one);
        CHECK_FUNCTION_ABORTS_FALSE(utils::math::stats::variance<std::vector<int>>, two);
    }

    SECTION("utils::math::stats::stddev") {
        const double result = 2.7386127875258;
        const double result1 = utils::math::stats::stddev(test.begin(), test.end());
        const double result2 = utils::math::stats::stddev(test);
        const double result3 = utils::math::stats::stddev(test2);
        const double mean3   = utils::math::stats::mean(test2);
        const double sd3 = std::sqrt((  (test2[0] - mean3) * (test2[0] - mean3)
                                      + (test2[1] - mean3) * (test2[1] - mean3)
                                      + (test2[2] - mean3) * (test2[2] - mean3)
                                     ) / 2.0);

        CHECK(result == Approx(result1));
        CHECK(result == Approx(result2));
        CHECK(sd3    == Approx(result3));

        CHECK_FUNCTION_ABORTS(utils::math::stats::stddev<std::vector<int>>, empty);
        CHECK_FUNCTION_ABORTS(utils::math::stats::stddev<std::vector<int>>, one);
        CHECK_FUNCTION_ABORTS_FALSE(utils::math::stats::stddev<std::vector<int>>, two);
    }
}

#endif
