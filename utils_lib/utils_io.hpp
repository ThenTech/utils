#ifndef UTILS_IO_HPP
#define UTILS_IO_HPP

#include "utils_exceptions.hpp"
#include "utils_bits.hpp"
#include "utils_string.hpp"
#include "utils_memory.hpp"

#include <fstream>
#include <vector>

/**
 *  Enable FileSystem
 *  Currently only included when linking with -lstdc++fs.
 */
#if __cplusplus <= 201703L
    #include <experimental/filesystem>
#else
    #include <filesystem>
#endif

namespace utils::io {
    #ifdef __cpp_lib_experimental_filesystem
        namespace fs = std::experimental::filesystem;
    #else
        namespace fs = std::filesystem;
    #endif

    [[maybe_unused]]
    static void safe_filename(std::string& filename) {
        static constexpr std::string_view invalid_chars = "\"<>?!*|/:\\\n";

        for (const char& c : invalid_chars) {
            utils::string::strReplaceAll(filename, c);
        }
    }

    [[maybe_unused]]
    static std::string safe_filename(const std::string& filename) {
        std::string str(filename);
        utils::io::safe_filename(str);
        return str;
    }

    [[maybe_unused]]
    static auto list_contents(const std::string& folder) {
        auto contents = utils::memory::new_unique_var<std::vector<std::string>>();

        for (const auto& entry : fs::directory_iterator(folder)) {
            contents->emplace_back(entry.path().string());
        }

        return contents;
    }

    /**	\brief	Read the given file and return a pointer to a string containing its contents.
     *
     *	\param	filename
     *		The (path and) name of the file to read.
     *
     *	\return	Returns a string pointer.
     *
     *	\exception	FileReadException
     *		Throws FileReadException if the file could not be read properly.
     */
    [[maybe_unused]]
    static auto readStringFromFile(const std::string& filename) {
        auto str = utils::memory::new_unique_var<std::string>();
        std::ifstream file(filename, std::fstream::in | std::fstream::ate);

        try {
            str->reserve(size_t(file.tellg()));
            file.seekg(0, std::ios::beg);

            str->assign(std::istreambuf_iterator<char>(file),
                        std::istreambuf_iterator<char>());
        } catch (...) {
            file.close();
            throw utils::exceptions::FileReadException(filename);
        }

        file.close();
        return str;
    }

    /**
     *	\brief TO-DO
     *	\param filename
     *	\param str
     */
    [[maybe_unused]] static void writeStringToFile(const std::string& filename, const std::string& str) {
        std::ofstream file(filename);

        try {
            file << str;
        } catch (...) {
            file.close();
            throw utils::exceptions::FileWriteException(filename);
        }

        file.close();
    }

    /**
     *	\brief	Read the given file and return a pointer to a list containing its contents in binary data (raw chars).
     *
     *	\param	filename
     *		The (path and) name of the file to read.
     *
     *	\return	std::vector<char>*
     *			A vector with buffer->size() bytes containing a program in raw binary.
     *			Print with cast to (unsigned char) for proper viewing.
     *
     *	\exception	FileReadException
     *		Throws FileReadException if the file could not be read properly.
     */
    [[maybe_unused]]
    static auto readBinaryFile(const std::string& filename) {
        std::ifstream file(filename, std::ifstream::binary | std::ifstream::ate);

        if (!file.good()) {
            file.close();
            throw utils::exceptions::FileReadException(filename);
        }

        auto v_buff = utils::memory::new_unique_var<std::vector<uint8_t>>();

        try {
            // Filepointer is already at end due to ::ate option, so tellg() gives filesize
            v_buff->reserve(size_t(file.tellg()));
            file.seekg(0, std::ios::beg);
            v_buff->assign(std::istreambuf_iterator<char>(file),
                           std::istreambuf_iterator<char>());
        } catch (...) {
            file.close();
            throw utils::exceptions::FileReadException(filename);
        }

        file.close();
        return v_buff;
    }

    /**
     *	\brief	Write the given char buffer to the given file.
     *
     *	\param	filename
     *		The (path and) name of the file to write to (will be created if it does not exist).
     *	\param	buffer
     *		The char buffer to write to a file.
     *	\param	length
     *		The length of the given char buffer.
     *
     *	\exception	FileWriteException
     *		Throws FileWriteException if the file could not be written properly.
     */
    [[maybe_unused]] static void writeBinaryFile(const std::string& filename, const uint8_t* buffer, size_t length) {
        std::ofstream file(filename, std::ofstream::binary);

        try {
            file.write(reinterpret_cast<const char*>(buffer), std::streamsize(length));
        } catch (...) {
            file.close();
            throw utils::exceptions::FileWriteException(filename);
        }

        file.close();
    }

    ////////////////////////////////////////////////////////////////////////////

    class BitStream {
        protected:
            uint8_t *buffer;
            size_t  size;        ///< Size in bytes
            size_t  position;    ///< Position in bits
            bool    managed;

            static constexpr uint8_t bitmasks[] = { 255, 128, 192, 224, 240, 248, 252, 254 };

        public:
            BitStream(uint8_t *b = nullptr, size_t s = 0, size_t p = 0, bool m = false)
                : buffer(b), size(s), position(p), managed(m) {}

            virtual ~BitStream(void) {
                if (this->managed) {
                    utils::memory::deallocArray(this->buffer);
                }
            }

            inline uint8_t* get_buffer(void) {
                return this->buffer;
            }

            inline const uint8_t* get_buffer() const {
                return this->buffer;
            }

            inline size_t get_size(void) const {
                return this->size;
            }

            inline size_t get_size_bits(void) const {
                return this->size * 8u;
            }

            inline void set_managed(bool m) {
                this->managed = m;
            }

            inline void set_position(size_t p) {
                this->position = p;
            }

            inline size_t get_position(void) const {
                return this->position;
            }

            inline size_t get_last_byte_position(void) const {
                return utils::bits::round_to_byte(this->position);
            }

            inline void reset(void) {
                this->set_position(0);
            }

            /**
             * Move the bitwise position pointer to the next byte boundary
             */
            virtual inline void flush() {
                this->position = this->get_last_byte_position() * 8u;
            }

            /**
             *  @brief  Resize the internal buffer if needed.
             *          Default resize is by 50% increase.
             *  @param  new_size
             *      The new size for the buffer.
             *  @return Returns the size after resizing.
             */
            size_t resize(size_t new_size=0u) {
                if (this->managed /*&& this->malloc*/) {
                    if (new_size == 0) {
                        // Resize by 50%
                        new_size = this->size + this->size / 2u;
                    } else if (new_size <= this->get_size()) {
                        return this->get_size();
                    }

                    utils::memory::reallocArray(this->buffer, this->size, new_size);
                }

                return this->get_size();
            }
    };

    /**
     * Class which eases reading bitwise from a buffer.
     */
    class BitStreamReader : public BitStream {
        public:
            /**
             * Create a bitstreamreader which reads from the provided buffer.
             *
             * @param [in] buffer The buffer from which bits will be read.
             * @param [in] size The size (expressed in bytes) of the buffer from which bits will be read.
             */
            BitStreamReader(uint8_t *buffer, size_t size)
                : BitStream(buffer, size, 0, false)
            {
                // Empty
            }

            BitStreamReader(std::vector<uint8_t> const& src)
                : BitStream(utils::memory::allocArray<uint8_t>(src.size()), src.size(), 0, true)
            {
                std::copy(src.begin(), src.end(), this->get_buffer());
            }

            ~BitStreamReader() {}

            /**
             * Read one bit from the bitstream.
             *
             * @return The value of the bit.
             */
            uint8_t get_bit(void) {
                const size_t current_start_byte = this->position / 8u;

                if (current_start_byte >= this->get_size()) {
                    // Prevent reading byte outside of array (-> Valgrind flagged)
                    return 0u;
                }

                const size_t  bits_taken = this->position % 8;
                const uint8_t value      = this->buffer[current_start_byte];

                this->position++;

                return (value & (1 << (7 - bits_taken))) != 0;
            }

            /**
             * Get l bits from the bitstream
             *
             * @param [in] l number of bits to read
             * @return The value of the bits read
             *
             * buffer: 0101 1100, position==0
             * get(4) returns value 5, position==4
             */
            uint32_t get(size_t l) {
                uint32_t value = 0;

                for (size_t i = 0; i < l; i++) {
                    const uint32_t v = this->get_bit();
                    value |= v << (l - i - 1);
                }

                return value;
            }

            /**
             * @brief from_file
             * @param filename
             * @return
             */
            static auto from_file(const std::string& filename) {
                utils::memory::unique_t<BitStreamReader> reader;

                try {
                    auto invec = utils::io::readBinaryFile(filename);
                    reader.reset(utils::memory::allocVar<BitStreamReader>(*invec));
                } catch (utils::exceptions::FileReadException const& e) {
                    reader.reset(nullptr);
                    throw e;
                }

                return reader;
            }
    };

    /**
     * Class which eases writing bitwise into a buffer.
     */
    class BitStreamWriter : public BitStream {
        public:
            /**
             * Create a bitstreamwriter which writes into the provided buffer.
             *
             * @param [in] buffer The buffer into which bits will be written.
             * @param [in] size The size (expressed in bytes) of the buffer into which bits will be written.
             */
            BitStreamWriter(uint8_t *buffer, size_t size)
                : BitStream(buffer, size, 0, false)
            {
                // Empty
            }

            /**
             * Create a bitstreamwriter and allocate a buffer for it.
             *
             * @param [in] size The size (expressed in bytes) of the buffer into which bits will be written.
             */
            BitStreamWriter(size_t s)
                : BitStream(utils::memory::allocArray<uint8_t>(s), s, 0, true)
            {
                // Empty
            }

            ~BitStreamWriter() {}

            /**
             * Write one bit into the bitstream.
             * @param [in] value The value to put into the bitstream.
             */
            void put_bit(int8_t value) {
                const size_t bits_taken = this->position % 8;

                if (value) {
                    this->buffer[this->position / 8] |= 1 << (7 - bits_taken);
                } else {
                    this->buffer[this->position / 8] &= ~(1 << (7 - bits_taken));
                }

                this->position++;
            }

            /**
             * Put 'length' bits with value 'value' into the bitstream
             *
             * @param [in] length Number of bits to use for storing the value
             * @param [in] value The value to store
             *
             * buffer: xxxx xxxx, position==0
             * put(4, 5)
             * buffer: 1010 xxxx, position==4
             */
            void put(size_t length, uint32_t value) {
                for (size_t p = 0; p < length; p++) {
                    put_bit(1 & (value >> (length - 1 - p)));
                }
            }

            /**
             * Byte-align: Move the bitwise position pointer to the next byte boundary
             */
            void flush(void) {
                // Only keep written bits in current byte, make rest 0
                this->buffer[this->position / 8] &= BitStream::bitmasks[this->position % 8];

                BitStream::flush();
            }

            void write_to_file(const std::string &filename) {
                utils::io::writeBinaryFile(filename,
                                           this->get_buffer(),
                                           this->get_last_byte_position());
            }
    };
}

#endif // UTILS_IO_HPP
