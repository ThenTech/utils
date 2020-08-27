#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/doctest.hpp"

#include "../utils_lib/utils_random.hpp"
#include "../utils_lib/utils_string.hpp"
#include "../utils_lib/utils_algorithm.hpp"
#include <algorithm>
#include <vector>

TEST_CASE("Test utils::random::pick_x_from") {
    const std::vector<int> vec{1,2,3,4,5,6,7,8,9,10};
    const std::vector<int> empty{};

    auto s1 = utils::random::pick_x_from(10, empty);
    CHECK(s1.size() == 0);

    s1 = utils::random::pick_x_from(0, vec);
    CHECK(s1.size() == 0);

    s1 = utils::random::pick_x_from(10, vec);
    CHECK(s1.size() == 10);
    CHECK(utils::algorithm::within(1, 10, s1));

    s1 = utils::random::sample(5, vec);
    CHECK(s1.size() == 5);
    CHECK(utils::algorithm::within(1, 10, s1));

    s1 = utils::random::pick_x_from(3, vec);
    CHECK(s1.size() == 3);
    CHECK(utils::algorithm::within(1, 10, s1));
}

TEST_CASE("Test utils::random::sample") {
    const std::vector<int> vec{1,2,3,4,5,6,7,8,9,10};
    const std::vector<int> empty{};

    auto s1 = utils::random::sample(10, empty);
    CHECK(s1.size() == 0);

    s1 = utils::random::sample(0, vec);
    CHECK(s1.size() == 0);

    s1 = utils::random::sample(10, vec);
    CHECK(vec == s1);

    s1 = utils::random::sample(5, vec);
    CHECK(s1.size() == 5);
    CHECK(utils::algorithm::within(1, 10, s1));
    CHECK(utils::algorithm::is_ascending(s1));

    s1 = utils::random::sample(3, vec);
    CHECK(s1.size() == 3);
    CHECK(utils::algorithm::within(1, 10, s1));
    CHECK(utils::algorithm::is_ascending(s1));
}

TEST_CASE("Test utils::random::generate_x") {
    auto cont = utils::random::generate_x<int>(0);
    CHECK(cont.size() == 0);

    cont = utils::random::generate_x<int>(10);
    CHECK(cont.size() == 10);
    CHECK(utils::algorithm::within(std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), cont));

    cont = utils::random::generate_x<int>(10, -100, 100);
    CHECK(cont.size() == 10);
    CHECK(utils::algorithm::within(-100, 100, cont));

    cont = utils::random::generate_x<int>(10, 100, -100);
    CHECK(cont.size() == 10);
    CHECK(utils::algorithm::within(-100, 100, cont));

    auto cont2 = utils::random::generate_x<bool>(10);
    CHECK(cont2.size() == 10);
    CHECK(utils::algorithm::within(false, true, cont2));

    auto cont3 = utils::random::generate_x<uint8_t>(10);
    CHECK(cont3.size() == 10);
    CHECK(utils::algorithm::within(0x00, 0xFF, cont3));

    auto cont4 = utils::random::generate_x<float>(10, -5.221f, 85.0144f);
    CHECK(cont4.size() == 10);
    CHECK(utils::algorithm::within(-5.221f, 85.0144f, cont4));
}

TEST_CASE("Test utils::random::generate_string") {
    auto str = utils::random::generate_string<char>(0);
    CHECK(str.size() == 0);

    str = utils::random::generate_string<char>(100);
    CHECK(str.size() == 100);
    CHECK(utils::algorithm::within(std::numeric_limits<char>::min(), std::numeric_limits<char>::max(), str));
}

TEST_CASE("Test utils::random::generate_safe_string") {
    auto str = utils::random::generate_safe_string<char>(0);
    CHECK(str.size() == 0);

    str = utils::random::generate_safe_string(100);
    CHECK(str.size() == 100);
    CHECK(std::all_of(str.begin(), str.end(), [](const char c){
        return utils::string::contains(utils::string::_base64_chars, c);
    }));
}

TEST_CASE("Test utils::random::generate_bool") {
    auto cont = utils::random::generate_bool(0);
    CHECK(cont.size() == 0);

    cont = utils::random::generate_bool(10, 0.0);
    CHECK(cont.size() == 10);
    CHECK(std::all_of(cont.begin(), cont.end(), [](const bool x){
        return x == false;
    }));

    cont = utils::random::generate_bool(10, 1.0);
    CHECK(cont.size() == 10);
    CHECK(std::all_of(cont.begin(), cont.end(), [](const bool x){
        return x == true;
    }));

    cont = utils::random::generate_bool(10);
    CHECK(cont.size() == 10);
    CHECK(utils::algorithm::contains(cont, false).has_value());
    CHECK(utils::algorithm::contains(cont, true).has_value());
}

TEST_CASE("Test utils::random::generate_uuid") {
    const std::string guid = utils::random::generate_uuid();
    REQUIRE(guid.size() == 36);
    REQUIRE(guid != utils::random::generate_uuid());

    const auto parts = utils::string::split(guid, '-');
    REQUIRE(parts.size() == 5);
    CHECK(parts[0].size() == 8);
    CHECK(parts[1].size() == 4);
    CHECK(parts[2].size() == 4);
    CHECK(parts[3].size() == 4);
    CHECK(parts[4].size() == 12);
}

#endif
