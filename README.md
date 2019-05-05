# C++ Utility library

**Note: A C++17 compliant compiler is preferred.**
The code won't be changed to support previous C++ standards.

Compile with: `--std=c++17`
And link with `-lstdc++fs` to enable `std::experimental::filesystem`

## Contents

| File                                                         | Description                                                  |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| [utils_algorithm.hpp](utils_lib/utils_algorithm.hpp)         | Algorithmic extensions                                       |
| [utils_bits.hpp](utils_lib/utils_bits.hpp)                   | Bit related extensions                                       |
| [utils_exceptions.hpp](utils_lib/utils_exceptions.hpp)       | Extra Exceptions                                             |
| [utils_io.hpp](utils_lib/utils_io.hpp)                       | File/Stream IO (BitStream...)                                |
| [utils_json.hpp](utils_lib/utils_json.hpp)                   | Namespace wrapper for [nlohmann JSON](https://github.com/nlohmann/json) |
| [utils_logger.hpp](utils_lib/utils_logger.hpp)               | Static Logger singleton                                      |
| [utils_memory.hpp](utils_lib/utils_memory.hpp)               | Memory management                                            |
| [utils_misc.hpp](utils_lib/utils_misc.hpp)                   | Other                                                        |
| [utils_os.hpp](utils_lib/utils_os.hpp)                       | OS extensions (console manipulation with colours etc.)       |
| [utils_print.hpp](utils_lib/utils_print.hpp)                 | Generic pretty printer                                       |
| [utils_random.hpp](utils_lib/utils_random.hpp)               | Namespace wrapper for [effolkronium/random](https://github.com/effolkronium/random) |
| [utils_string.hpp](utils_lib/utils_string.hpp)               | String extensions                                            |
| [utils_time.hpp](utils_lib/utils_time.hpp)                   | Time measurement                                             |
| [utils_xorstring.hpp](utils_xorstring.hpp)                   | Compile time string obfuscation from [JustasMasiulis](https://github.com/JustasMasiulis/xorstr) or [qis](https://github.com/qis/xorstr) |
| [algo/utils_huffman.hpp](utils_lib/algo/algo_huffman.hpp)    | Huffman compress/decompress                                  |
| [external/catch.hpp](utils_lib/external/catch.hpp)           | [Catch2](https://github.com/catchorg/Catch2) C++ test framework |
| [external/catch_extra.hpp](utils_lib/external/catch_extra.hpp) | Extra's for Catch like custom ASSERT and std::abort() recoverable test. |
| [external/json.hpp](utils_lib/external/json.hpp)             | [JSON](https://github.com/nlohmann/json) library by Niels Lohmann |
| [external/random.hpp](utils_lib/external/random.hpp)         | Modern random utilities from [effolkronium/random](https://github.com/effolkronium/random) |
| [/utils_test/](utils_test/)                                  | Unit tests for every function with [Catch2](https://github.com/catchorg/Catch2) |
