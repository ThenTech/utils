#include "utils_test/test_settings.hpp"

#ifdef ENABLE_TESTS
    #warning "TESTS ENABLED"

    #define CATCH_CONFIG_RUNNER
    #define CATCH_CONFIG_CONSOLE_WIDTH 100
    #include "utils_lib/utils_logger.hpp"
    #include "utils_lib/external/catch.hpp"
#else
    #warning "TESTS DISABLED"

    #include "utils_lib/utils_exceptions.hpp"
    #include "utils_lib/utils_algorithm.hpp"
    #include "utils_lib/utils_bits.hpp"
    #include "utils_lib/utils_catch.hpp"
    #include "utils_lib/utils_colour.hpp"
    #include "utils_lib/utils_string.hpp"
    #include "utils_lib/utils_io.hpp"
    #include "utils_lib/utils_json.hpp"
    #include "utils_lib/utils_logger.hpp"
    #include "utils_lib/utils_math.hpp"
    #include "utils_lib/utils_memory.hpp"
    #include "utils_lib/utils_misc.hpp"
    #include "utils_lib/utils_os.hpp"
    #include "utils_lib/utils_print.hpp"
    #include "utils_lib/utils_random.hpp"
    #include "utils_lib/utils_time.hpp"
    #include "utils_lib/utils_xorstring.hpp"
    #include "utils_lib/utils_threading.hpp"

    #include "utils_lib/algo/algo_huffman.hpp"
#endif

/*
 *  Possible others:
 *      - Config reader with sections?
 *      - csv lib
 *      - Floating/Signed-Bitset
 *      - Other threading/future features
 *
 *      - Rename functions
 *
 *      - Catch 2 with BENCHMARK or Hayai
 *
 *  https://github.com/Martchus/cpp-utilities
 *  https://github.com/tlx/tlx
 *
 * TODO Doxygen
 * TODO ErrorTrace + Stacktrace?
 */
int main(int argc, char* argv[]) {

#ifdef ENABLE_TESTS
    utils::Logger::SetScreenTitle("Testing C++ Utility library");
    utils::Logger::WriteLn("Running tests...");

    int status = 0;
    const auto test_duration = utils::time::Timer::time<utils::time::Timer::time_ms>([&](){
        status = Catch::Session().run(argc, argv);
    });

    utils::Logger::Writef("Tests completed in %.3f ms (%d)" + utils::Logger::CRLF, test_duration, status);

    return status;
#else
    utils::Logger::Create("test.log", utils::Logger::Level::LOG_DEBUG);
    utils::Logger::SetScreenTitle("C++ Utility library");

    utils::Logger::WriteLn("Start");

    utils::Logger::Debug("Debug");
    utils::Logger::Info("Info");
    utils::Logger::Success("Success");
    utils::Logger::Notice("Notice");
    utils::Logger::Warn("Warn");
    utils::Logger::Error("Error");
    utils::Logger::Critical("Critical");
    utils::Logger::Alert("Alert");
    utils::Logger::Emergency("Emergency");

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
    utils::Logger::WriteLn(utils::print::type2name(std::string() /*, "std::"*/));
    utils::Logger::Command(utils::os::Console::RESET);

//    utils::algo::Huffman<>::encode("README.md", "enc.txt");
//    utils::algo::Huffman<>::decode("enc.txt", "dec.txt");

//    auto vec2 = utils::memory::new_unique_vector<utils::algo::Node<>>();
//    vec2->emplace_back(utils::memory::allocVar<utils::algo::Node<>>(0));

//    auto arr = utils::memory::new_unique_flat_array<int>(10, 10);
//    std::iota(arr.get(), arr.get()+100, 0);
//    utils::Logger::Writef("sum: %d\n", std::accumulate(arr.get(), arr.get() + 100, 0));

    auto file_list = utils::io::list_contents("./");
    utils::Logger::Stream("\n\nHello\n", *file_list, "\n");
    utils::Logger::Stream(utils::random::Random::get(*file_list), "\n");

    std::string s = utils::random::generate_string<char>(10, 'a', 'z');
    utils::Logger::Stream(s.size(), ", ", s.size() == 10, " => '", s, "'\n");

    try {
        throw utils::exceptions::FileReadException("log.txt");
    } catch (const std::exception& e) {
        LOG_ERROR_TRACE(e);
    }

    utils::Logger::Writef("UUID: %s\n", utils::random::generate_uuid().c_str());

//    const double timed_ns = utils::time::Timer::time_n<1>([](){
//        using namespace std::chrono_literals;
//        std::this_thread::sleep_for(1s);
//    });
//    utils::Logger::Writef("Took %0.6f ns\n", timed_ns);

//    utils::Logger::Stream(utils::colour::GetHotColor(0.5));
//    utils::Logger::Stream(utils::colour::Colour::from_hex("#ABCDEF").to_hex());

    return 0;
#endif  // ENABLE_TESTS
}
