#include <iostream>

#include "utils_lib/utils_exceptions.hpp"
#include "utils_lib/utils_bits.hpp"
#include "utils_lib/utils_string.hpp"
#include "utils_lib/utils_io.hpp"
#include "utils_lib/utils_logger.hpp"
#include "utils_lib/utils_memory.hpp"
#include "utils_lib/utils_os.hpp"
#include "utils_lib/utils_time.hpp"
#include "utils_lib/utils_misc.hpp"

#include "utils_lib/algo/algo_huffman.hpp"


/*
 *  Possible others:
 *      - Config reader with sections?
 *      - json lib
 *      - csv lib
 *      - Floating/Signed-Bitset
 *      - Auto Mutex lock
 */

int main() {
    utils::Logger::Create();

    /* 0x06 => 4
     * 0xFFFFFFF0 => 5
     * 0x10 => 6
     * 0x08 => 5
     */
    utils::Logger::Writef("0x%02X => %d\n", 6  , utils::bits::bits_needed(6)   );
    utils::Logger::Writef("0x%02X => %d\n", -16, utils::bits::bits_needed<uint8_t>(16) );
    utils::Logger::Writef("0x%02X => %d\n", 16 , utils::bits::bits_needed(16)  );
    utils::Logger::Writef("0x%02X => %d\n", 8  , utils::bits::bits_needed(8)   );
    utils::Logger::Writef("0x%02X => %d\n", 8  , utils::bits::ffs(8)  );

    utils::Logger::Destroy();
    return 0;
}
