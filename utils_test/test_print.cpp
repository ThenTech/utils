#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/utils_catch.hpp"

#include <numeric>
#include <map>
#include <set>
#include <sstream>

#include "../utils_lib/utils_memory.hpp"
#include "../utils_lib/utils_print.hpp"
#include "../utils_lib/utils_random.hpp"


#define PUT_SS(V)       std::stringstream().swap(test_op); test_op << (V);

TEST_CASE("Test utils::print", "[utils][utils::print]") {
    std::stringstream test_op;
    std::stringstream formatter;

    const int         randomint(utils::random::Random::get<int>());
    const std::string randomstr(std::to_string(randomint));

    SECTION("Test std::pair printing") {
        utils::print::delimiters_values dels(
            utils::print::delimiters<std::pair<int, std::string>, char>::values);
        formatter << dels.prefix    << randomint
                  << dels.delimiter << randomint
                  << dels.postfix;
        const std::string frmt(formatter.str());

        const std::pair<int, std::string> var1(randomint, randomstr);
        const auto var2 = std::make_pair(randomstr, randomint);

        PUT_SS(var1);
        CHECK(test_op.str() == frmt);

        PUT_SS(var2);
        CHECK(test_op.str() == frmt);
    }

    SECTION("Test std::array printing") {
        utils::print::delimiters_values dels(
            utils::print::delimiters<std::array<char, 5>, char>::values);
        formatter << dels.prefix    << 'h'
                  << dels.delimiter << 'e'
                  << dels.delimiter << 'l'
                  << dels.delimiter << 'l'
                  << dels.delimiter << 'o'
                  << dels.postfix;
        const std::string frmt(formatter.str());

        std::array<char, 5> var{{ 'h', 'e', 'l', 'l', 'o' }};

        PUT_SS(var);
        CHECK(test_op.str() == frmt);
    }

    SECTION("Test c-style array printing") {
        int var[] = {
            utils::random::Random::get<int>(),
            utils::random::Random::get<int>(),
            utils::random::Random::get<int>(),
            utils::random::Random::get<int>()
        };

        utils::print::delimiters_values dels(
            utils::print::delimiters<const int(&)[4], char>::values);
        formatter << dels.prefix    << var[0]
                  << dels.delimiter << var[1]
                  << dels.delimiter << var[2]
                  << dels.delimiter << var[3]
                  << dels.postfix;
        const std::string frmt(formatter.str());

        PUT_SS(utils::print::print_array_helper(var));
        CHECK(test_op.str() == frmt);
    }

    SECTION("Test flat array printing") {
        auto var = utils::memory::new_unique_flat_array<int>(10, 10);
        std::generate_n(var.get(), 100, []{ return utils::random::Random::get<int>(); });

        utils::print::delimiters_values dels(
            utils::print::delimiters<const int(&)[100], char>::values);
        formatter << dels.prefix << var[0];
        for (size_t i = 1; i < 100;)
            formatter << dels.delimiter << var[i++];
        formatter << dels.postfix;

        const std::string frmt(formatter.str());

        PUT_SS( (utils::print::print_array_helper<int, 100>(var.get())) );
        CHECK(test_op.str() == frmt);
    }

    SECTION("Test std::tuple printing") {
        SECTION("Test std::tuple<int> printing") {
            utils::print::delimiters_values dels(
                utils::print::delimiters<std::tuple<int>, char>::values);
            formatter << dels.prefix    << randomint
                      << dels.postfix;
            const std::string frmt(formatter.str());

            const auto var = std::make_tuple(randomint);

            PUT_SS(var);
            CHECK(test_op.str() == frmt);
        }

        SECTION("Test std::tuple<string, pair, int> printing") {
            const auto var1 = std::make_pair(randomstr, randomint);

            utils::print::delimiters_values dels(
                utils::print::delimiters<std::tuple<std::string, std::pair<int, std::string>, int>, char>::values);
            formatter << dels.prefix    << randomstr
                      << dels.delimiter << var1
                      << dels.delimiter << randomint
                      << dels.postfix;
            const std::string frmt(formatter.str());

            const auto var2 = std::make_tuple(randomstr, var1, randomint);

            PUT_SS(var2);
            CHECK(test_op.str() == frmt);
        }

        SECTION("Test std::tuple<int, int, pair> printing") {
            const int    randomint2(utils::random::Random::get<int>());
            const double randomdbl (utils::random::Random::get<double>());
            const std::pair<double, std::string> var1(randomdbl, "meow");

            utils::print::delimiters_values dels(
                utils::print::delimiters<std::tuple<int, int, std::pair<double, std::string>>, char>::values);
            formatter << dels.prefix    << randomint
                      << dels.delimiter << randomint2
                      << dels.delimiter << var1
                      << dels.postfix;
            const std::string frmt(formatter.str());

            const auto var2 = std::make_tuple(randomint, randomint2, var1);

            PUT_SS(var2);
            CHECK(test_op.str() == frmt);
        }
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

TEST_CASE("Test utils::print::type2name", "[utils][utils::print]") {
    CHECK(utils::string::contains(utils::print::type2name(3)    , "int"     ).first);
    CHECK(utils::string::contains(utils::print::type2name(3.0)  , "double"  ).first);
    CHECK(utils::string::contains(utils::print::type2name(3.0f) , "float"   ).first);
    CHECK(utils::string::contains(utils::print::type2name("")   , "char"    ).first);
    CHECK(utils::string::contains(utils::print::type2name("")   , '['       ).first);

    std::string name;

    name = utils::print::type2name(std::string());
    CHECK(utils::string::contains(name, "string").first);
    CHECK(utils::string::contains(name, "std::").first);

    name = utils::print::type2name(std::string(), "std::");
    CHECK(utils::string::contains(name, "string").first);
    CHECK_FALSE(utils::string::contains(name, "std::").first);
}

TEST_CASE("Test utils::print::hexDump", "[utils][utils::print]") {
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

#endif
