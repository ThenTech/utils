# C++ Utility library

[![codecov](https://codecov.io/gh/Wosser1sProductions/utils/branch/master/graph/badge.svg)](https://codecov.io/gh/Wosser1sProductions/utils)
[![Travis (.org)](https://img.shields.io/travis/wosser1sproductions/utils.svg)](https://travis-ci.org/Wosser1sProductions/utils)
[![GitHub last commit](https://img.shields.io/github/last-commit/Wosser1sProductions/utils.svg)](https://github.com/Wosser1sProductions/utils/commits/master)
![License](https://img.shields.io/github/license/Wosser1sProductions/utils.svg?color=blue)

[![version](https://img.shields.io/badge/version-0.45.0--beta-brightgreen.svg)](http://semver.org/)
![c++-version](https://img.shields.io/badge/std-%3E%3D%20c%2B%2B17-brightgreen.svg)
[![tested-with](https://img.shields.io/badge/tested%20with-g%2B%2B--9.2-orange.svg)](https://gcc.gnu.org/gcc-9/)

Compile with: `-std=c++17`

Linker options:
- `-lstdc++fs` to enable `std::experimental::filesystem`
- `-pthread` to use threading with CSV files
- `-lws2_32` for Windows socket support (in `utils::http`)
- `-lsqlite3` to link with SQLite on Unix systems (SQLite source is also available as alternative)
    Install `sqlite3` and `libsqlite3-dev` to link automatically.

For MSVC:

- For sanity sake, globally define `_CRT_SECURE_NO_WARNINGS` in Properties>C/C++>Preprocessor>Preprocessor Definitions
- Change `#include <sqlite3>` to `#include "sqlite3.h"` inside `sqlite_orm` and add `sqlite3.c` to your build system.

[Doxygen documentation can be found here.](https://wosser1sproductions.github.io/utils/docs/)

[Code coverage (lcov) report can be found here.](https://wosser1sproductions.github.io/utils/docs/code_coverage)

## Contents

| File                                                               | Description                                                  |
| ------------------------------------------------------------------ | ------------------------------------------------------------ |
| [utils_algorithm.hpp](utils_lib/utils_algorithm.hpp)               | Algorithmic extensions and `::iter` with [CPPItertools](https://github.com/ryanhaining/cppitertools) |
| [utils_bits.hpp](utils_lib/utils_bits.hpp)                         | Bit related extensions                                       |
| [utils_colour.hpp](utils_lib/utils_colour.hpp)                     | Colour class and LUTs for colour mappings from [tinycolormap](https://github.com/yuki-koyama/tinycolormap) |
| [utils_compiler.hpp](utils_lib/utils_compiler.hpp)                 | MACRO helpers                                                |
| [utils_control.hpp](utils_lib/utils_control.hpp)                   | PIDController implementation from [tekdemo](https://github.com/tekdemo/MiniPID) |
| [utils_crc.hpp](utils_lib/utils_crc.hpp)                           | Namespace wrapper for CRC calculations from [CRCpp](https://github.com/d-bahr/CRCpp) |
| [utils_csv.hpp](utils_lib/utils_csv.hpp)                           | Namespace wrapper for CSV file IO from [p-ranav/csv](http://github.com/p-ranav/csv) |
| [utils_exceptions.hpp](utils_lib/utils_exceptions.hpp)             | Extra Exceptions                                             |
| [utils_http.hpp](utils_lib/utils_http.hpp)                         | Namespace wrapper for HTTPRequest from [elnormous/HTTPRequest](http://github.com/elnormous/HTTPRequest) |
| [utils_ini.hpp](utils_lib/utils_ini.hpp)                           | ConfigReader class commonly for `.ini` files                 |
| [utils_io.hpp](utils_lib/utils_io.hpp)                             | File/Stream IO (BitStream...) and `::mio` with [memory mapped file io](https://github.com/mandreyel/mio) |
| [utils_json.hpp](utils_lib/utils_json.hpp)                         | Namespace wrapper for [nlohmann JSON](https://github.com/nlohmann/json) |
| [utils_logger.hpp](utils_lib/utils_logger.hpp)                     | Static Logger singleton                                      |
| [utils_math.hpp](utils_lib/utils_math.hpp)                         | Math extensions                                              |
| [utils_memory.hpp](utils_lib/utils_memory.hpp)                     | Memory management                                            |
| [utils_misc.hpp](utils_lib/utils_misc.hpp)                         | Other (lexical_cast, scoped callback...)                     |
| [utils_os.hpp](utils_lib/utils_os.hpp)                             | OS extensions (console manipulation with colours etc.)       |
| [utils_print.hpp](utils_lib/utils_print.hpp)                       | Generic pretty printer                                       |
| [utils_profiler.hpp](utils_lib/utils_profiler.hpp)                 | Static Profiler singleton and macros for profiling function-/blocks in `chrome://tracing` format |
| [utils_random.hpp](utils_lib/utils_random.hpp)                     | Namespace wrapper for [effolkronium/random](https://github.com/effolkronium/random) and extra generators |
| [utils_sqlite.hpp](utils_lib/utils_sqlite.hpp)                     | Namespace wrapper for [fnc12/sqlite_orm](https://github.com/fnc12/sqlite_orm) |
| [utils_string.hpp](utils_lib/utils_string.hpp)                     | String extensions                                            |
| [utils_test.hpp](utils_lib/utils_test.hpp)                         | Extra's for Doctest like custom ASSERT and std::abort() recoverable test |
| [utils_threading.hpp](utils_lib/utils_threading.hpp)               | Threadpool class and related things                          |
| [utils_time.hpp](utils_lib/utils_time.hpp)                         | Time measurement                                             |
| [utils_traits.hpp](utils_lib/utils_traits.hpp)                     | Type traits and other macro helpers.                         |
| [utils_version.hpp](utils_lib/utils_version.hpp)                   | Namespace wrapper for [Neargye semver](https://github.com/Neargye/semver) |
| [utils_xorstring.hpp](utils_xorstring.hpp)                         | Compile time string obfuscation from [JustasMasiulis](https://github.com/JustasMasiulis/xorstr) or [qis](https://github.com/qis/xorstr) |
| [algo/algo_avltree.hpp](utils_lib/algo/algo_avltree.hpp)           | AVL Tree implementation                                      |
| [algo/algo_bstree.hpp](utils_lib/algo/algo_bstree.hpp)             | Binary Search Tree implementation                            |
| [algo/algo_huffman.hpp](utils_lib/algo/algo_huffman.hpp)           | Huffman compress/decompress                                  |
| [crypto/crypto_aes.hpp](utils_lib/crypto/crypto_aes.hpp)           | Basic AES implementation (WIP)                               |
| [crypto/crypto_feistel.hpp](utils_lib/crypto/crypto_feistel.hpp)   | Basic Feistel cipher structure (WIP)                         |
| [crypto/crypto_packager.hpp](utils_lib/crypto/crypto_packager.hpp) | Basic functions to pack/unpack data (WIP)                    |
| [external/cppitertools](utils_lib/external/cppitertools/)          | [CPPItertools](https://github.com/ryanhaining/cppitertools) by Ryan Haining |
| [external/crc](utils_lib/external/crc)                             | CRC calculations from [CRCpp](https://github.com/d-bahr/CRCpp) |
| [external/csv](utils_lib/external/csv/)                            | [csv](http://github.com/p-ranav/csv) library by p-ranav      |
| [external/hedley](utils_lib/external/hedley/)                      | [Hedley](https://github.com/nemequ/hedley) MACRO helpers     |
| [external/HTTPRequest](utils_lib/external/HTTPRequest/)            | [HTTPRequest](http://github.com/elnormous/HTTPRequest) library by elnormous |
| [external/sqlite_orm](utils_lib/external/sqlite_orm/)              | [SQLite3](https://www.sqlite.org/index.html) wrapper by [fnc12](https://github.com/fnc12/sqlite_orm). Also contains sqlite3 amalgamation. |
| [external/doctest.hpp](utils_lib/external/doctest.hpp)             | [Doctest](https://github.com/onqtam/doctest) C++ test framework |
| [external/json.hpp](utils_lib/external/json.hpp)                   | [JSON](https://github.com/nlohmann/json) library by Niels Lohmann |
| [external/mio.hpp](utils_lib/external/mio.hpp)                     | [mio](https://github.com/mandreyel/mio) by Mabdreyel         |
| [external/random.hpp](utils_lib/external/random.hpp)               | Modern random utilities from [effolkronium/random](https://github.com/effolkronium/random) |
| [external/semver.hpp](utils_lib/external/semver.hpp)               | [Semantic versioning](https://semver.org/) library by [Neargye](https://github.com/Neargye/semver) |
| [/utils_test/](utils_test/)                                        | Unit tests for most functions with [Doctest](https://github.com/onqtam/doctest) |
