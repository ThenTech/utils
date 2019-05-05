#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/catch.hpp"

#include "../utils_lib/utils_string.hpp"

#include "../utils_lib/utils_random.hpp"


#define SPACES { ' ', '\t', '\n' }
constexpr std::string_view alphabet_lower = "abcdefghijklmnopqrstuvwxyz";
constexpr std::string_view alphabet_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

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

    for (int i = utils::random::Random::get<int>(1, 10); i > 0; i--) {
        spacesstr += utils::random::Random::get<char>(SPACES);
    }

    for (int i = utils::random::Random::get<int>(1, 10); i > 0; i--) {
        charstr += utils::random::Random::get<char>('a', 'z');
    }

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

    for (int i = utils::random::Random::get<int>(1, 10); i > 0; i--) {
        spacesstr += utils::random::Random::get<char>(SPACES);
    }

    for (int i = utils::random::Random::get<int>(1, 10); i > 0; i--) {
        charstr += utils::random::Random::get<char>('a', 'z');
    }

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

    for (int i = utils::random::Random::get<int>(1, 10); i > 0; i--) {
        spacesstr += utils::random::Random::get<char>(SPACES);
    }

    for (int i = utils::random::Random::get<int>(1, 10); i > 0; i--) {
        charstr += utils::random::Random::get<char>('a', 'z');
    }

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

    for (int i = utils::random::Random::get<int>(1, 10); i > 0; i--) {
        spacesstr += utils::random::Random::get<char>(SPACES);
    }

    for (int i = utils::random::Random::get<int>(1, 10); i > 0; i--) {
        charstr += utils::random::Random::get<char>('a', 'z');
    }

    REQUIRE(utils::string::trimmed(spacesstr).size() == 0);
    REQUIRE(utils::string::trimmed(spacesstr + charstr + spacesstr) == charstr);
    REQUIRE(utils::string::trimmed(charstr) == charstr);
}

TEST_CASE("Test utils::string::strEraseFrom", "[utils][utils::string]") {
    std::string empty("");
    utils::string::strEraseFrom(empty, ".");
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcd     ");
    utils::string::strEraseFrom(test_1, "d");
    REQUIRE(test_1 == "     abc");

    std::string test_2("\t\t\t\tabcd\t\t");
    utils::string::strEraseFrom(test_2, "\ta");
    REQUIRE(test_2 == "\t\t\t");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    utils::string::strEraseFrom(test_3, "dcba");
    REQUIRE(test_3 == "\n\n\n\nabcd\n\n\n");

    std::string test_4(alphabet_lower);
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size());
    const std::string erase_char(1, test_4[erase_index]);

    utils::string::strEraseFrom(test_4, erase_char);
    REQUIRE(test_4.size() == erase_index);
}

TEST_CASE("Test utils::string::strErasedFrom", "[utils][utils::string]") {
    std::string empty("");
    REQUIRE(utils::string::strErasedFrom(empty, ".").size() == 0);

    std::string test_1("     abcd     ");
    REQUIRE(utils::string::strErasedFrom(test_1, "d") == "     abc");

    std::string test_2("\t\t\t\tabcd\t\t");
    REQUIRE(utils::string::strErasedFrom(test_2, "\ta") == "\t\t\t");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    REQUIRE(utils::string::strErasedFrom(test_3, "dcba") == "\n\n\n\nabcd\n\n\n");

    std::string test_4(alphabet_lower);
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size());
    const std::string erase_char(1, test_4[erase_index]);

    REQUIRE(utils::string::strErasedFrom(test_4, erase_char).size() == erase_index);
}

TEST_CASE("Test utils::string::strEraseTo", "[utils][utils::string]") {
    std::string empty("");
    utils::string::strEraseTo(empty, ".");
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcd     ");
    utils::string::strEraseTo(test_1, "d");
    REQUIRE(test_1 == "d     ");

    std::string test_2("\t\t\t\tabcd\t\t");
    utils::string::strEraseTo(test_2, "\ta");
    REQUIRE(test_2 == "\tabcd\t\t");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    utils::string::strEraseTo(test_3, "dcba");
    REQUIRE(test_3 == "\n\n\n\nabcd\n\n\n");

    std::string test_4(alphabet_lower);
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size());
    const std::string erase_char(1, test_4[erase_index]);

    utils::string::strEraseTo(test_4, erase_char);
    REQUIRE(test_4.size() == alphabet_lower.size() - erase_index);
}

TEST_CASE("Test utils::string::strErasedTo", "[utils][utils::string]") {
    std::string empty("");
    REQUIRE(utils::string::strErasedTo(empty, ".").size() == 0);

    std::string test_1("     abcd     ");
    REQUIRE(utils::string::strErasedTo(test_1, "d") == "d     ");

    std::string test_2("\t\t\t\tabcd\t\t");
    REQUIRE(utils::string::strErasedTo(test_2, "\ta") == "\tabcd\t\t");

    std::string test_3("\n\n\n\nabcd\n\n\n");
    REQUIRE(utils::string::strErasedTo(test_3, "dcba") == "\n\n\n\nabcd\n\n\n");

    std::string test_4(alphabet_lower);
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size());
    const std::string erase_char(1, test_4[erase_index]);

    REQUIRE(utils::string::strErasedTo(test_4, erase_char).size() == alphabet_lower.size() - erase_index);
}

TEST_CASE("Test utils::string::strToUpper", "[utils][utils::string]") {
    std::string empty("");
    utils::string::strToUpper(empty);
    REQUIRE(empty.size() == 0);

    std::string test_1(" \t\n\rabcd\t-=_+[]'#;/,.<>?:@~");
    utils::string::strToUpper(test_1);
    REQUIRE(test_1 == " \t\n\rABCD\t-=_+[]'#;/,.<>?:@~");

    std::string test_2(alphabet_lower);
    utils::string::strToUpper(test_2);
    REQUIRE(test_2.size() == alphabet_lower.size());
    REQUIRE(test_2 == alphabet_upper);

    std::string test_3("üêéè");
    utils::string::strToUpper(test_3);
    CHECK_FALSE(test_3 == "ÜÊÉÈ");
}

TEST_CASE("Test utils::string::strToUppercase", "[utils][utils::string]") {
    std::string empty("");
    REQUIRE(utils::string::strToUppercase(empty).size() == 0);

    std::string test_1(" \t\n\rabcd\t-=_+[]'#;/,.<>?:@~");
    REQUIRE(utils::string::strToUppercase(test_1) == " \t\n\rABCD\t-=_+[]'#;/,.<>?:@~");

    std::string test_2(alphabet_lower);
    REQUIRE(utils::string::strToUppercase(test_2) == alphabet_upper);

    std::string test_3("üêéè");
    CHECK_FALSE(utils::string::strToUppercase(test_3) == "ÜÊÉÈ");
}

TEST_CASE("Test utils::string::strToLower", "[utils][utils::string]") {
    std::string empty("");
    utils::string::strToLower(empty);
    REQUIRE(empty.size() == 0);

    std::string test_1(" \t\n\rABCD\t-=_+[]'#;/,.<>?:@~");
    utils::string::strToLower(test_1);
    REQUIRE(test_1 == " \t\n\rabcd\t-=_+[]'#;/,.<>?:@~");

    std::string test_2(alphabet_upper);
    utils::string::strToLower(test_2);
    REQUIRE(test_2.size() == alphabet_upper.size());
    REQUIRE(test_2 == alphabet_lower);

    std::string test_3("ÜÊÉÈ");
    utils::string::strToLower(test_3);
    CHECK_FALSE(test_3 == "üêéè");
}

TEST_CASE("Test utils::string::strToLowercase", "[utils][utils::string]") {
    std::string empty("");
    REQUIRE(utils::string::strToLowercase(empty).size() == 0);

    std::string test_1(" \t\n\rABCD\t-=_+[]'#;/,.<>?:@~");
    REQUIRE(utils::string::strToLowercase(test_1) == " \t\n\rabcd\t-=_+[]'#;/,.<>?:@~");

    std::string test_2(alphabet_upper);
    REQUIRE(utils::string::strToLowercase(test_2) == alphabet_lower);

    std::string test_3("ÜÊÉÈ");
    CHECK_FALSE(utils::string::strToLowercase(test_3) == "üêéè");
}

TEST_CASE("Test utils::string::strHasChar", "[utils][utils::string]") {
    CHECK(false);
}

TEST_CASE("Test utils::string::strReplaceConsecutive", "[utils][utils::string]") {
    CHECK(false);
}

TEST_CASE("Test utils::string::strReplaceAll", "[utils][utils::string]") {
    CHECK(false);
}

TEST_CASE("Test utils::string::convert2WSTR", "[utils][utils::string]") {
    CHECK(false);
}

TEST_CASE("Test utils::string::split_on_borderchar", "[utils][utils::string]") {
    CHECK(false);
}

TEST_CASE("Test utils::string::format", "[utils][utils::string]") {
    CHECK(false);
}

#endif
