#define UTILS_PROFILER_ENABLE 1
#include "utils_test/test_settings.hpp"
#include "utils_lib/utils_compiler.hpp"
#include "utils_lib/utils_version.hpp"

// Minor version is git commit count: git rev-list --all --count
static constexpr utils::Version VERSION(0, 46, 0, utils::version::prerelease::beta);

#ifdef ENABLE_TESTS
    HEDLEY_WARNING("Warning: TESTS ENABLED")

    #define DOCTEST_CONFIG_IMPLEMENT
    #include "utils_lib/external/doctest.hpp"

    #include "utils_lib/utils_logger.hpp"
    #include "utils_lib/utils_time.hpp"
    #include "utils_lib/utils_profiler.hpp"
#else
    HEDLEY_WARNING("Warning: TESTS DISABLED")

    // Include first to evade warning about needing to include winsock before windows.h
    #include "utils_lib/utils_http.hpp"

    #include "utils_lib/utils_exceptions.hpp"
    #include "utils_lib/utils_algorithm.hpp"
    #include "utils_lib/utils_bits.hpp"
    #include "utils_lib/utils_colour.hpp"
    #include "utils_lib/utils_crc.hpp"
    #include "utils_lib/utils_csv.hpp"
//    #include "utils_lib/utils_http.hpp"
    #include "utils_lib/utils_ini.hpp"
    #include "utils_lib/utils_io.hpp"
    #include "utils_lib/utils_json.hpp"
    #include "utils_lib/utils_logger.hpp"
    #include "utils_lib/utils_math.hpp"
    #include "utils_lib/utils_memory.hpp"
    #include "utils_lib/utils_misc.hpp"
    #include "utils_lib/utils_os.hpp"
    #include "utils_lib/utils_print.hpp"
    #include "utils_lib/utils_profiler.hpp"
    #include "utils_lib/utils_random.hpp"
    #include "utils_lib/utils_sqlite.hpp"
    #include "utils_lib/utils_string.hpp"
    #include "utils_lib/utils_test.hpp"
    #include "utils_lib/utils_threading.hpp"
    #include "utils_lib/utils_time.hpp"
    #include "utils_lib/utils_traits.hpp"
    #include "utils_lib/utils_xorstring.hpp"

    #include "utils_lib/algo/algo_huffman.hpp"
    #include "utils_lib/algo/algo_bstree.hpp"
    #include "utils_lib/algo/algo_avltree.hpp"
    #include "utils_lib/crypto/crypto_feistel.hpp"
    #include "utils_lib/crypto/crypto_aes.hpp"
#endif

/*
 *  Possible others:
 *      - Floating/Signed-Bitset
 *      - Other threading/future features
 *      - utils::os other commands (cursor movement etc)
 *      - Rename functions
 *
 *      - Catch 2 with BENCHMARK or Hayai
 *          => Move to doctest for faster compilation...
 *             https://github.com/onqtam/doctest
 *
 *  https://github.com/Martchus/cpp-utilities
 *  https://github.com/tlx/tlx
 *  https://github.com/nothings/single_file_libs
 *
 *  https://github.com/palacaze/sigslot
 *  https://github.com/fgoujeon/signal
 *  https://github.com/NoAvailableAlias/nano-signal-slot ?
 *
 *  https://github.com/hosseinmoein/Matrix
 *  https://github.com/fengwang/matrix
 *  Eigen
 *
 *  More containers (like the bs tree)
 *  Common algorithms? (Dijkstra/A* etc)
 *
 *  Args:
 *      https://github.com/p-ranav/argparse
 *      https://github.com/muellan/clipp
 *      https://github.com/Taywee/args
 *
 *  Extra:
 *      https://github.com/jan-moeller/perlin
 *      https://github.com/Reputeless/PerlinNoise
 *
 *      https://github.com/r-lyeh-archived/statvs
 *      https://github.com/nemequ/portable-snippets
 *      https://github.com/chronoxor/CppCommon/blob/master/source/system/stack_trace.cpp
 *      https://github.com/chuyangliu/tastylib
 *
 *      https://github.com/k06a/boolinq
 *      https://github.com/pfultz2/Linq
 *      https://github.com/mrange/cpplinq
 *
 *      /// Add to linker: -lsqlite3; also see ../SQLite_test
 *      https://github.com/qicosmos/ormpp
 *
 *      https://github.com/whoshuu/cpr
 *
 *      https://github.com/yhirose/cpp-httplib
 *      https://github.com/seomoz/url-cpp
 *      https://github.com/chmike/CxxUrl
 *      https://github.com/jwerle/url.h
 *      https://www.youtube.com/watch?v=2hNdkYInj4g  (ASIO without Boost)
 *
 *      https://github.com/troldal/OpenXLSX
 *      https://github.com/tfussell/xlnt
 *
 *      https://github.com/arun11299/cpp-subprocess
 *      https://github.com/sheredom/process.h
 *
 *      https://github.com/jfalcou/spy
 *
 *      https://github.com/dpilger26/NumCpp
 *      https://github.com/skypjack/uvw
 *
 *      https://github.com/alandefreitas/matplotplusplus
 *
 *      https://github.com/JochenKalmbach/StackWalker
 *
 *  GCC >= 9:
 *      https://github.com/Neargye/magic_enum
 *
 *  Resources:
 *      https://github.com/caiorss/C-Cpp-Notes
 *      https://stackoverflow.com/Questions/4158900/embedding-resources-in-executable-using-gcc
 *
 *      https://github.com/OpenCppCoverage/OpenCppCoverage
 */
int main(int argc, char *argv[]) {

#ifdef ENABLE_TESTS
    UTILS_PROFILE_BEGIN_SESSION("utils_test_profile.json");
#if defined(UTILS_COMPILER_MSVC)
    // Disable breaking on std::abort(), CATCH must handle this instead.
    _set_abort_behavior(0, _WRITE_ABORT_MSG);
#endif

    utils::Logger::SetScreenTitle("Testing C++ Utility library " + VERSION.to_string());
    utils::Logger::WriteLn("Running tests...");

    int status = 0;
    const auto test_duration = utils::time::Timer::time<utils::time::Timer::time_ms>([&]() {
         status = doctest::Context(argc, argv).run();
    });

    utils::Logger::Notice("Tests completed in %.3f ms (%d)", test_duration, status);
    return status;
#else
    UTILS_PROFILE_BEGIN_SESSION("utils_profile.json");
    UNUSED(argc, argv);
    utils::Logger::Create("test.log", utils::Logger::Level::LOG_DEBUG);
    utils::Logger::SetScreenTitle("C++ Utility library " + VERSION.to_string());

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

    utils::Logger::Command(utils::os::Console::BRIGHT | utils::os::Console::CYAN);
    utils::Logger::WriteLn(utils::print::type2name(std::string() /*, "std::"*/));
    utils::Logger::Command(utils::os::Console::RESET);

//    utils::algo::Huffman<>::encode("README.md", "enc.txt");
//    utils::algo::Huffman<>::decode("enc.txt", "dec.txt");

//    utils::crypto::FeistelCipher<>::encode("README.md", "enc.txt");
//    utils::crypto::FeistelCipher<>::decode("enc.txt", "dec.txt");

//    auto vec2 = utils::memory::new_unique_vector<utils::algo::Node<>>();
//    vec2->emplace_back(utils::memory::allocVar<utils::algo::Node<>>(0));

//    auto arr = utils::memory::new_unique_flat_array<int>(10, 10);
//    std::iota(arr.get(), arr.get()+100, 0);
//    utils::Logger::Writef("sum: %d\n", utils::algorithm::sum(arr.get(), arr.get() + 100));

    const auto file_list = utils::io::list_contents("./");
    const auto rfile     = utils::random::Random::get(*file_list);
    utils::Logger::Stream("\n\nHello\n", *file_list, "\n");
    utils::Logger::Stream(rfile, " => ",
                          utils::time::Timestamp(utils::io::file_last_modified(*rfile)), "\n");

    std::string s = utils::random::generate_string<char>(10, 'a', 'z');
    utils::Logger::Stream(s.size(), ", ", s.size() == 10, " => '", s, "'\n");

    try {
        throw utils::exceptions::FileReadException("log.txt");
    } CATCH_AND_LOG_ERROR_TRACE()

    utils::Logger::Writef("UUID: %s\n", utils::random::generate_uuid().c_str());

//    const double timed_ns = utils::time::Timer::time_n<1>([](){
//        utils::time::sleep(utils::time::seconds(1));
//    });
//    utils::Logger::Writef("Took %0.6f ns\n", timed_ns);

//    utils::Logger::Stream(utils::colour::GetHotColor(0.5));
//    utils::Logger::Stream(utils::colour::Colour::from_hex("#ABCDEF").to_hex());

//    utils::Logger::WriteProgress(utils::algorithm::iter::range<size_t>(0, 400),
//                                 [](size_t){ utils::time::sleep(utils::time::milliseconds(1)); });

    utils::Logger::Stream("\n\n", utils::memory::Metrics, "\n");

    return 0;
#endif  // ENABLE_TESTS
}
