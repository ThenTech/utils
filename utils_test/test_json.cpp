#include "test_settings.hpp"

#ifdef ENABLE_TESTS
#include "../utils_lib/utils_catch.hpp"

#include "../utils_lib/utils_json.hpp"

#include "../utils_lib/utils_string.hpp"
#include "../utils_lib/utils_logger.hpp"

//#define TEST_JSON_LOG_STEPS


TEST_CASE("Test utils::json::to_ubjson", "[utils][utils::json]" ) {
    utils::json start = R"(
        {
            "LABELS": {
                "YELLOW": {
                    "DEFAULT": {
                        "BM_HILLBOOST_ACTIVE__NAK": {
                            "description": "Bitmask to activate the hillboost functionality. Checks z_bc_hillboost__nak",
                            "func": "NAK_IO",
                            "info_param_names": ["Hill Boost"],
                            "name": "BM_HILLBOOST_ACTIVE__NAK",
                            "type": 0,
                            "value": 0,
                            "value_choices": [["no", 0], ["yes", 16]],
                            "value_unit": "-"
                        }
                    }
                }
            }
        }
    )"_json;

    REQUIRE(start.size() > 0);
    #ifdef TEST_JSON_LOG_STEPS
        utils::Logger::Stream(std::setw(4), start, utils::Logger::CRLF,
                              utils::Logger::LINE<>);
    #endif

    std::vector<std::uint8_t> bson = utils::json::to_ubjson(start);
    std::string bson_str = utils::string::join(bson);
    REQUIRE(bson.size() > 0);
    #ifdef TEST_JSON_LOG_STEPS
        utils::Logger::Stream(bson_str, utils::Logger::CRLF, utils::Logger::LINE<>);
    #endif

    std::string encoded = utils::string::to_base64(bson.data(), bson.size());
    CHECK(utils::string::is_base64(encoded));
    #ifdef TEST_JSON_LOG_STEPS
        utils::Logger::Stream(encoded, utils::Logger::CRLF, utils::Logger::LINE<>);
    #endif

    std::string decoded = utils::string::from_base64(encoded);
    CHECK(decoded == bson_str);
    #ifdef TEST_JSON_LOG_STEPS
        utils::Logger::Stream(decoded, utils::Logger::CRLF, utils::Logger::LINE<>);
    #endif

    auto jsbson = utils::json::from_ubjson(decoded);
    CHECK(jsbson == start);
    #ifdef TEST_JSON_LOG_STEPS
        utils::Logger::Stream(std::setw(4), jsbson, utils::Logger::CRLF, utils::Logger::LINE<>);
    #endif
}

#endif
