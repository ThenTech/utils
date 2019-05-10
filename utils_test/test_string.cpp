#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/catch.hpp"

#include "../utils_lib/utils_string.hpp"

#include "../utils_lib/utils_random.hpp"


static const std::string SPACES { ' ', '\t', '\n' };
static constexpr std::string_view alphabet_lower = "abcdefghijklmnopqrstuvwxyz";
static constexpr std::string_view alphabet_upper = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

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
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size() - 1);
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
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size() - 1);
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
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size() - 1);
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
    const size_t erase_index = utils::random::Random::get<size_t>(0, alphabet_lower.size() - 1);
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
    REQUIRE_FALSE(utils::string::strHasChar("", '.'));

    REQUIRE(utils::string::strHasChar("     abcd     ", 'd'));
    REQUIRE(utils::string::strHasChar("abcd\t\t", '\t'));
    REQUIRE(utils::string::strHasChar("\n\n\n\nabcd\n\n\n", 'd'));

    std::string test_1(alphabet_lower);
    for (int i = 5; i--;) {
        auto c = *utils::random::Random::get(test_1);
        REQUIRE(utils::string::strHasChar(test_1, c));
    }

    REQUIRE_FALSE(utils::string::strHasChar(test_1, 'A'));
}

TEST_CASE("Test utils::string::strEraseConsecutive", "[utils][utils::string]") {
    std::string empty("");
    utils::string::strEraseConsecutive(empty, '.');
    REQUIRE(empty.size() == 0);

    std::string test_1("     aabcd     ");
    utils::string::strEraseConsecutive(test_1, 'a');
    REQUIRE(test_1 == "     abcd     ");
    utils::string::strEraseConsecutive(test_1, 'b');
    REQUIRE(test_1 == "     abcd     ");
    utils::string::strEraseConsecutive(test_1, ' ');
    REQUIRE(test_1 == " abcd ");

    std::string test_2, charstr;

    test_2 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                             'a', 'z');

    charstr = std::string(utils::random::Random::get<size_t>(1, 10), '.');
    test_2 += charstr;

    test_2 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                             'a', 'z');

    const size_t prelen(test_2.size());
    utils::string::strEraseConsecutive(test_2, '.');
    // Only repeated are removed, unique is kept => +1
    REQUIRE(test_2.size() == prelen - charstr.size() + 1);
}

TEST_CASE("Test utils::string::strReplaceAll", "[utils][utils::string]") {
    std::string empty("");
    utils::string::strReplaceAll(empty, "", "");
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcad     ");
    utils::string::strReplaceAll(test_1, 'a');
    REQUIRE(test_1 == "     bcd     ");
    utils::string::strReplaceAll(test_1, "bc", "cb");
    REQUIRE(test_1 == "     cbd     ");
    utils::string::strReplaceAll(test_1, ' ', "<>");
    REQUIRE(test_1 == "<><><><><>cbd<><><><><>");
    utils::string::strReplaceAll(test_1, '<', '?');
    REQUIRE(test_1 == "?>?>?>?>?>cbd?>?>?>?>?>");

    std::string test_2, part1, charstr, part2;

    part1 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                            'a', 'z');
    charstr = std::string(utils::random::Random::get<size_t>(1, 10), '.');
    part2 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                            'a', 'z');

    test_2 = part1 + charstr + part2;

    utils::string::strReplaceAll(test_2, '.', '=');
    REQUIRE(test_2 == part1 + std::string(charstr.size(), '=') + part2);
}

TEST_CASE("Test utils::string::strEraseAll", "[utils][utils::string]") {
    std::string empty("");
    utils::string::strEraseAll(empty, "");
    REQUIRE(empty.size() == 0);

    std::string test_1("     abcad     ");
    utils::string::strEraseAll(test_1, 'a');
    REQUIRE(test_1 == "     bcd     ");
    utils::string::strEraseAll(test_1, "bc");
    REQUIRE(test_1 == "     d     ");
    utils::string::strEraseAll(test_1, ' ');
    REQUIRE(test_1 == "d");
    test_1 += std::string(5, '0');
    utils::string::strEraseAll(test_1, '0');
    REQUIRE(test_1 == "d");

    std::string test_2, part1, charstr, part2;

    part1 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                            'a', 'z');
    charstr = std::string(utils::random::Random::get<size_t>(1, 10), '.');
    part2 += utils::random::generate_string(utils::random::Random::get<size_t>(1, 10),
                                            'a', 'z');

    test_2 = part1 + charstr + part2;

    utils::string::strEraseAll(test_2, '.');
    REQUIRE(test_2 == part1 + part2);
}

TEST_CASE("Test utils::string::convert2WSTR", "[utils][utils::string]") {
    auto test = utils::memory::unique_t<wchar_t>(utils::string::convert2WSTR("\0"));

    REQUIRE(std::wstring(test.get()) == L"");
    REQUIRE(std::wcscmp(test.get(), L"\0") == 0);

    test = utils::memory::unique_t<wchar_t>(utils::string::convert2WSTR("AbCdE\0"));
    REQUIRE(std::wcscmp(test.get(), L"AbCdE\0") == 0);
}

TEST_CASE("Test utils::string::strExtractQuotedStrings", "[utils][utils::string]") {
    std::vector<std::string> list;
    std::string quoted;

    quoted = "'Hello', 'World'";
    utils::string::strExtractQuotedStrings(list, quoted);
    REQUIRE(list.size() == 2);
    REQUIRE((list[0] == "Hello" && list[1] == "World"));

    quoted = "'Hello', 'Wo\'rld'";
    utils::string::strExtractQuotedStrings(list, quoted);
    REQUIRE(list.size() == 2);
    REQUIRE((list[0] == "Hello" && list[1] == "Wo"));

    quoted = "''";
    utils::string::strExtractQuotedStrings(list, quoted);
    REQUIRE(list.size() == 0);

    quoted = "' '";
    utils::string::strExtractQuotedStrings(list, quoted);
    REQUIRE(list.size() == 1);
    REQUIRE(list[0] == " ");

    quoted = "abcdef";
    utils::string::strExtractQuotedStrings(list, quoted);
    REQUIRE(list.size() == 0);
}

TEST_CASE("Test utils::string::format", "[utils][utils::string]") {
    std::stringstream ss;
    std::string test;

    std::stringstream().swap(ss);
    test = utils::string::format("");
    utils::string::print_format(ss, "");
    REQUIRE(test     == "");
    REQUIRE(ss.str() == "");

    std::stringstream().swap(ss);
    test = utils::string::format("abc");
    utils::string::print_format(ss, "abc");
    REQUIRE(test     == "abc");
    REQUIRE(ss.str() == "abc");

    std::stringstream().swap(ss);
    test = utils::string::format("\r\n%d\t%%", 42);
    utils::string::print_format(ss, "\r\n%d\t%%", 42);
    REQUIRE(test     == "\r\n42\t%");
    REQUIRE(ss.str() == "\r\n42\t%");

    std::stringstream().swap(ss);
    test = utils::string::format("0x%08X", 0xBEEF);
    utils::string::print_format(ss, "0x%08X", 0xBEEF);
    REQUIRE(test     == "0x0000BEEF");
    REQUIRE(ss.str() == "0x0000BEEF");

    std::stringstream().swap(ss);
    test = utils::string::format("%c%c%c%s%c", 'a', 'b', 'c', "Haha", '\n');
    utils::string::print_format(ss, "%c%c%c%s%c", 'a', 'b', 'c', "Haha", '\n');
    REQUIRE(test     == "abcHaha\n");
    REQUIRE(ss.str() == "abcHaha\n");

    std::stringstream().swap(ss);
    test = utils::string::format("%*d", 4, 2);
    utils::string::print_format(ss, "%*d", 4, 2);
    REQUIRE(test     == "   2");
    REQUIRE(ss.str() == "   2");

    std::stringstream().swap(ss);
    const std::string frmt("%d%d%d");
    const int x = 1, y = 2, z = 3;
    test = utils::string::format(frmt + "%c", x, y, z, '\n');
    utils::string::print_format(ss, frmt + "%c", x, y, z, '\n');
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
}

TEST_CASE("Test utils::string::base64_encode", "[utils][utils::string]") {
    std::string enc, dec, str;

    REQUIRE(utils::string::base64_encode(""      ) == "");
    REQUIRE(utils::string::base64_encode("f"     ) == "Zg==");
    REQUIRE(utils::string::base64_encode("fo"    ) == "Zm8=");
    REQUIRE(utils::string::base64_encode("foo"   ) == "Zm9v");
    REQUIRE(utils::string::base64_encode("foob"  ) == "Zm9vYg==");
    REQUIRE(utils::string::base64_encode("fooba" ) == "Zm9vYmE=");
    REQUIRE(utils::string::base64_encode("foobar") == "Zm9vYmFy");

    REQUIRE(utils::string::base64_encode(reinterpret_cast<const uint8_t*>("\0")          , 1) == "AA==");
    REQUIRE(utils::string::base64_encode(reinterpret_cast<const uint8_t*>("\0\0")        , 2) == "AAA=");
    REQUIRE(utils::string::base64_encode(reinterpret_cast<const uint8_t*>("\0\0\0")      , 3) == "AAAA");
    REQUIRE(utils::string::base64_encode(reinterpret_cast<const uint8_t*>("\377")        , 1) == "/w==");
    REQUIRE(utils::string::base64_encode(reinterpret_cast<const uint8_t*>("\377\377")    , 2) == "//8=");
    REQUIRE(utils::string::base64_encode(reinterpret_cast<const uint8_t*>("\377\377\377"), 3) == "////");
    REQUIRE(utils::string::base64_encode(reinterpret_cast<const uint8_t*>("\xff\xef")    , 2) == "/+8=");

    str = "Hello World!!";
    enc = utils::string::base64_encode(str);
    dec = utils::string::base64_decode(enc);
    REQUIRE((enc.size() > 0 && enc.back() == '='));
    REQUIRE(utils::string::is_base64(enc));
    REQUIRE(dec == str);

    for (int j = 0; j < 5; ++j) {
        str = utils::random::generate_string<>(1024);
        enc = utils::string::base64_encode(str);
        dec = utils::string::base64_decode(enc);
        REQUIRE(utils::string::is_base64(enc));
        REQUIRE(dec == str);
    }
}

TEST_CASE("Test utils::string::base64_decode", "[utils][utils::string]") {
    REQUIRE(utils::string::base64_decode(""        ) == ""      );
    REQUIRE(utils::string::base64_decode("Zg=="    ) == "f"     );
    REQUIRE(utils::string::base64_decode("Zm8="    ) == "fo"    );
    REQUIRE(utils::string::base64_decode("Zm9v"    ) == "foo"   );
    REQUIRE(utils::string::base64_decode("Zm9vYg==") == "foob"  );
    REQUIRE(utils::string::base64_decode("Zm9vYmE=") == "fooba" );
    REQUIRE(utils::string::base64_decode("Zm9vYmFy") == "foobar");

    REQUIRE(std::strcmp(utils::string::base64_decode("AA==").c_str(), "\0"          ) == 0);
    REQUIRE(std::strcmp(utils::string::base64_decode("AAA=").c_str(), "\0\0"        ) == 0);
    REQUIRE(std::strcmp(utils::string::base64_decode("AAAA").c_str(), "\0\0\0"      ) == 0);
    REQUIRE(std::strcmp(utils::string::base64_decode("/w==").c_str(), "\377"        ) == 0);
    REQUIRE(std::strcmp(utils::string::base64_decode("//8=").c_str(), "\377\377"    ) == 0);
    REQUIRE(std::strcmp(utils::string::base64_decode("////").c_str(), "\377\377\377") == 0);
    REQUIRE(std::strcmp(utils::string::base64_decode("/+8=").c_str(), "\xff\xef"    ) == 0);

    CHECK_THROWS_AS(utils::string::base64_decode("^"    ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::base64_decode("A"    ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::base64_decode("A^"   ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::base64_decode("AA"   ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::base64_decode("AA="  ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::base64_decode("AA===") == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::base64_decode("AAA"  ) == "", utils::exceptions::ConversionException);
    CHECK_THROWS_AS(utils::string::base64_decode("AAA^" ) == "", utils::exceptions::ConversionException);
}

#endif
