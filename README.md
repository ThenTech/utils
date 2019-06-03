# C++ Utility library

[![codecov](https://codecov.io/gh/Wosser1sProductions/utils/branch/master/graph/badge.svg)](https://codecov.io/gh/Wosser1sProductions/utils)
[![Travis (.org)](https://img.shields.io/travis/wosser1sproductions/utils.svg)](https://travis-ci.org/Wosser1sProductions/utils)
![version](https://img.shields.io/badge/version-0.35.0--beta-brightgreen.svg)
![GitHub last commit](https://img.shields.io/github/last-commit/Wosser1sProductions/utils.svg)
![c++-version](https://img.shields.io/badge/c%2B%2B-%3E%3D%20c%2B%2B17-brightgreen.svg)
![tested-unix](https://img.shields.io/badge/tested%20with-g%2B%2B--9.1-orange.svg)
![tested-win](https://img.shields.io/badge/tested%20with-MinGW--7.3-orange.svg)

Compile with: `-std=c++17`
And link with `-lstdc++fs` to enable `std::experimental::filesystem`

## Contents

| File                                                     | Description                                                  |
| -------------------------------------------------------- | ------------------------------------------------------------ |
| [utils_algorithm.hpp](utils_lib/utils_algorithm.hpp)     | Algorithmic extensions                                       |
| [utils_bits.hpp](utils_lib/utils_bits.hpp)               | Bit related extensions                                       |
| [utils_catch.hpp](utils_lib/utils_catch.hpp)             | Extra's for Catch2 like custom ASSERT and std::abort() recoverable test |
| [utils_colour.hpp](utils_lib/utils_colour.hpp)           | Colour class and LUTs for colour mappings from [tinycolormap](https://github.com/yuki-koyama/tinycolormap) |
| [utils_exceptions.hpp](utils_lib/utils_exceptions.hpp)   | Extra Exceptions                                             |
| [utils_ini.hpp](utils_lib/utils_ini.hpp)                 | ConfigReader class commonly for `.ini` files                 |
| [utils_io.hpp](utils_lib/utils_io.hpp)                   | File/Stream IO (BitStream...)                                |
| [utils_json.hpp](utils_lib/utils_json.hpp)               | Namespace wrapper for [nlohmann JSON](https://github.com/nlohmann/json) |
| [utils_logger.hpp](utils_lib/utils_logger.hpp)           | Static Logger singleton                                      |
| [utils_math.hpp](utils_lib/utils_math.hpp)               | Math extensions                                              |
| [utils_memory.hpp](utils_lib/utils_memory.hpp)           | Memory management                                            |
| [utils_misc.hpp](utils_lib/utils_misc.hpp)               | Other                                                        |
| [utils_os.hpp](utils_lib/utils_os.hpp)                   | OS extensions (console manipulation with colours etc.)       |
| [utils_print.hpp](utils_lib/utils_print.hpp)             | Generic pretty printer                                       |
| [utils_random.hpp](utils_lib/utils_random.hpp)           | Namespace wrapper for [effolkronium/random](https://github.com/effolkronium/random) and extra generators |
| [utils_string.hpp](utils_lib/utils_string.hpp)           | String extensions                                            |
| [utils_threading.hpp](utils_lib/utils_threading.hpp)     | Threadpool class                                             |
| [utils_time.hpp](utils_lib/utils_time.hpp)               | Time measurement                                             |
| [utils_traits.hpp](utils_lib/utils_traits.hpp)           | Type traits and other macro helpers.                         |
| [utils_version.hpp](utils_lib/utils_version.hpp)         | Namespace wrapper for [Neargye semver](https://github.com/Neargye/semver) |
| [utils_xorstring.hpp](utils_xorstring.hpp)               | Compile time string obfuscation from [JustasMasiulis](https://github.com/JustasMasiulis/xorstr) or [qis](https://github.com/qis/xorstr) |
| [algo/algo_avltree.hpp](utils_lib/algo/algo_avltree.hpp) | AVL Tree implementation                                      |
| [algo/algo_bstree.hpp](utils_lib/algo/algo_bstree.hpp)   | Binary Search Tree implementation                            |
| [algo/algo_huffman.hpp](utils_lib/algo/algo_huffman.hpp) | Huffman compress/decompress                                  |
| [external/catch.hpp](utils_lib/external/catch.hpp)       | [Catch2](https://github.com/catchorg/Catch2) C++ test framework |
| [external/json.hpp](utils_lib/external/json.hpp)         | [JSON](https://github.com/nlohmann/json) library by Niels Lohmann |
| [external/random.hpp](utils_lib/external/random.hpp)     | Modern random utilities from [effolkronium/random](https://github.com/effolkronium/random) |
| [external/semver.hpp](utils_lib/external/semver.hpp)     | [Semantic versioning](https://semver.org/) library by [Neargye](https://github.com/Neargye/semver) |
| [/utils_test/](utils_test/)                              | Unit tests for most functions with [Catch2](https://github.com/catchorg/Catch2) |
