#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/doctest.hpp"

#include "../utils_lib/utils_math.hpp"

#include "../utils_lib/utils_random.hpp"


TEST_CASE("Test utils::math::sign") {
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


TEST_CASE("Test utils::math::sum") {
    REQUIRE(utils::math::sum( ) == 0);
    REQUIRE(utils::math::sum(0) == 0);
    REQUIRE(utils::math::sum(1) == 1);
    REQUIRE(utils::math::sum(0.0) == 0.0);

    const double rnd = utils::random::Random::get<double>(-1e6, 1e6);
    REQUIRE(utils::math::sum(rnd)      == doctest::Approx(rnd));
    REQUIRE(utils::math::sum(rnd, rnd) == doctest::Approx(rnd + rnd));

    const double div = rnd / 5.0;
    REQUIRE(utils::math::sum(div, div, div, div, div) == doctest::Approx(rnd));

    REQUIRE(utils::math::sum(10, 20, 30, 40, 50) == 150);
}

TEST_CASE("Test utils::math::product") {
    REQUIRE(utils::math::product( ) == 1);
    REQUIRE(utils::math::product(0) == 0);
    REQUIRE(utils::math::product(1) == 1);
    REQUIRE(utils::math::product(0.0) == 0.0);

    const double rnd = utils::random::Random::get<double>(-1e6, 1e6);
    REQUIRE(utils::math::product(rnd)      == doctest::Approx(rnd));
    REQUIRE(utils::math::product(rnd, rnd) == doctest::Approx(rnd * rnd));

    const double div = rnd / utils::random::Random::get<double>(-1e6, 1e6);
    REQUIRE(utils::math::product(div, div, div, div, div) == doctest::Approx(std::pow(div, 5.0)));

    REQUIRE(utils::math::product(10, 20, 30, 40, 50) == 12000000);
}

TEST_CASE("Test utils::math::min") {
    REQUIRE(utils::math::min( 0,  0) ==  0);
    REQUIRE(utils::math::min( 1,  0) ==  0);
    REQUIRE(utils::math::min( 0,  1) ==  0);
    REQUIRE(utils::math::min(-1,  0) == -1);
    REQUIRE(utils::math::min( 0, -1) == -1);

    const double smaller = utils::random::Random::get<double>(0.0, 100.0);
    const double bigger  = utils::random::Random::get<double>(smaller + 1.0);
    REQUIRE(utils::math::min(smaller, bigger ) == doctest::Approx(smaller));
    REQUIRE(utils::math::min(bigger , smaller) == doctest::Approx(smaller));

    REQUIRE(utils::math::min(bigger,  bigger, bigger,  bigger, smaller) == doctest::Approx(smaller));
    REQUIRE(utils::math::min(bigger,  bigger, bigger, smaller) == doctest::Approx(smaller));
    REQUIRE(utils::math::min(bigger, smaller, bigger,  bigger) == doctest::Approx(smaller));
}

TEST_CASE("Test utils::math::max") {
    REQUIRE(utils::math::max( 0,  0) ==  0);
    REQUIRE(utils::math::max( 1,  0) ==  1);
    REQUIRE(utils::math::max( 0,  1) ==  1);
    REQUIRE(utils::math::max(-1,  0) ==  0);
    REQUIRE(utils::math::max( 0, -1) ==  0);

    const double smaller = utils::random::Random::get<double>(0.0, 100.0);
    const double bigger  = utils::random::Random::get<double>(smaller + 1.0);
    REQUIRE(utils::math::max(smaller, bigger ) == doctest::Approx(bigger));
    REQUIRE(utils::math::max(bigger , smaller) == doctest::Approx(bigger));

    REQUIRE(utils::math::max(smaller, smaller, smaller, smaller, bigger) == doctest::Approx(bigger));
    REQUIRE(utils::math::max(smaller, smaller, smaller,  bigger) == doctest::Approx(bigger));
    REQUIRE(utils::math::max(smaller,  bigger, smaller, smaller) == doctest::Approx(bigger));
}

TEST_CASE("Test utils::math::pow") {
    REQUIRE(utils::math::pow( 0) ==  0);
    REQUIRE(utils::math::pow( 1) ==  1);
    REQUIRE(utils::math::pow<0>(42) == 1);
    REQUIRE(utils::math::pow<1>(42) == 42);
    REQUIRE(utils::math::pow<2>(42) == 1764);
    REQUIRE(utils::math::pow   (42) == 1764);

    REQUIRE(utils::math::pow<10>(2) == (1 << 10));
    REQUIRE(utils::math::pow<12>(4) == (4*4*4*4*4*4*4*4*4*4*4*4));

    REQUIRE(utils::math::pow   (1.9) == doctest::Approx(3.61));
    REQUIRE(utils::math::pow<5>(2.5) == doctest::Approx(97.65625));
}

TEST_CASE("Test utils::math::gcd") {
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
    REQUIRE(utils::math::gcd(irnd, double(irnd * 3.0)) == doctest::Approx(irnd));
    REQUIRE(utils::math::gcd(-irnd, double(irnd)) == doctest::Approx(irnd));
}

TEST_CASE("Test utils::math::lcm") {
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
    REQUIRE(utils::math::lcm(irnd, double(irnd * 3.0)) == doctest::Approx(irnd * 3));
    REQUIRE(utils::math::lcm(-irnd, double(irnd)) == doctest::Approx(irnd));

    REQUIRE(utils::math::lcm_chain(1, 2, 3, 4, 5)    ==  60);
    REQUIRE(utils::math::lcm_chain(2, 7, 11)         == 154);
    REQUIRE(utils::math::lcm_chain(irnd, irnd, irnd) == irnd);
    REQUIRE(utils::math::lcm_chain(irnd, 2, 0, 3, 1) == 0);
}

TEST_CASE("Test utils::math::epsilon_equals") {
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

    REQUIRE(utils::math::epsilon_rel_equals(1.0   , 1.0    , eps));
    REQUIRE(utils::math::epsilon_rel_equals(1     , 1      , eps));
    REQUIRE(utils::math::epsilon_rel_equals(0.0001, 0.0001 , eps));

    REQUIRE_FALSE(utils::math::epsilon_rel_equals(0.0f  , 0.0f   , eps));
    REQUIRE_FALSE(utils::math::epsilon_rel_equals(0.0001, 0.00015, eps));
    REQUIRE_FALSE(utils::math::epsilon_rel_equals(0.0001, 0.0002 , 1e-3));
    REQUIRE_FALSE(utils::math::epsilon_rel_equals(1     , 0     , eps));
    REQUIRE_FALSE(utils::math::epsilon_rel_equals(0.0   , 1.0   , eps));
    REQUIRE_FALSE(utils::math::epsilon_rel_equals(0.0001, 0.0002, eps));
    REQUIRE(utils::math::epsilon_rel_equals(0.0001, 0.0002, 1.0));
}

TEST_CASE("Test utils::math::within") {
    REQUIRE(utils::math::within          (0.0f  , -1.0f, 1.0f));
    REQUIRE(utils::math::within_inclusive(0.0f  ,  0.0f, 0.0f));
    REQUIRE_FALSE(utils::math::within    (0.0f  ,  0.0f, 0.0f));

    REQUIRE(utils::math::within(5, 0, 10));
    REQUIRE(utils::math::within(9, 0, 10));
    REQUIRE_FALSE(utils::math::within(10, 0, 10));
    REQUIRE_FALSE(utils::math::within(0, 0, 10));
    REQUIRE(utils::math::within_inclusive(0, 0, 10));
    REQUIRE(utils::math::within_inclusive(10, 0, 10));
}

TEST_CASE("Test utils::math::mix") {
    CHECK(utils::math::mix(5, 5, 0.00) == doctest::Approx(5));
    CHECK(utils::math::mix(5, 5, 0.33) == doctest::Approx(5));
    CHECK(utils::math::mix(5, 5, 0.50) == doctest::Approx(5));
    CHECK(utils::math::mix(5, 5, 0.75) == doctest::Approx(5));
    CHECK(utils::math::mix(5, 5, 1.00) == doctest::Approx(5));

    CHECK(utils::math::mix( 0, 10, 0.6) == doctest::Approx(6));
    CHECK(utils::math::mix(10,  0, 0.6) == doctest::Approx(4));

    CHECK(utils::math::mix( 5, 10, 0.5) == doctest::Approx(7.5));
}

TEST_CASE("Test utils::math::interpolate_linear") {
    CHECK(utils::math::interpolate_linear(0, 10, 0, 1000, 0) == 0);
    CHECK(utils::math::interpolate_linear(0, 10, 0, 1000, 5) == 500);

    CHECK(utils::math::interpolate_linear(0.0, 10.0, 0.0, 1000.0, 7.32)
            == doctest::Approx(732));
    CHECK(utils::math::interpolate_linear(0.0, 10.0, 233.4, 1234.12, 7.32)
            == doctest::Approx(965.92704));
}

TEST_CASE("Test utils::math::interpolate_hermite") {
    CHECK(utils::math::interpolate_hermite(0, 10, 0, 1000, 0) == 0);
    CHECK(utils::math::interpolate_hermite(0, 10, 0, 1000, 5) == 500);

    CHECK(utils::math::interpolate_hermite(0.0, 10.0, 0.0, 1000.0, 7.32)
            == doctest::Approx(823.026));
    CHECK(utils::math::interpolate_hermite(0.0, 10.0, 233.4, 1234.12, 7.32)
            == doctest::Approx(1057.018242));
}

TEST_CASE("Test utils::math::rounded") {
    const auto gen = utils::random::generate_x(10, 0.0, 100.0);
    auto test = gen;

    utils::math::rounded<2>(test);
    for (size_t i = 0; i < 10; i++) {
        CHECK(test[i] == doctest::Approx(gen[i]).epsilon(0.01));
    }

    utils::math::rounded<1>(test.begin(), test.end());
    for (size_t i = 0; i < 10; i++) {
        CHECK(test[i] == doctest::Approx(gen[i]).epsilon(0.1));
    }

    utils::math::rounded<0>(test.begin(), test.end());
    for (size_t i = 0; i < 10; i++) {
        CHECK(test[i] == doctest::Approx(gen[i]).epsilon(1));
    }
}

TEST_CASE("Test utils::math::stats") {
    constexpr size_t len = 9;

    std::vector<int> empty;
    std::vector<int> one(1);
    std::vector<int> two(2);
    std::vector<int> test(len);
    std::iota(test.begin(), test.end(), 0);

    const std::vector<int> test2 = utils::random::generate_x<int>(3, -1000, 1000);

    SUBCASE("Test utils::math::stats::mean") {
        const double result = double(0+1+2+3+4+5+6+7+8) / len;
        const double result1 = utils::math::stats::mean(test.begin(), test.end());
        const double result2 = utils::math::stats::mean(test);
        const double result3 = utils::math::stats::mean(test2);

        CHECK(result == doctest::Approx(result1));
        CHECK(result == doctest::Approx(result2));
        CHECK((double(test2[0]+test2[1]+test2[2]) / 3.0) == doctest::Approx(result3));

        CHECK_FUNCTION_ABORTS(utils::math::stats::mean<std::vector<int>>, empty);
        CHECK_FUNCTION_ABORTS_FALSE(utils::math::stats::mean<std::vector<int>>, one);
    }

    SUBCASE("Test utils::math::stats::variance") {
        const double result = 6.66666666666667;
        const double result1 = utils::math::stats::variance(test.begin(), test.end());
        const double result2 = utils::math::stats::variance(test);
        const double result3 = utils::math::stats::variance(test2);
        const double mean3   = utils::math::stats::mean(test2);
        const double sd3 = (  (test2[0] - mean3) * (test2[0] - mean3)
                            + (test2[1] - mean3) * (test2[1] - mean3)
                            + (test2[2] - mean3) * (test2[2] - mean3)
                           ) / 3.0;

        CHECK(result == doctest::Approx(result1));
        CHECK(result == doctest::Approx(result2));
        CHECK(sd3    == doctest::Approx(result3));

        CHECK_FUNCTION_ABORTS(utils::math::stats::variance<std::vector<int>>, empty);
        CHECK_FUNCTION_ABORTS_FALSE(utils::math::stats::variance<std::vector<int>>, one);
        CHECK_FUNCTION_ABORTS_FALSE(utils::math::stats::variance<std::vector<int>>, two);
    }

    SUBCASE("Test utils::math::stats::stddev") {
        const double result = 2.5819888975;
        const double result1 = utils::math::stats::stddev(test.begin(), test.end());
        const double result2 = utils::math::stats::stddev(test);
        const double result3 = utils::math::stats::stddev(test2);
        const double mean3   = utils::math::stats::mean(test2);
        const double sd3 = std::sqrt((  (test2[0] - mean3) * (test2[0] - mean3)
                                      + (test2[1] - mean3) * (test2[1] - mean3)
                                      + (test2[2] - mean3) * (test2[2] - mean3)
                                     ) / 3.0);

        CHECK(result == doctest::Approx(result1));
        CHECK(result == doctest::Approx(result2));
        CHECK(sd3    == doctest::Approx(result3));

        CHECK_FUNCTION_ABORTS(utils::math::stats::stddev<std::vector<int>>, empty);
        CHECK_FUNCTION_ABORTS_FALSE(utils::math::stats::stddev<std::vector<int>>, one);
        CHECK_FUNCTION_ABORTS_FALSE(utils::math::stats::stddev<std::vector<int>>, two);
    }

    SUBCASE("Test utils::math::stats::normalise") {
        std::vector<double> testd(len);
        std::iota(testd.begin(), testd.end(), 0.0);
        std::vector<double> result(len);
        std::iota(result.begin(), result.end(), -4);
        std::for_each(result.begin(), result.end(),
            [stddev = utils::math::stats::stddev(result)](double& x){
                x /= stddev;
        });
        auto testd2 = testd;

        utils::math::stats::normalise(testd.begin(), testd.end());
        utils::math::stats::normalise(testd2);

        for (size_t i = 0; i < len; i++) {
            CHECK(testd[i]  == doctest::Approx(result[i]));
            CHECK(testd2[i] == doctest::Approx(result[i]));
        }

        std::vector<double> testd3 = utils::random::generate_x<double>(3, -1000.0, 1000.0);
        const double mean3 = utils::math::stats::mean(testd3);
        std::vector<double> result3 {
            testd3[0] - mean3,
            testd3[1] - mean3,
            testd3[2] - mean3
        };
        const double sd3   = std::sqrt((  result3[0] * result3[0]
                                        + result3[1] * result3[1]
                                        + result3[2] * result3[2]
                                       ) / 3.0);

        utils::math::stats::normalise(testd3);
        CHECK(testd3[0] == doctest::Approx(result3[0] / sd3));
        CHECK(testd3[1] == doctest::Approx(result3[1] / sd3));
        CHECK(testd3[2] == doctest::Approx(result3[2] / sd3));
    }
}

#endif
