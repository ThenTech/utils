#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/doctest.hpp"

#include "../utils_lib/utils_print.hpp"

#include "../utils_lib/utils_memory.hpp"
#include "../utils_lib/utils_random.hpp"
#include "../utils_lib/utils_misc.hpp"

#include <numeric>
#include <map>
#include <set>
#include <sstream>


#define PUT_SS(V)       std::stringstream().swap(test_op); test_op << V;

TEST_CASE("Test utils::print") {
    std::stringstream test_op;
    std::stringstream formatter;

    const int         randomint(utils::random::Random::get<int>());
    const std::string randomstr(std::to_string(randomint));

    SUBCASE("Test std::pair printing") {
        utils::print::delimiters_values dels(
            utils::print::delimiters<std::pair<int, std::string>, char>::values);
        formatter << (dels.prefix    ? dels.prefix    : "") << randomint
                  << (dels.delimiter ? dels.delimiter : "");
        utils::print::print_quoted_helper(formatter, randomstr)
                  << (dels.postfix   ? dels.postfix   : "");

        const std::pair<int, std::string> var1(randomint, randomstr);
        PUT_SS(var1);
        CHECK(test_op.str() == formatter.str());

        const auto var2 = std::pair(randomstr, randomint);
        std::stringstream().swap(formatter);
        formatter << (dels.prefix    ? dels.prefix    : "");
        utils::print::print_quoted_helper(formatter, randomstr)
                  << (dels.delimiter ? dels.delimiter : "") << randomint
                  << (dels.postfix   ? dels.postfix   : "");

        PUT_SS(var2);
        CHECK(test_op.str() == formatter.str());
    }

    SUBCASE("Test std::array printing") {
        utils::print::delimiters_values dels(
            utils::print::delimiters<std::array<char, 5>, char>::values);
        formatter << (dels.prefix    ? dels.prefix    : "") << 'h'
                  << (dels.delimiter ? dels.delimiter : "") << 'e'
                  << (dels.delimiter ? dels.delimiter : "") << 'l'
                  << (dels.delimiter ? dels.delimiter : "") << 'l'
                  << (dels.delimiter ? dels.delimiter : "") << 'o'
                  << (dels.postfix   ? dels.postfix   : "");

        std::array<char, 5> var{{ 'h', 'e', 'l', 'l', 'o' }};

        PUT_SS(var);
        CHECK(test_op.str() == formatter.str());

        PUT_SS(var.data());
        CHECK(test_op.str() == "h");
    }

    SUBCASE("Test c-style array printing") {
        int var[] = {
            utils::random::Random::get<int>(),
            utils::random::Random::get<int>(),
            utils::random::Random::get<int>(),
            utils::random::Random::get<int>()
        };

        utils::print::delimiters_values dels(
            utils::print::delimiters<const int(&)[4], char>::values);
        formatter << (dels.prefix    ? dels.prefix    : "") << var[0]
                  << (dels.delimiter ? dels.delimiter : "") << var[1]
                  << (dels.delimiter ? dels.delimiter : "") << var[2]
                  << (dels.delimiter ? dels.delimiter : "") << var[3]
                  << (dels.postfix   ? dels.postfix   : "");

        PUT_SS(var);
        CHECK(test_op.str() == formatter.str());

        PUT_SS( (utils::print::print_array_helper<int, 4>(&var[0])) );
        CHECK(test_op.str() == formatter.str());

        const int* var2 = &var[0];
        PUT_SS( (utils::print::array_wrapper<int, 4>(var2)) );
        CHECK(test_op.str() == formatter.str());

//        int empty[]{};
//        PUT_SS(empty);
//        CHECK(test_op.str() == "<Object int [0]>");
    }

    SUBCASE("Test flat array printing") {
        auto var = utils::memory::new_unique_flat_array<int>(10, 10);
        std::generate_n(var.get(), 100, [&](){ return utils::random::Random::get<int>(); });

        utils::print::delimiters_values dels(
            utils::print::delimiters<const int(&)[100], char>::values);
        formatter << (dels.prefix ? dels.prefix : "") << var[0];
        for (size_t i = 1; i < 100;)
            formatter << (dels.delimiter ? dels.delimiter : "") << var[i++];
        formatter << (dels.postfix ? dels.postfix : "");

        PUT_SS(var.get());
        CHECK(test_op.str() == std::to_string(var[0]));

        PUT_SS( (utils::print::print_array_helper<int, 100>(var.get())) );
        CHECK(test_op.str() == formatter.str());
    }

    SUBCASE("Test std::tuple printing") {
        SUBCASE("Test std::tuple<int> printing") {
            utils::print::delimiters_values dels(
                utils::print::delimiters<std::tuple<int>, char>::values);
            formatter << (dels.prefix  ? dels.prefix  : "")    << randomint
                      << (dels.postfix ? dels.postfix : "");
            const auto var = std::make_tuple(randomint);

            PUT_SS(var);
            CHECK(test_op.str() == formatter.str());
        }

        SUBCASE("Test std::tuple<string, pair, int> printing") {
            const auto var1 = std::make_pair(randomstr, randomint);

            utils::print::delimiters_values dels(
                utils::print::delimiters<std::tuple<std::string, std::pair<int, std::string>, int>, char>::values);

            formatter << (dels.prefix    ? dels.prefix    : "");
            utils::print::print_quoted_helper(formatter, randomstr)
                      << (dels.delimiter ? dels.delimiter : "") << var1
                      << (dels.delimiter ? dels.delimiter : "") << randomint
                      << (dels.postfix   ? dels.postfix   : "");
            const auto var2 = std::make_tuple(randomstr, var1, randomint);

            PUT_SS(var2);
            CHECK(test_op.str() == formatter.str());
        }

        SUBCASE("Test std::tuple<int, int, pair> printing") {
            const int    randomint2(utils::random::Random::get<int>());
            const double randomdbl (utils::random::Random::get<double>());
            const std::pair<double, std::string> var1(randomdbl, "meow");

            utils::print::delimiters_values dels(
                utils::print::delimiters<std::tuple<int, int, std::pair<double, std::string>>, char>::values);
            formatter << (dels.prefix    ? dels.prefix    : "") << randomint
                      << (dels.delimiter ? dels.delimiter : "") << randomint2
                      << (dels.delimiter ? dels.delimiter : "") << var1
                      << (dels.postfix   ? dels.postfix   : "");
            const auto var2 = std::make_tuple(randomint, randomint2, var1);

            PUT_SS(var2);
            CHECK(test_op.str() == formatter.str());
        }
    }

    SUBCASE("Test std::optional printing") {
        SUBCASE("Test empty std::optional printing") {
            const std::optional<int> var1;

            utils::print::delimiters_values dels(
                utils::print::delimiters<std::optional<int>, char>::values);
            formatter << (dels.prefix    ? dels.prefix    : "")
                      << (dels.delimiter ? dels.delimiter : "")
                      << (dels.postfix   ? dels.postfix   : "");

            PUT_SS(var1);
            CHECK(test_op.str() == formatter.str());
        }

        SUBCASE("Test std::optional<int> printing") {
            const std::optional<int> var1(randomint);

            utils::print::delimiters_values dels(
                utils::print::delimiters<std::optional<int>, char>::values);
            formatter << (dels.prefix  ? dels.prefix  : "")
                      << randomint
                      << (dels.postfix ? dels.postfix : "");

            PUT_SS(var1);
            CHECK(test_op.str() == formatter.str());
        }
    }

    SUBCASE("Test std::variant printing") {
        std::variant<int, double> var1;

        SUBCASE("Test std::variant int printing") {
            var1 = randomint;

            utils::print::delimiters_values dels(
                utils::print::delimiters<std::variant<int, double>, char>::values);
            formatter << (dels.prefix  ? dels.prefix  : "")
                      << randomint
                      << (dels.postfix ? dels.postfix : "");

            PUT_SS(var1);
            CHECK(test_op.str() == formatter.str());
        }

        SUBCASE("Test std::variant double printing") {
            const double randomdbl (utils::random::Random::get<double>());
            var1 = randomdbl;

            utils::print::delimiters_values dels(
                utils::print::delimiters<std::variant<int, double>, char>::values);
            formatter << (dels.prefix  ? dels.prefix  : "")
                      << randomdbl
                      << (dels.postfix ? dels.postfix : "");
            PUT_SS(var1);
            CHECK(test_op.str() == formatter.str());
        }
    }

    SUBCASE("Test object printing") {
        PUT_SS(formatter);
        CHECK(utils::string::starts_with(test_op.str(), "<Object"));
        CHECK(utils::string::ends_with(test_op.str(), ">"));
        CHECK(utils::string::contains(test_op.str(), "stringstream"));

        PUT_SS("test");
        CHECK(test_op.str() == "test");

        PUT_SS(randomstr.begin());
        CHECK(test_op.str() == randomstr.substr(0, 1));

        PUT_SS(std::hex << static_cast<void*>(&test_op));
        size_t memaddr;
        CHECK((test_op >> memaddr));
        CHECK(memaddr > 0);

        PUT_SS(static_cast<void*>(nullptr));
        CHECK(test_op.str() == "nullptr");
    }

//    // Init vars
//    int argc = 10;

//    std::string cs;
//    std::unordered_map<int, std::string> um;
//    std::map<int, std::string> om;
//    std::set<std::string> ss;
//    std::vector<std::string> v;
//    std::vector<std::vector<std::string>> vv;
//    std::vector<std::pair<int, std::string>> vp;
//    std::vector<double> vd;
//     v.reserve(std::size_t(argc - 1));
//    vv.reserve(std::size_t(argc - 1));
//    vp.reserve(std::size_t(argc - 1));
//    vd.reserve(std::size_t(argc - 1));

//    // Set vars and pair test
//    while (--argc) {
//        std::string s(std::to_string(argc));
//        std::pair<int, std::string> p(argc, s);

//        um[argc] = s;
//        om[argc] = s;
//        v.push_back(s);
//        vv.push_back(v);
//        vp.push_back(p);
//        vd.push_back(1./double(argc));
//        ss.insert(s);
//        cs += s;
//    }

//    std::cout << "Vector: " << v << std::endl
//              << "Incremental vector: " << vv << std::endl
//              << "Another vector: " << vd << std::endl
//              << "Pairs: " << vp << std::endl
//              << "Set: " << ss << std::endl
//              << "OMap: " << om << std::endl
//              << "UMap: " << um << std::endl
//              << "String: " << cs << std::endl
//    ;
}

TEST_CASE("Test utils::print::type2name") {
    CHECK(utils::string::contains(utils::print::type2name(3)    , "int"   ));
    CHECK(utils::string::contains(utils::print::type2name(3.0)  , "double"));
    CHECK(utils::string::contains(utils::print::type2name(3.0f) , "float" ));
    CHECK(utils::string::contains(utils::print::type2name("")   , "char"  ));
    CHECK(utils::string::contains(utils::print::type2name("")   , '['     ));

    std::string name;

    name = utils::print::type2name(std::string());
    CHECK(utils::string::contains(name, "string"));
    CHECK(utils::string::contains(name, "std::"));

    name = utils::print::type2name(std::string(), "std::");
    CHECK(utils::string::contains(name, "string"));
    CHECK_FALSE(utils::string::contains(name, "std::"));
}

TEST_CASE("Test utils::print::hexDump") {
    std::ostringstream test_op;
    #define STR_TEST(DATA, LEN, RESULT) \
        utils::print::hexdump(test_op, DATA, LEN); \
        CHECK(test_op.str() == RESULT); \
        std::ostringstream().swap(test_op);

    STR_TEST(nullptr, 0, "");
    STR_TEST(nullptr, 1, "");
    STR_TEST(""     , 0, "");
    STR_TEST(""     , 1, "0000 : .                00\n");
    STR_TEST("abc"  , 3, "0000 : abc              61 62 63\n");

    const std::vector<int> v{'1', '2', '3', '4', '5', '6', '7', '8'};
    STR_TEST(v.data(), v.size() * sizeof(int),
             "0000 : 1...2...3...4... 31 00 00 00 32 00 00 00 33 00 00 00 34 00 00 00\n"
             "0010 : 5...6...7...8... 35 00 00 00 36 00 00 00 37 00 00 00 38 00 00 00\n");
}

TEST_CASE("Test utils::print::with_progressbar") {
    std::ostringstream test_op;
    std::vector<int> test(10);
    constexpr size_t size =  70 * 10;
    int cnt = 0;

    utils::print::with_progressbar(test.begin(), test.end(), test_op, [&](int){
        cnt++;
    });
    CHECK(cnt == 10);
    CHECK(test_op.str().size() >= size);


    utils::print::Progressbar bar(test.size(), "", true, true, true, true, true, true);
    for (auto val : test) {
        UNUSED(val);
        cnt++;
        test_op << bar++;
    }
    --bar; bar--; bar -= 1; bar += 3;
    test_op << bar.done();

    CHECK(cnt == 20);
    CHECK(test_op.str().size() >= size*2);
}

#endif
