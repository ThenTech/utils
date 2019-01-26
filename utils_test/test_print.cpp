#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/catch.hpp"

#include <numeric>
#include <map>
#include <set>
#include <sstream>

#include "../utils_lib/utils_memory.hpp"
#include "../utils_lib/utils_print.hpp"
#include "../utils_lib/utils_random.hpp"


#define PUT_SS(V)     std::stringstream().swap(test_op); test_op << (V);

TEST_CASE("Test utils::print", "utils::print" ) {
    std::stringstream test_op;
    std::stringstream formatter;

    const int         randomint(utils::random::Random::get<int>());
    const std::string randomstr(std::to_string(randomint));

    SECTION("Test std::pair printing") {
        utils::printer::delimiters_values dels(
            utils::printer::delimiters<std::pair<int, std::string>, char>::values);
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
        utils::printer::delimiters_values dels(
            utils::printer::delimiters<std::array<char, 5>, char>::values);
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

        utils::printer::delimiters_values dels(
            utils::printer::delimiters<const int(&)[4], char>::values);
        formatter << dels.prefix    << var[0]
                  << dels.delimiter << var[1]
                  << dels.delimiter << var[2]
                  << dels.delimiter << var[3]
                  << dels.postfix;
        const std::string frmt(formatter.str());

        PUT_SS(utils::printer::print_array_helper(var));
        CHECK(test_op.str() == frmt);
    }

    SECTION("Test flat array printing") {
        auto var = utils::memory::new_unique_flat_array<int>(10, 10);
        std::generate_n(var.get(), 100, []{ return utils::random::Random::get<int>(); });

        utils::printer::delimiters_values dels(
            utils::printer::delimiters<const int(&)[100], char>::values);
        formatter << dels.prefix << var[0];
        for (size_t i = 1; i < 100;)
            formatter << dels.delimiter << var[i++];
        formatter << dels.postfix;

        const std::string frmt(formatter.str());

        PUT_SS( (utils::printer::print_array_helper<int, 100>(var.get())) );
        CHECK(test_op.str() == frmt);
    }

    SECTION("Test std::tuple printing") {
        SECTION("Test std::tuple<int> printing") {
            utils::printer::delimiters_values dels(
                utils::printer::delimiters<std::tuple<int>, char>::values);
            formatter << dels.prefix    << randomint
                      << dels.postfix;
            const std::string frmt(formatter.str());

            const auto var = std::make_tuple(randomint);

            PUT_SS(var);
            CHECK(test_op.str() == frmt);
        }

        SECTION("Test std::tuple<string, pair, int> printing") {
            const auto var1 = std::make_pair(randomstr, randomint);

            utils::printer::delimiters_values dels(
                utils::printer::delimiters<std::tuple<std::string, std::pair<int, std::string>, int>, char>::values);
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

            utils::printer::delimiters_values dels(
                utils::printer::delimiters<std::tuple<int, int, std::pair<double, std::string>>, char>::values);
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

#endif
