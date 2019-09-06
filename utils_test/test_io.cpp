#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/utils_catch.hpp"

#include "../utils_lib/utils_io.hpp"

#include "../utils_lib/utils_string.hpp"
#include "../utils_lib/utils_random.hpp"


TEST_CASE("Test utils::io::TemporaryFile", "[utils][utils::io]") {
    utils::io::fs::path pp;

    SECTION("Test utils::io::TemporaryFile deleted after scope") {
        {
            utils::io::TemporaryFile t;
            pp = t.get_path();

            REQUIRE(utils::io::fs::exists(pp));

            const int rnd = utils::random::Random::get<int>();
            const std::string test = utils::string::format("Hello World %d!\n", rnd);
            t.printf("Hello World %d!\n", rnd);
            t.reopen();

            char buffer[40] = { 0 };
            t.read(buffer, 40);

            CHECK(std::string(buffer) == test);
        }

        REQUIRE_FALSE(utils::io::fs::exists(pp));
    }

    SECTION("Test utils::io::TemporaryFile deleted after created after ctor") {
        {
            utils::io::TemporaryFile t(false, "", "", "_test_", ".abc");
            pp = t.get_path();

            CHECK(utils::string::ends_with(t.get_name(), ".abc"));
            CHECK(utils::string::contains(t.get_name(), "_test_"));
            REQUIRE_FALSE(utils::io::fs::exists(pp));

            std::FILE *fp = std::fopen(t.get_name().data(), "wb+");

            const int rnd = utils::random::Random::get<int>();
            const std::string test = utils::string::format("Hello World %d!\n", rnd);
            std::fprintf(fp, "Hello World %d!\n", rnd);

            std::fclose(fp);

            REQUIRE(utils::io::fs::exists(pp));

            t.reopen();
            t.seekstart();

            char buffer[40] = { 0 };
            t.read(buffer, 40);

            CHECK(std::string(buffer) == test);
        }

        REQUIRE_FALSE(utils::io::fs::exists(pp));
    }
}

#endif
