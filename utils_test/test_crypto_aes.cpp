#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/external/doctest.hpp"

#include "../utils_lib/crypto/crypto_aes.hpp"
#include "../utils_lib/utils_random.hpp"
#include "../utils_lib/utils_logger.hpp"

TEST_CASE("Test utils::crypto::aes") {
//#define PRINT_DEBUG

#ifdef PRINT_DEBUG
    utils::Logger::Info("Test AES");
#endif

    utils::crypto::AES aes;

    auto key = utils::random::generate_x<uint8_t>(256 / 8);
    auto inp = utils::random::generate_string<char>(65, 'a', 'z');
    uint32_t out_len = 0;
    auto enc = aes.EncryptECB(reinterpret_cast<uint8_t*>(inp.data()), uint32_t(inp.size()), key.data(), out_len);
    auto dec = aes.DecryptECB(enc.get(), out_len, key.data());
    auto out = std::string{reinterpret_cast<char*>(dec.get())};

#ifdef PRINT_DEBUG
    utils::Logger::Stream("\nInput: ", inp,
                          "\nKey  : ", utils::string::join(key),
                          "\n\nout  : (", out_len, ")\n");
    utils::print::hexdump(utils::Logger::GetConsoleStream(),
                          enc.get(), out_len);
    utils::Logger::Stream("\ndecr : ", out, " (", out.size(), ")\n");
    utils::print::hexdump(utils::Logger::GetConsoleStream(),
                          out.data(), out.size());
#endif

    const bool enr_decr_test = inp == out;

    REQUIRE(enr_decr_test);

#ifdef PRINT_DEBUG
    utils::Logger::Stream("\nTest: ",
                          enr_decr_test ? (utils::os::Console::FG | utils::os::Console::BOLD | utils::os::Console::GREEN)
                                        : (utils::os::Console::FG | utils::os::Console::BOLD | utils::os::Console::RED),
                          std::boolalpha, enr_decr_test,
                          utils::os::Console::RESET, '\n');
#endif
}

#endif
