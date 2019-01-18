#include "utils_logger.hpp"

/**
 *  Symbols for printing an image to the console.
 *
 *  █
 *  std::string_format("%c", 219)
 *  219
 */
const std::string utils::Logger::FILL  = "#";
const std::string utils::Logger::EMPTY = " ";
const std::string utils::Logger::CRLF  = "\r\n";

utils::Logger utils::Logger::instance;
