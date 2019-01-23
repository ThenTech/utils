#include "utils_lib/utils_exceptions.hpp"
#include "utils_lib/utils_algorithm.hpp"
#include "utils_lib/utils_bits.hpp"
#include "utils_lib/utils_string.hpp"
#include "utils_lib/utils_io.hpp"
#include "utils_lib/utils_logger.hpp"
#include "utils_lib/utils_memory.hpp"
#include "utils_lib/utils_os.hpp"
#include "utils_lib/utils_print.hpp"
#include "utils_lib/utils_time.hpp"
#include "utils_lib/utils_misc.hpp"

#include "utils_lib/algo/algo_huffman.hpp"

#if 0
    #define ENABLE_TESTS
    #include <cassert>

    #include "utils_test/utils_print_test.hpp"
#endif


/*
 *  Possible others:
 *      - Config reader with sections?
 *      - json lib
 *      - csv lib
 *      - Floating/Signed-Bitset
 *      - Auto Mutex lock
 *      - Log level
 *
 *      - Rename functions
 *
 *  https://github.com/Martchus/cpp-utilities
 */

//struct MyDel { static const utils::printer::delimiters_values<char> values; };
//const utils::printer::delimiters_values<char> MyDel::values = { "[\n    \"", "\",\n    \"", "\"\n]" };

int main() {
    utils::Logger::Create("test.log");

#ifdef ENABLE_TESTS
    assert(utils::algorithm::all(
        test::print::run()
    ));
#else
    utils::Logger::SetScreenTitle("C++ Utility library");

    utils::Logger::WriteLn("Start");

    utils::Logger::Success("Success");
    utils::Logger::Info("Info");
    utils::Logger::Warn("Warn");
    utils::Logger::Error("Error");

    /* 0x06 => 4
     * 0xFFFFFFF0 => 5
     * 0x10 => 6
     * 0x08 => 5
     */
    utils::Logger::Writef("0x%02X => %d\n", 6  , utils::bits::bits_needed(6));
    utils::Logger::Writef("0x%02X => %d\n", -16, utils::bits::bits_needed<int8_t>(-16));
    utils::Logger::Writef("0x%02X => %d\n", 16 , utils::bits::bits_needed(16));
    utils::Logger::Writef("0x%02X => %d\n", 8  , utils::bits::bits_needed(8));
    utils::Logger::Writef("0x%02X => %d\n", 8  , utils::bits::ffs(8));

    utils::Logger::Command(utils::os::Console::BRIGHT | utils::os::Console::CYAN);
    utils::Logger::WriteLn(utils::misc::type2name(std::string() /*, "std::"*/));
    utils::Logger::Command(utils::os::Console::RESET);

//    utils::algo::Huffman<>::encode("README.md", "enc.txt");
//    utils::algo::Huffman<>::decode("enc.txt", "dec.txt");

//    auto vec2 = utils::memory::new_unique_vector<utils::algo::Node<>>();
//    vec2->emplace_back(utils::memory::allocVar<utils::algo::Node<>>(0));

//    auto arr = utils::memory::new_unique_flat_array<int>(10, 10);
//    std::iota(arr.get(), arr.get()+100, 0);
//    utils::Logger::Writef("sum: %d\n", std::accumulate(arr.get(), arr.get() + 100, 0));

    utils::Logger::Stream("\n\nHello\n", *utils::io::list_contents("./"));
#endif

    return 0;
}
