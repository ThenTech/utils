#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/utils_catch.hpp"

#include "../utils_lib/utils_string.hpp"

#include "../utils_lib/utils_random.hpp"


static const std::string SPACES { ' ', '\t', '\n' };
static constexpr std::string_view alphabet_lower = "abcdefghijklmnopqrstuvwxyz";
static constexpr std::string_view alphabet_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";


TEST_CASE("Test utils::string::contains", "[utils][utils::string]") {
    utils::traits::found_t found;

    SECTION("Test with char") {
        REQUIRE_FALSE(utils::string::contains("", '.'));
        REQUIRE_FALSE(utils::string::rcontains("", '.'));

        REQUIRE((found = utils::string::contains("     abcd     ", 'd')));
        REQUIRE(*found == 8);
        REQUIRE((found = utils::string::rcontains("     abcd     ", 'd')));
        REQUIRE(*found == 8);

        REQUIRE((found = utils::string::contains("abcd\t\t", '\t')));
        REQUIRE(*found == 4);
        REQUIRE((found = utils::string::rcontains("abcd\t\t", '\t')));
        REQUIRE(*found == 5);

        REQUIRE((found = utils::string::contains("\n\n\n\nabcd\n\n\n", 'd')));
        REQUIRE(*found == 7);
        REQUIRE((found = utils::string::rcontains("\n\n\n\nabcd\n\n\n", 'd')));
        REQUIRE(*found == 7);

        std::string test_1(alphabet_lower);
        for (int i = 5; i--;) {
            const char c = *utils::random::Random::get(test_1);

            REQUIRE((found = utils::string::contains(test_1, c)));
            REQUIRE(*found == size_t(c - 'a'));
            REQUIRE((found = utils::string::rcontains(test_1, c)));
            REQUIRE(*found == size_t(c - 'a'));
        }

        REQUIRE_FALSE(utils::string::contains(test_1, 'A'));
        REQUIRE_FALSE(utils::string::rcontains(test_1, 'A'));
    }

    SECTION("Test with string") {
        REQUIRE_FALSE(utils::string::contains("", "."));
        REQUIRE_FALSE(utils::string::rcontains("", "."));

        REQUIRE((found = utils::string::contains("     abcd     ", "d")));
        REQUIRE(*found == 8);
        REQUIRE((found = utils::string::rcontains("     abcd     ", "d")));
        REQUIRE(*found == 8);

        REQUIRE((found = utils::string::contains("     abcd     ", "  ab")));
        REQUIRE(*found == 3);
        REQUIRE((found = utils::string::rcontains("     abcd     ", "  ab")));
        REQUIRE(*found == 3);

        REQUIRE((found = utils::string::contains("abcd\t\t", "\t")));
        REQUIRE(*found == 4);
        REQUIRE((found = utils::string::rcontains("abcd\t\t", "\t")));
        REQUIRE(*found == 5);

        REQUIRE((found = utils::string::contains("abcd\t\t", "cd\t")));
        REQUIRE(*found == 2);
        REQUIRE((found = utils::string::rcontains("abcd\t\t", "cd\t")));
        REQUIRE(*found == 2);

        REQUIRE((found = utils::string::contains("\n\n\n\nabcd\n\n\n", "d")));
        REQUIRE(*found == 7);
        REQUIRE((found = utils::string::rcontains("\n\n\n\nabcd\n\n\n", "d")));
        REQUIRE(*found == 7);

        REQUIRE((found = utils::string::contains("\n\n\n\nabcd\n\n\n", "d\n")));
        REQUIRE(*found == 7);
        REQUIRE((found = utils::string::rcontains("\n\n\n\nabcd\n\n\n", "d\n")));
        REQUIRE(*found == 7);

        std::string test_1(alphabet_lower);
        std::stringstream ss;
        for (int i = 5; i--;) {
            std::stringstream().swap(ss);
            char c = utils::random::Random::get<char>('a', 'z' - 1);
            ss << c << char(c + 1);

            REQUIRE((found = utils::string::contains(test_1, ss.str())));
            REQUIRE(*found == size_t(c - 'a'));
            REQUIRE((found = utils::string::rcontains(test_1, ss.str())));
            REQUIRE(*found == size_t(c - 'a'));
        }

        REQUIRE_FALSE(utils::string::contains(test_1, "A"));
        REQUIRE_FALSE(utils::string::rcontains(test_1, "A"));
    }
}

TEST_CASE("Test utils::string::starts_with", "[utils][utils::string]") {
    CHECK_FALSE(utils::string::starts_with(""   , '.'));
    CHECK_FALSE(utils::string::starts_with(""   , '\0'));
    CHECK_FALSE(utils::string::starts_with("abc", '\0'));
    CHECK      (utils::string::starts_with("abc", 'a'));

    CHECK_FALSE(utils::string::starts_with(""   , ""));
    CHECK_FALSE(utils::string::starts_with(""   , "\0"));
    CHECK_FALSE(utils::string::starts_with("abc", "\0"));
    CHECK      (utils::string::starts_with("abc", "a"));
    CHECK      (utils::string::starts_with("abc", "abc"));

    CHECK_FALSE(utils::string::starts_with(""   , std::string("")));
    CHECK_FALSE(utils::string::starts_with(""   , std::string("\0")));
    CHECK_FALSE(utils::string::starts_with("abc", std::string("\0")));
    CHECK      (utils::string::starts_with("abc", std::string("a")));
    CHECK      (utils::string::starts_with("abc", std::string("abc")));

    CHECK_FALSE(utils::string::starts_with(std::string("")   , std::string("")));
    CHECK_FALSE(utils::string::starts_with(std::string("")   , std::string("\0")));
    CHECK_FALSE(utils::string::starts_with(std::string("abc"), std::string("\0")));
    CHECK      (utils::string::starts_with(std::string("abc"), std::string("a")));
    CHECK      (utils::string::starts_with(std::string("abc"), std::string("abc")));

    CHECK_FALSE(utils::string::starts_with(std::string("\t\n_?abcd"), std::string("ab")));
    CHECK      (utils::string::starts_with(std::string("\t\n_?abcd"), std::string("\t\n")));
}

TEST_CASE("Test utils::string::ends_with", "[utils][utils::string]") {
    CHECK_FALSE(utils::string::ends_with(""   , '.'));
    CHECK_FALSE(utils::string::ends_with(""   , '\0'));
    CHECK_FALSE(utils::string::ends_with("abc", '\0'));
    CHECK      (utils::string::ends_with("abc", 'c'));

    CHECK_FALSE(utils::string::ends_with(""   , "."));
    CHECK_FALSE(utils::string::ends_with(""   , ""));
    CHECK_FALSE(utils::string::ends_with(""   , "\0"));
    CHECK_FALSE(utils::string::ends_with("abc", "\0"));
    CHECK      (utils::string::ends_with("abc", "c"));
    CHECK      (utils::string::ends_with("abc", "abc"));

    CHECK_FALSE(utils::string::ends_with(""   , std::string(".")));
    CHECK_FALSE(utils::string::ends_with(""   , std::string("")));
    CHECK_FALSE(utils::string::ends_with(""   , std::string("\0")));
    CHECK_FALSE(utils::string::ends_with("abc", std::string("\0")));
    CHECK      (utils::string::ends_with("abc", std::string("c")));
    CHECK      (utils::string::ends_with("abc", std::string("abc")));

    CHECK_FALSE(utils::string::ends_with(std::string("")   , std::string(".")));
    CHECK_FALSE(utils::string::ends_with(std::string("")   , std::string("")));
    CHECK_FALSE(utils::string::ends_with(std::string("")   , std::string("\0")));
    CHECK_FALSE(utils::string::ends_with(std::string("abc"), std::string("\0")));
    CHECK      (utils::string::ends_with(std::string("abc"), std::string("c")));
    CHECK      (utils::string::ends_with(std::string("abc"), std::string("abc")));

    CHECK_FALSE(utils::string::ends_with(std::string("\t\n_?abcd"), std::string("ab")));
    CHECK      (utils::string::ends_with(std::string("\t\n_?abcd"), std::string("cd")));
}

TEST_CASE("Test utils::string::ltrim", "[utils][utils::string]") {
    std::string empty("");
    utils::string::ltrim(empty);
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcd     ");
    utils::string::ltrim(test_1);
    REQUIRE(test_1 == "abcd     ");

    std::string test_2("\t\t\t\tabcd\t\t");
    utils::string::ltrim(test_2);
    REQUIRE(test_2 == "abcd\t\t");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    utils::string::ltrim(test_3);
    REQUIRE(test_3 == "abcd\n\n\n");

    std::string test_4, spacesstr, charstr;

    spacesstr = utils::random::pick_x_from(utils::random::Random::get<size_t>(1, 10),
                                           SPACES);
    charstr = utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                             'a', 'z');

    test_4 = spacesstr;
    utils::string::ltrim(test_4);
    REQUIRE(test_4.size() == 0);

    test_4 = spacesstr + charstr + spacesstr;
    utils::string::ltrim(test_4);
    REQUIRE(test_4 == charstr + spacesstr);

    std::string test_5(charstr);
    utils::string::ltrim(test_5);
    REQUIRE(test_5 == charstr);
}

TEST_CASE("Test utils::string::rtrim", "[utils][utils::string]") {
    std::string empty("");
    utils::string::rtrim(empty);
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcd     ");
    utils::string::rtrim(test_1);
    REQUIRE(test_1 == "     abcd");

    std::string test_2("\t\t\t\tabcd\t\t");
    utils::string::rtrim(test_2);
    REQUIRE(test_2 == "\t\t\t\tabcd");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    utils::string::rtrim(test_3);
    REQUIRE(test_3 == "\n\n\n\nabcd");

    std::string test_4, spacesstr, charstr;

    spacesstr = utils::random::pick_x_from(utils::random::Random::get<size_t>(1, 10),
                                           SPACES);
    charstr = utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                             'a', 'z');

    test_4 = spacesstr;
    utils::string::rtrim(test_4);
    REQUIRE(test_4.size() == 0);

    test_4 = spacesstr + charstr + spacesstr;
    utils::string::rtrim(test_4);
    REQUIRE(test_4 == spacesstr + charstr);

    std::string test_5(charstr);
    utils::string::rtrim(test_5);
    REQUIRE(test_5 == charstr);
}

TEST_CASE("Test utils::string::trim", "[utils][utils::string]") {
    std::string empty("");
    utils::string::trim(empty);
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcd     ");
    utils::string::trim(test_1);
    REQUIRE(test_1 == "abcd");

    std::string test_2("\t\t\t\tabcd\t\t");
    utils::string::trim(test_2);
    REQUIRE(test_2 == "abcd");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    utils::string::trim(test_3);
    REQUIRE(test_3 == "abcd");

    std::string test_4, spacesstr, charstr;

    spacesstr = utils::random::pick_x_from(utils::random::Random::get<size_t>(1, 10),
                                           SPACES);
    charstr = utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                             'a', 'z');

    test_4 = spacesstr;
    utils::string::trim(test_4);
    REQUIRE(test_4.size() == 0);

    test_4 = spacesstr + charstr + spacesstr;
    utils::string::trim(test_4);
    REQUIRE(test_4 == charstr);

    std::string test_5(charstr);
    utils::string::trim(test_5);
    REQUIRE(test_5 == charstr);
}

TEST_CASE("Test utils::string::trimmed", "[utils][utils::string]") {
    std::string empty("");
    REQUIRE(utils::string::trimmed(empty).size() == 0);

    std::string test_1("     abcd     ");
    REQUIRE(utils::string::trimmed(test_1) == "abcd");

    std::string test_2("\t\t\t\tabcd\t\t");
    REQUIRE(utils::string::trimmed(test_2) == "abcd");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    REQUIRE(utils::string::trimmed(test_3) == "abcd");

    std::string spacesstr, charstr;

    spacesstr = utils::random::pick_x_from(utils::random::Random::get<size_t>(1, 10),
                                           SPACES);
    charstr = utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                             'a', 'z');

    REQUIRE(utils::string::trimmed(spacesstr).size() == 0);
    REQUIRE(utils::string::trimmed(spacesstr + charstr + spacesstr) == charstr);
    REQUIRE(utils::string::trimmed(charstr) == charstr);
}

TEST_CASE("Test utils::string::erase_from", "[utils][utils::string]") {
    std::string empty("");
    utils::string::erase_from(empty, ".");
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcd     ");
    utils::string::erase_from(test_1, "d");
    REQUIRE(test_1 == "     abc");

    std::string test_2("\t\t\t\tabcd\t\t");
    utils::string::erase_from(test_2, "\ta");
    REQUIRE(test_2 == "\t\t\t");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    utils::string::erase_from(test_3, "dcba");
    REQUIRE(test_3 == "\n\n\n\nabcd\n\n\n");

    std::string test_4(alphabet_lower);
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size() - 1);
    const std::string erase_char(1, test_4[erase_index]);

    utils::string::erase_from(test_4, erase_char);
    REQUIRE(test_4.size() == erase_index);
}

TEST_CASE("Test utils::string::erased_from", "[utils][utils::string]") {
    std::string empty("");
    REQUIRE(utils::string::erased_from(empty, ".").size() == 0);

    std::string test_1("     abcd     ");
    REQUIRE(utils::string::erased_from(test_1, "d") == "     abc");

    std::string test_2("\t\t\t\tabcd\t\t");
    REQUIRE(utils::string::erased_from(test_2, "\ta") == "\t\t\t");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    REQUIRE(utils::string::erased_from(test_3, "dcba") == "\n\n\n\nabcd\n\n\n");

    std::string test_4(alphabet_lower);
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size() - 1);
    const std::string erase_char(1, test_4[erase_index]);

    REQUIRE(utils::string::erased_from(test_4, erase_char).size() == erase_index);
}

TEST_CASE("Test utils::string::erase_to", "[utils][utils::string]") {
    std::string empty("");
    utils::string::erase_to(empty, ".");
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcd     ");
    utils::string::erase_to(test_1, "d");
    REQUIRE(test_1 == "d     ");

    std::string test_2("\t\t\t\tabcd\t\t");
    utils::string::erase_to(test_2, "\ta");
    REQUIRE(test_2 == "\tabcd\t\t");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    utils::string::erase_to(test_3, "dcba");
    REQUIRE(test_3 == "\n\n\n\nabcd\n\n\n");

    std::string test_4(alphabet_lower);
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size() - 1);
    const std::string erase_char(1, test_4[erase_index]);

    utils::string::erase_to(test_4, erase_char);
    REQUIRE(test_4.size() == alphabet_lower.size() - erase_index);
}

TEST_CASE("Test utils::string::erased_to", "[utils][utils::string]") {
    std::string empty("");
    REQUIRE(utils::string::erased_to(empty, ".").size() == 0);

    std::string test_1("     abcd     ");
    REQUIRE(utils::string::erased_to(test_1, "d") == "d     ");

    std::string test_2("\t\t\t\tabcd\t\t");
    REQUIRE(utils::string::erased_to(test_2, "\ta") == "\tabcd\t\t");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    REQUIRE(utils::string::erased_to(test_3, "dcba") == "\n\n\n\nabcd\n\n\n");

    std::string test_4(alphabet_lower);
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size() - 1);
    const std::string erase_char(1, test_4[erase_index]);

    REQUIRE(utils::string::erased_to(test_4, erase_char).size() == alphabet_lower.size() - erase_index);
}

TEST_CASE("Test utils::string::to_upper", "[utils][utils::string]") {
    std::string empty ("");
    std::string test_1(" \t\n\rabcd\t-=_+[]'#;/,.<>?:@~)");
    std::string test_2(alphabet_lower);
    std::string test_3("üêéè");

    std::wstring wempty (L"");
    std::wstring wtest_1(L" \t\n\rabcd\t-=_+[]'#;/,.<>?:@~)");
    std::wstring wtest_2(utils::string::to_wstring(test_2));
    std::wstring wtest_3(L"üêéè");

    utils::string::to_upper(empty);
    utils::string::to_upper(wempty);
    REQUIRE(empty.size()  == 0);
    REQUIRE(wempty.size() == 0);

    utils::string::to_upper(test_1);
    utils::string::to_upper(wtest_1);
    REQUIRE(test_1  == " \t\n\rABCD\t-=_+[]'#;/,.<>?:@~)");
    REQUIRE(wtest_1 == L" \t\n\rABCD\t-=_+[]'#;/,.<>?:@~)");

    utils::string::to_upper(test_2);
    utils::string::to_upper(wtest_2);
    REQUIRE(test_2.size()  == alphabet_lower.size());
    REQUIRE(test_2  == alphabet_upper);
    REQUIRE(wtest_2.size() == alphabet_lower.size());
    REQUIRE(wtest_2 == utils::string::to_wstring(alphabet_upper));

    utils::string::to_upper(test_3);
    utils::string::to_upper(wtest_3);
    CHECK_FALSE(test_3  == "ÜÊÉÈ");
    CHECK_FALSE(wtest_3 == L"ÜÊÉÈ");
}

TEST_CASE("Test utils::string::to_uppercase", "[utils][utils::string]") {
    const std::string empty ("");
    const std::string test_1(" \t\n\rabcd\t-=_+[]'#;/,.<>?:@~)");
    const std::string test_2(alphabet_lower);
    const std::string test_3("üêéè");

    const std::wstring wempty (L"");
    const std::wstring wtest_1(L" \t\n\rabcd\t-=_+[]'#;/,.<>?:@~)");
    const std::wstring wtest_2(utils::string::to_wstring(test_2));
    const std::wstring wtest_3(L"üêéè");

    REQUIRE(utils::string::to_uppercase(empty).size() == 0);
    REQUIRE(utils::string::to_uppercase(test_1) == " \t\n\rABCD\t-=_+[]'#;/,.<>?:@~)");
    REQUIRE(utils::string::to_uppercase(test_2) == alphabet_upper);
    CHECK_FALSE(utils::string::to_uppercase(test_3) == "ÜÊÉÈ");

    REQUIRE(utils::string::to_uppercase(wempty).size() == 0);
    REQUIRE(utils::string::to_uppercase(wtest_1) == L" \t\n\rABCD\t-=_+[]'#;/,.<>?:@~)");
    REQUIRE(utils::string::to_uppercase(wtest_2) == utils::string::to_wstring(alphabet_upper));
    CHECK_FALSE(utils::string::to_uppercase(wtest_3) == L"ÜÊÉÈ");
}

TEST_CASE("Test utils::string::to_lower", "[utils][utils::string]") {
    std::string empty ("");
    std::string test_1(" \t\n\rABCD\t-=_+[]'#;/,.<>?:@~)");
    std::string test_2(alphabet_upper);
    std::string test_3("ÜÊÉÈ");

    std::wstring wempty (L"");
    std::wstring wtest_1(L" \t\n\rABCD\t-=_+[]'#;/,.<>?:@~)");
    std::wstring wtest_2(utils::string::to_wstring(test_2));
    std::wstring wtest_3(L"ÜÊÉÈ");

    utils::string::to_lower(empty);
    utils::string::to_lower(wempty);
    REQUIRE(empty.size()  == 0);
    REQUIRE(wempty.size() == 0);

    utils::string::to_lower(test_1);
    utils::string::to_lower(wtest_1);
    REQUIRE(test_1  == " \t\n\rabcd\t-=_+[]'#;/,.<>?:@~)");
    REQUIRE(wtest_1 == L" \t\n\rabcd\t-=_+[]'#;/,.<>?:@~)");

    utils::string::to_lower(test_2);
    utils::string::to_lower(wtest_2);
    REQUIRE(test_2.size()  == alphabet_upper.size());
    REQUIRE(test_2  == alphabet_lower);
    REQUIRE(wtest_2.size() == alphabet_upper.size());
    REQUIRE(wtest_2 == utils::string::to_wstring(alphabet_lower));

    utils::string::to_lower(test_3);
    utils::string::to_lower(wtest_3);
    CHECK_FALSE(test_3  == "üêéè");
    CHECK_FALSE(wtest_3 == L"üêéè");
}

TEST_CASE("Test utils::string::to_lowercase", "[utils][utils::string]") {
    const std::string empty ("");
    const std::string test_1(" \t\n\rABCD\t-=_+[]'#;/,.<>?:@~)");
    const std::string test_2(alphabet_upper);
    const std::string test_3("ÜÊÉÈ");

    const std::wstring wempty (L"");
    const std::wstring wtest_1(L" \t\n\rABCD\t-=_+[]'#;/,.<>?:@~)");
    const std::wstring wtest_2(utils::string::to_wstring(test_2));
    const std::wstring wtest_3(L"ÜÊÉÈ");

    REQUIRE(utils::string::to_lowercase(empty).size() == 0);
    REQUIRE(utils::string::to_lowercase(test_1) == " \t\n\rabcd\t-=_+[]'#;/,.<>?:@~)");
    REQUIRE(utils::string::to_lowercase(test_2) == alphabet_lower);
    CHECK_FALSE(utils::string::to_lowercase(test_3) == "üêéè");

    REQUIRE(utils::string::to_lowercase(wempty).size() == 0);
    REQUIRE(utils::string::to_lowercase(wtest_1) == L" \t\n\rabcd\t-=_+[]'#;/,.<>?:@~)");
    REQUIRE(utils::string::to_lowercase(wtest_2) == utils::string::to_wstring(alphabet_lower));
    CHECK_FALSE(utils::string::to_lowercase(wtest_3) == L"üêéè");
}

TEST_CASE("Test utils::string::erase_consecutive", "[utils][utils::string]") {
    std::string empty("");
    utils::string::erase_consecutive(empty, '.');
    REQUIRE(empty.size() == 0);

    std::string test_1("     aabcd     ");
    utils::string::erase_consecutive(test_1, 'a');
    REQUIRE(test_1 == "     abcd     ");
    utils::string::erase_consecutive(test_1, 'b');
    REQUIRE(test_1 == "     abcd     ");
    utils::string::erase_consecutive(test_1, ' ');
    REQUIRE(test_1 == " abcd ");

    std::string test_2, charstr;

    test_2 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                             'a', 'z');

    charstr = std::string(utils::random::Random::get<size_t>(1, 10), '.');
    test_2 += charstr;

    test_2 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                             'a', 'z');

    const size_t prelen(test_2.size());
    utils::string::erase_consecutive(test_2, '.');
    // Only repeated are removed, unique is kept => +1
    REQUIRE(test_2.size() == prelen - charstr.size() + 1);
}

TEST_CASE("Test utils::string::replace_all", "[utils][utils::string]") {
    std::string empty("");
    utils::string::replace_all(empty, "", "");
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcad     ");
    utils::string::replace_all(test_1, 'a');
    REQUIRE(test_1 == "     bcd     ");
    utils::string::replace_all(test_1, "bc", "cb");
    REQUIRE(test_1 == "     cbd     ");
    utils::string::replace_all(test_1, ' ', "<>");
    REQUIRE(test_1 == "<><><><><>cbd<><><><><>");
    utils::string::replace_all(test_1, '<', '?');
    REQUIRE(test_1 == "?>?>?>?>?>cbd?>?>?>?>?>");

    std::string test_2, part1, charstr, part2;

    part1 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                            'a', 'z');
    charstr = std::string(utils::random::Random::get<size_t>(1, 10), '.');
    part2 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                            'a', 'z');

    test_2 = part1 + charstr + part2;

    utils::string::replace_all(test_2, '.', '=');
    REQUIRE(test_2 == part1 + std::string(charstr.size(), '=') + part2);
}

TEST_CASE("Test utils::string::erase_all", "[utils][utils::string]") {
    std::string empty("");
    utils::string::erase_all(empty, "");
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcad     ");
    utils::string::erase_all(test_1, 'a');
    REQUIRE(test_1 == "     bcd     ");
    utils::string::erase_all(test_1, "bc");
    REQUIRE(test_1 == "     d     ");
    utils::string::erase_all(test_1, ' ');
    REQUIRE(test_1 == "d");
    test_1 += std::string(5, '0');
    utils::string::erase_all(test_1, '0');
    REQUIRE(test_1 == "d");

    std::string test_2, part1, charstr, part2;

    part1 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                            'a', 'z');
    charstr = std::string(utils::random::Random::get<size_t>(1, 10), '.');
    part2 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                            'a', 'z');

    test_2 = part1 + charstr + part2;

    utils::string::erase_all(test_2, '.');
    REQUIRE(test_2 == part1 + part2);
}

TEST_CASE("Test utils::string::to_wstring", "[utils][utils::string]") {
    std::wstring test = utils::string::to_wstring("\0");
    REQUIRE(test == L"");
    REQUIRE(std::wcscmp(test.data(), L"\0") == 0);

    test = utils::string::to_wstring("AbCdE\0");
    REQUIRE(std::wcscmp(test.data(), L"AbCdE\0") == 0);

    test = utils::string::to_wstring(std::string("\xEF\xBF\xBF"));
    REQUIRE(std::wcscmp(test.data(), L"\xFFFF") == 0);

    test = utils::string::to_wstring(std::string("\xE2\x82\xAC"));
    REQUIRE(std::wcscmp(test.data(), L"\x20ac") == 0);
}

TEST_CASE("Test utils::string::to_string", "[utils][utils::string]") {
    std::string test = utils::string::to_string(L"\0");
    REQUIRE(test == "");
    REQUIRE(std::strcmp(test.data(), "\0") == 0);

    test = utils::string::to_string(L"AbCdE\0");
    REQUIRE(std::strcmp(test.data(), "AbCdE\0") == 0);

    test = utils::string::to_string(std::wstring(L"\xFFFF"));
    REQUIRE(std::strcmp(test.data(), "\xEF\xBF\xBF") == 0);

    test = utils::string::to_string(std::wstring(L"\x20ac"));
    REQUIRE(std::strcmp(test.data(), "\xE2\x82\xAC") == 0);
}

TEST_CASE("Test utils::string::quote", "[utils][utils::string]") {
    std::string em = "";
    std::string q1 = ".";
    std::string q2 = "'Hello'";
    std::string q3 = "Hello";

    utils::string::quote(em);
    CHECK(em == "\"\"");
    utils::string::quote(em);
    CHECK(em == "\"\"");
    utils::string::quote(q1);
    CHECK(q1 == "\".\"");
    utils::string::quote(q2, '\'');
    CHECK(q2 == "'Hello'");
    utils::string::quote(q2);
    CHECK(q2 == "\"'Hello'\"");
    utils::string::quote(q3);
    CHECK(q3 == "\"Hello\"");
    utils::string::quote(q3, '_');
    CHECK(q3 == "_\"Hello\"_");
    utils::string::quote(q3, "**");
    CHECK(q3 == "**_\"Hello\"_**");
}

TEST_CASE("Test utils::string::quoted", "[utils][utils::string]") {
    const std::string em = "";
    const std::string q1 = ".";
    const std::string q2 = "'Hello'";
    const std::string q3 = "Hello";

    CHECK(utils::string::quoted(em)       == "\"\"");
    CHECK(utils::string::quoted(utils::string::quoted(em)) == "\"\"");
    CHECK(utils::string::quoted(q1)       == "\".\"");
    CHECK(utils::string::quoted(q2, '\'') == "'Hello'");
    CHECK(utils::string::quoted(q2)       == "\"'Hello'\"");
    CHECK(utils::string::quoted(q3)       == "\"Hello\"");
    CHECK(utils::string::quoted(q3, '_')  == "_Hello_");
    CHECK(utils::string::quoted(q3, "**") == "**Hello**");
}

TEST_CASE("Test utils::string::extract_quoted", "[utils][utils::string]") {
    std::vector<std::string_view> list;
    std::string quoted;

    quoted = "'Hello', 'World'";
    utils::string::extract_quoted(list, quoted, '\'');
    REQUIRE(list.size() == 2);
    REQUIRE((list[0] == "Hello" && list[1] == "World"));

    quoted = "'Hello', 'Wo\'rld'";
    utils::string::extract_quoted(list, quoted, '\'');
    REQUIRE(list.size() == 2);
    REQUIRE((list[0] == "Hello" && list[1] == "Wo"));

    quoted = "''";
    utils::string::extract_quoted(list, quoted, '\'');
    REQUIRE(list.size() == 1);
    REQUIRE(list[0] == "");

    quoted = "' '";
    utils::string::extract_quoted(list, quoted, '\'');
    REQUIRE(list.size() == 1);
    REQUIRE(list[0] == " ");

    quoted = "abcdef";
    utils::string::extract_quoted(list, quoted, '\'');
    REQUIRE(list.size() == 0);

    quoted = "\"Hello\", \"World\"";
    utils::string::extract_quoted(list, quoted);
    REQUIRE(list.size() == 2);
    REQUIRE((list[0] == "Hello" && list[1] == "World"));

    int checkidx = 0;
    utils::string::for_each_quoted([&](const std::string_view& sv){
        CHECK(list[checkidx] == sv);
        checkidx++;
    }, quoted);
}

TEST_CASE("Test utils::string::join", "[utils][utils::string]") {
    REQUIRE(utils::string::join({}) == "");
    REQUIRE(utils::string::join({"test"}) == "test");
    REQUIRE(utils::string::join({"test", "tset"}, ",")  == "test,tset");
    REQUIRE(utils::string::join({"test", "tset"}, ", ") == "test, tset");
    REQUIRE(utils::string::join({"test", "tset"}, '.')  == "test.tset");
    REQUIRE(utils::string::join({"test", "tset"}, "")   == "testtset");
    REQUIRE(utils::string::join({"", "tset"}, "+")      == "+tset");
    REQUIRE(utils::string::join({"test", ""}, "+")      == "test+");
    REQUIRE(utils::string::join({"1", "2", "3", "4"}, " * ") == "1 * 2 * 3 * 4");
    REQUIRE(utils::string::join({"1", "2", "3", "4"}, '*')   == "1*2*3*4");

    REQUIRE(utils::string::join(std::vector<char>{}, ' ') == "");
    REQUIRE(utils::string::join(std::vector<char>{'x'}, '\0') == "x");
    REQUIRE(utils::string::join(std::vector<char>{'x'}, '_') == "x");
    REQUIRE(utils::string::join(std::vector<char>{'x', 'y'}, '_') == "x_y");
    REQUIRE(utils::string::join(std::vector<char>{'1', '2', '3', '4'}, '*') == "1*2*3*4");

    std::string ds = utils::string::join(std::vector<uint8_t>{'a', 'b', '\0', '\"'}, '\0');
    CHECK(int(ds[0]) == int('a'));
    CHECK(int(ds[1]) == int('b'));
    CHECK(int(ds[2]) == int('\0'));
    CHECK(int(ds[3]) == int('\"'));
}

TEST_CASE("Test utils::string::split", "[utils][utils::string]") {
    std::vector<std::string_view> splitted;

    utils::string::split(splitted, "");
    REQUIRE(splitted.size() == 1);

    utils::string::split(splitted, "a,b,c", ',');
    REQUIRE(splitted.size() == 3);
    REQUIRE((splitted[0] == "a" && splitted[1] == "b" && splitted[2] == "c"));
    utils::string::split(splitted, "a,b,c,", ',');
    REQUIRE(splitted.size() == 4);
    REQUIRE((splitted[0] == "a" && splitted[1] == "b" && splitted[2] == "c" && splitted[3] == ""));
    utils::string::split(splitted, ",a,b,c,", ',');
    REQUIRE(splitted.size() == 5);
    REQUIRE((splitted[0] == "" && splitted[1] == "a" && splitted[2] == "b" && splitted[3] == "c" && splitted[4] == ""));

    utils::string::split(splitted, "a\\b\\c", '\\');
    REQUIRE(splitted.size() == 3);
    REQUIRE((splitted[0] == "a" && splitted[1] == "b" && splitted[2] == "c"));

    utils::string::split(splitted, ",,a ,\tb\n, c ;", ','); // char
    REQUIRE(splitted.size() == 5);
    CHECK(splitted[0] == "");
    CHECK(splitted[1] == "");
    CHECK(splitted[2] == "a ");
    CHECK(splitted[3] == "\tb\n");
    CHECK(splitted[4] == " c ;");
    utils::string::split(splitted, ",,a ,\tb\n, c ;", ","); // char*
    REQUIRE(splitted.size() == 5);
    CHECK(splitted[0] == "");
    CHECK(splitted[1] == "");
    CHECK(splitted[2] == "a ");
    CHECK(splitted[3] == "\tb\n");
    CHECK(splitted[4] == " c ;");

    utils::string::split(splitted, ",,a ,\tb\n, c ;", ',', 4);
    REQUIRE(splitted.size() == 5);
    CHECK(splitted[0] == "");
    CHECK(splitted[4] == " c ;");

    utils::string::split(splitted, ",,a ,\tb\n, c ;", ',', 0);
    REQUIRE(splitted.size() == 1);
    CHECK(splitted[0] == ",,a ,\tb\n, c ;");

    utils::string::split(splitted, ",,a ,\tb\n, c ;", ',', 1);
    REQUIRE(splitted.size() == 2);
    CHECK(splitted[0] == "");
    CHECK(splitted[1] == ",a ,\tb\n, c ;");

    utils::string::split(splitted, ",,a ,\tb\n, c ;", ',', 3);
    REQUIRE(splitted.size() == 4);
    CHECK(splitted[0] == "");
    CHECK(splitted[1] == "");
    CHECK(splitted[2] == "a ");
    CHECK(splitted[3] == "\tb\n, c ;");

    utils::string::split(splitted, "**1****2**", "**"); // char*
    REQUIRE(splitted.size() == 5);
    CHECK(splitted[0] == "");
    CHECK(splitted[1] == "1");
    CHECK(splitted[2] == "");
    CHECK(splitted[3] == "2");
    CHECK(splitted[4] == "");

    auto split_iter = splitted.begin();
    utils::string::for_each_splitted([&](const std::string_view& sv){
        CHECK(*split_iter == sv);
        split_iter++;
    }, "**1****2**", "**");
}

TEST_CASE("Test utils::string::rsplit", "[utils][utils::string]") {
    std::vector<std::string_view> splitted;

    utils::string::rsplit(splitted, "");
    REQUIRE(splitted.size() == 1);

    utils::string::rsplit(splitted, "a,b,c", ',');
    REQUIRE(splitted.size() == 3);
    REQUIRE((splitted[0] == "a" && splitted[1] == "b" && splitted[2] == "c"));
    utils::string::rsplit(splitted, "a,b,c,", ',');
    REQUIRE(splitted.size() == 4);
    REQUIRE((splitted[0] == "a" && splitted[1] == "b" && splitted[2] == "c" && splitted[3] == ""));
    utils::string::rsplit(splitted, ",a,b,c,", ',');
    REQUIRE(splitted.size() == 5);
    REQUIRE((splitted[0] == "" && splitted[1] == "a" && splitted[2] == "b" && splitted[3] == "c" && splitted[4] == ""));

    utils::string::rsplit(splitted, "a\\b\\c", '\\');
    REQUIRE(splitted.size() == 3);
    REQUIRE((splitted[0] == "a" && splitted[1] == "b" && splitted[2] == "c"));

    utils::string::rsplit(splitted, ",,a ,\tb\n, c ;", ','); // char
    REQUIRE(splitted.size() == 5);
    CHECK(splitted[0] == "");
    CHECK(splitted[1] == "");
    CHECK(splitted[2] == "a ");
    CHECK(splitted[3] == "\tb\n");
    CHECK(splitted[4] == " c ;");
    utils::string::rsplit(splitted, ",,a ,\tb\n, c ;", ","); // char*
    REQUIRE(splitted.size() == 5);
    CHECK(splitted[0] == "");
    CHECK(splitted[1] == "");
    CHECK(splitted[2] == "a ");
    CHECK(splitted[3] == "\tb\n");
    CHECK(splitted[4] == " c ;");

    utils::string::rsplit(splitted, ",,a ,\tb\n, c ;", ',', 4);
    REQUIRE(splitted.size() == 5);
    CHECK(splitted[0] == ",");

    utils::string::rsplit(splitted, ",,a ,\tb\n, c ;", ',', 0);
    REQUIRE(splitted.size() == 1);
    CHECK(splitted[0] == ",,a ,\tb\n, c ;");

    utils::string::rsplit(splitted, ",,a ,\tb\n, c ;", ',', 1);
    REQUIRE(splitted.size() == 2);
    CHECK(splitted[0] == ",,a ,\tb\n");
    CHECK(splitted[1] == " c ;");

    utils::string::rsplit(splitted, ",,a ,\tb\n, c ;", ',', 3);
    REQUIRE(splitted.size() == 4);
    CHECK(splitted[0] == ",");
    CHECK(splitted[1] == "a ");
    CHECK(splitted[2] == "\tb\n");
    CHECK(splitted[3] == " c ;");

    utils::string::rsplit(splitted, "**1****2**", "**"); // char*
    REQUIRE(splitted.size() == 5);
    CHECK(splitted[0] == "");
    CHECK(splitted[1] == "1");
    CHECK(splitted[2] == "");
    CHECK(splitted[3] == "2");
    CHECK(splitted[4] == "");

    auto split_iter = splitted.rbegin();
    utils::string::for_each_rsplitted([&](const std::string_view& sv){
        CHECK(*split_iter == sv);
        split_iter++;
    }, "**1****2**", "**");
}

TEST_CASE("Test utils::string::format", "[utils][utils::string]") {
    std::stringstream ss;
    std::string test;

    std::stringstream().swap(ss);
    test = utils::string::format("");
    ss  << utils::string::format("");
    REQUIRE(test     == "");
    REQUIRE(ss.str() == "");

    std::stringstream().swap(ss);
    test = utils::string::format("abc");
    ss  << utils::string::format("abc");
    REQUIRE(test     == "abc");
    REQUIRE(ss.str() == "abc");

    std::stringstream().swap(ss);
    test = utils::string::format("\r\n%d\t%%", 42);
    ss  << utils::string::format("\r\n%d\t%%", 42);
    REQUIRE(test     == "\r\n42\t%");
    REQUIRE(ss.str() == "\r\n42\t%");

    std::stringstream().swap(ss);
    test = utils::string::format("0x%08X", 0xBEEF);
    ss  << utils::string::format("0x%08X", 0xBEEF);
    REQUIRE(test     == "0x0000BEEF");
    REQUIRE(ss.str() == "0x0000BEEF");

    std::stringstream().swap(ss);
    test = utils::string::format("%c%c%c%s%c", 'a', 'b', 'c', "Haha", '\n');
    ss  << utils::string::format("%c%c%c%s%c", 'a', 'b', 'c', "Haha", '\n');
    REQUIRE(test     == "abcHaha\n");
    REQUIRE(ss.str() == "abcHaha\n");

    std::stringstream().swap(ss);
    test = utils::string::format("%*d", 4, 2);
    ss  << utils::string::format("%*d", 4, 2);
    REQUIRE(test     == "   2");
    REQUIRE(ss.str() == "   2");

    std::stringstream().swap(ss);
    const std::string frmt("%d%d%d");
    const int x = 1, y = 2, z = 3;
    test = utils::string::format(frmt + "%c", x, y, z, '\n');
    ss  << utils::string::format(frmt + "%c", x, y, z, '\n');
    REQUIRE(test     == "123\n");
    REQUIRE(ss.str() == "123\n");
}

TEST_CASE("Test utils::string::is_base64", "[utils][utils::string]") {
    REQUIRE(utils::string::is_base64(""        ));
    REQUIRE(utils::string::is_base64("Zg=="    ));
    REQUIRE(utils::string::is_base64("Zm8="    ));
    REQUIRE(utils::string::is_base64("Zm9v"    ));
    REQUIRE(utils::string::is_base64("Zm9vYg=="));
    REQUIRE(utils::string::is_base64("Zm9vYmE="));
    REQUIRE(utils::string::is_base64("Zm9vYmFy"));

    REQUIRE(utils::string::is_base64("AA=="));
    REQUIRE(utils::string::is_base64("AAA="));
    REQUIRE(utils::string::is_base64("AAAA"));
    REQUIRE(utils::string::is_base64("/w=="));
    REQUIRE(utils::string::is_base64("//8="));
    REQUIRE(utils::string::is_base64("////"));
    REQUIRE(utils::string::is_base64("/+8="));

    REQUIRE_FALSE(utils::string::is_base64("^"    ));
    REQUIRE_FALSE(utils::string::is_base64("A"    ));
    REQUIRE_FALSE(utils::string::is_base64("A^"   ));
    REQUIRE_FALSE(utils::string::is_base64("AA"   ));
    REQUIRE_FALSE(utils::string::is_base64("AA="  ));
    REQUIRE_FALSE(utils::string::is_base64("AA==="));
    REQUIRE_FALSE(utils::string::is_base64("AAA"  ));
    REQUIRE_FALSE(utils::string::is_base64("AAA^" ));
    REQUIRE_FALSE(utils::string::is_base64("AA======"));
}

TEST_CASE("Test utils::string::to_base64", "[utils][utils::string]") {
    std::string enc, dec, str;

    REQUIRE(utils::string::to_base64(""      ) == "");
    REQUIRE(utils::string::to_base64("f"     ) == "Zg==");
    REQUIRE(utils::string::to_base64("fo"    ) == "Zm8=");
    REQUIRE(utils::string::to_base64("foo"   ) == "Zm9v");
    REQUIRE(utils::string::to_base64("foob"  ) == "Zm9vYg==");
    REQUIRE(utils::string::to_base64("fooba" ) == "Zm9vYmE=");
    REQUIRE(utils::string::to_base64("foobar") == "Zm9vYmFy");

    REQUIRE(utils::string::to_base64(reinterpret_cast<const uint8_t*>("\0")          , 1) == "AA==");
    REQUIRE(utils::string::to_base64(reinterpret_cast<const uint8_t*>("\0\0")        , 2) == "AAA=");
    REQUIRE(utils::string::to_base64(reinterpret_cast<const uint8_t*>("\0\0\0")      , 3) == "AAAA");
    REQUIRE(utils::string::to_base64(reinterpret_cast<const uint8_t*>("\377")        , 1) == "/w==");
    REQUIRE(utils::string::to_base64(reinterpret_cast<const uint8_t*>("\377\377")    , 2) == "//8=");
    REQUIRE(utils::string::to_base64(reinterpret_cast<const uint8_t*>("\377\377\377"), 3) == "////");
    REQUIRE(utils::string::to_base64(reinterpret_cast<const uint8_t*>("\xff\xef")    , 2) == "/+8=");

    str = "Hello World!!";
    enc = utils::string::to_base64(str);
    dec = utils::string::from_base64(enc);
    REQUIRE((enc.size() > 0 && enc.back() == '='));
    REQUIRE(utils::string::is_base64(enc));
    REQUIRE(dec == str);

    for (int j = 0; j < 5; ++j) {
        str = utils::random::generate_string<>(1024);
        enc = utils::string::to_base64(str);
        dec = utils::string::from_base64(enc);
        REQUIRE(utils::string::is_base64(enc));
        REQUIRE(dec == str);
    }
}

TEST_CASE("Test utils::string::from_base64", "[utils][utils::string]") {
    REQUIRE(utils::string::from_base64(""        ) == ""      );
    REQUIRE(utils::string::from_base64("Zg=="    ) == "f"     );
    REQUIRE(utils::string::from_base64("Zm8="    ) == "fo"    );
    REQUIRE(utils::string::from_base64("Zm9v"    ) == "foo"   );
    REQUIRE(utils::string::from_base64("Zm9vYg==") == "foob"  );
    REQUIRE(utils::string::from_base64("Zm9vYmE=") == "fooba" );
    REQUIRE(utils::string::from_base64("Zm9vYmFy") == "foobar");

    REQUIRE(std::strcmp(utils::string::from_base64("AA==").c_str(), "\0"          ) == 0);
    REQUIRE(std::strcmp(utils::string::from_base64("AAA=").c_str(), "\0\0"        ) == 0);
    REQUIRE(std::strcmp(utils::string::from_base64("AAAA").c_str(), "\0\0\0"      ) == 0);
    REQUIRE(std::strcmp(utils::string::from_base64("/w==").c_str(), "\377"        ) == 0);
    REQUIRE(std::strcmp(utils::string::from_base64("//8=").c_str(), "\377\377"    ) == 0);
    REQUIRE(std::strcmp(utils::string::from_base64("////").c_str(), "\377\377\377") == 0);
    REQUIRE(std::strcmp(utils::string::from_base64("/+8=").c_str(), "\xff\xef"    ) == 0);

    CHECK_THROWS_AS(utils::string::from_base64("^"    ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::from_base64("A"    ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::from_base64("A^"   ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::from_base64("AA"   ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::from_base64("AA="  ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::from_base64("AA===") == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::from_base64("AAA"  ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::from_base64("AAA^" ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::from_base64("AA======") == "", utils::exceptions::ConversionException);
}

#endif
