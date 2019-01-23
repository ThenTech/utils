#ifndef UTILS_PRINT_TEST_HPP
#define UTILS_PRINT_TEST_HPP

#include "../utils_lib/utils_memory.hpp"
#include "../utils_lib/utils_print.hpp"

#include <numeric>
#include <map>
#include <set>

namespace test::print {
    [[maybe_unused]]
    static bool run() {
        int argc = 10;

        std::string cs;
        std::unordered_map<int, std::string> um;
        std::map<int, std::string> om;
        std::set<std::string> ss;
        std::vector<std::string> v;
        std::vector<std::vector<std::string>> vv;
        std::vector<std::pair<int, std::string>> vp;
        std::vector<double> vd;
         v.reserve(std::size_t(argc - 1));
        vv.reserve(std::size_t(argc - 1));
        vp.reserve(std::size_t(argc - 1));
        vd.reserve(std::size_t(argc - 1));

        std::cout << "Printing pairs." << std::endl;

        while (--argc) {
          std::string s(std::to_string(argc));
          std::pair<int, std::string> p(argc, s);

          um[argc] = s;
          om[argc] = s;
          v.push_back(s);
          vv.push_back(v);
          vp.push_back(p);
          vd.push_back(1./double(argc));
          ss.insert(s);
          cs += s;

          std::cout << "  " << p << std::endl;
        }

        std::array<char, 5> a{{ 'h', 'e', 'l', 'l', 'o' }};

        std::cout << "Vector: " << v << std::endl
                  << "Incremental vector: " << vv << std::endl
                  << "Another vector: " << vd << std::endl
                  << "Pairs: " << vp << std::endl
                  << "Set: " << ss << std::endl
                  << "OMap: " << om << std::endl
                  << "UMap: " << um << std::endl
                  << "String: " << cs << std::endl
                  << "Array: " << a << std::endl
        ;

        auto arr = utils::memory::new_unique_flat_array<int>(10, 10);
        std::iota(arr.get(), arr.get()+100, 0);

        auto a1 = std::make_pair(std::string("Jello"), 9);
        auto a2 = std::make_tuple(1729);
        auto a3 = std::make_tuple("Qrgh", a1, 11);
        auto a4 = std::make_tuple(1729, 2875, std::pair<double, std::string>(1.5, "meow"));
        int arr2[] = { 1, 4, 9, 16 };

        std::cout << "C array: " << utils::printer::print_array_helper(arr2) << std::endl
                  << "Pair: " << a1 << std::endl
                  << "1-tuple: " << a2 << std::endl
                  << "n-tuple: " << a3 << std::endl
                  << "n-tuple: " << a4 << std::endl
                  << "arrrrrr: " << utils::printer::print_array_helper<int, 100>(arr.get()) << std::endl
        ;

        return true;
    }
}

#endif // UTILS_PRINT_TEST_HPP
