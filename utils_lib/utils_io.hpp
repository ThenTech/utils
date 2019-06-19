#ifndef UTILS_IO_HPP
#define UTILS_IO_HPP

#include "external/mio.hpp"
#include "utils_exceptions.hpp"
#include "utils_bits.hpp"
#include "utils_string.hpp"
#include "utils_memory.hpp"
#include "utils_traits.hpp"

#include <fstream>
#include <vector>
#include <chrono>

/**
 *  Enable FileSystem
 *  Currently only included when linking with -lstdc++fs.
 */
#if __has_include(<filesystem>)
    #include <filesystem>
    #define UTILS_IO_FS_SUPPORTED
#elif __has_include(<experimental/filesystem>)
    #include <experimental/filesystem>
    #define UTILS_IO_FS_SUPPORTED
#endif


namespace utils::io {
    /**
     *  Wrap mio as ::mio (memory mapping file io)
     *  Reference: https://github.com/mandreyel/mio (10/06/2019)
     */
    namespace mio = ::mio;

    #ifdef UTILS_IO_FS_SUPPORTED
        #ifdef __cpp_lib_experimental_filesystem
            namespace fs = std::experimental::filesystem;
        #else
            namespace fs = std::filesystem;
        #endif

        namespace filter {
            struct all {
                inline bool operator()(const fs::directory_entry&) const {
                    return true;
                }
            };

            struct file {
                inline bool operator()(const fs::directory_entry& entry) const {
                    return entry.status().type() == fs::file_type::regular;
                }
            };

            struct directory {
                inline bool operator()(const fs::directory_entry& entry) const {
                    return entry.status().type() == fs::file_type::directory;
                }
            };

            using filter_t = decltype(utils::io::filter::all());
        }

        template <
            typename TFilter = filter::filter_t
        > ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static auto list_contents(const std::string_view& folder,
                                  TFilter predicate = utils::io::filter::all())
        {
            static_assert(std::is_invocable_v<decltype(predicate), fs::directory_entry&>,
                          "utils::io::list_contents: Callable function required.");

			const fs::path path{ folder.begin(), folder.end() };
            auto contents = utils::memory::new_unique_var<std::vector<std::string>>();

            for (const auto& entry : fs::directory_iterator(path)) {
                if (predicate(entry))
                    contents->emplace_back(entry.path().string());
            }

            return contents;
        }

        template <
            typename TFilter = filter::filter_t
        > ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static auto list_contents_recur(const std::string_view& folder,
                                        TFilter predicate = utils::io::filter::all())
        {
            static_assert(std::is_invocable_v<decltype(predicate), fs::directory_entry&>,
                          "utils::io::list_contents_recur: Callable function required.");

			const fs::path path{ folder.begin(), folder.end() };
            auto contents = utils::memory::new_unique_var<std::vector<std::string>>();

            for (const auto& entry : fs::recursive_directory_iterator(path)) {
                if (predicate(entry))
                    contents->emplace_back(entry.path().string());
            }

            return contents;
        }

        ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static inline auto list_files(const std::string_view& folder) {
            return list_contents(folder, utils::io::filter::file());
        }

        ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static inline auto list_folders(const std::string_view& folder) {
            return list_contents(folder, utils::io::filter::directory());
        }

        ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static uint64_t file_size(const std::string_view& filename) {
            const fs::path path{ filename.begin(), filename.end() };

            if (fs::exists(path) && fs::is_regular_file(path)) {
                return fs::file_size(path);
            }

            return uint64_t(-1);
        }

        ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static std::time_t file_last_modified(const std::string_view& filename) {
            const fs::path path{ filename.begin(), filename.end() };

            if (fs::exists(path)) {
                const auto modified = fs::last_write_time(path);

                // FIXME: No clock casting possible
                // Could use:
                //      decltype(modified)::clock::to_time_t(modified);
                // instead, but clocks are different.
                // C++20 will provide clock_cast to fix this.

                return std::chrono::duration_cast<std::chrono::seconds>(modified.time_since_epoch()).count();
            }

            return std::time_t(0);
        }

        ATTR_MAYBE_UNUSED ATTR_NODISCARD
        static const std::string most_recent_file(const std::vector<std::string>& paths) {
            std::string most_recent("");

            if (paths.size()) {
                std::time_t last_date(0);

                for (const std::string& file : paths) {
                    if (fs::is_regular_file(file))
                    if (std::time_t mdate = utils::io::file_last_modified(file);
                        mdate >= last_date)
                    {
                        last_date = mdate;
                        most_recent = file;
                    }
                }
            }

            return most_recent;
        }
    #endif  // UTILS_IO_FS_SUPPORTED

    ATTR_MAYBE_UNUSED
    static void safe_filename(std::string& filename) {
        static constexpr std::string_view invalid_chars = "\"<>?*|/:\\\n";

        for (const char& c : invalid_chars) {
            utils::string::erase_all(filename, c);
        }
    }

    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static std::string safe_filename(const std::string_view& filename) {
        std::string str(filename);
        utils::io::safe_filename(str);
        return str;
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
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static auto file_to_string(const std::string& filename) {
        auto str = utils::memory::new_unique_var<std::string>();
        std::ifstream file;

        try {
            file.open(filename, std::fstream::in | std::fstream::ate);

            str->reserve(size_t(file.tellg()));
            file.seekg(0, std::ios::beg);

            str->assign(std::istreambuf_iterator<char>(file),
                        std::istreambuf_iterator<char>());
            file.close();
        } catch (...) {
            file.close();
            throw utils::exceptions::FileReadException(filename);
        }

        return str;
    }

    /**
     *	\brief TODO comment and test
     *	\param filename
     *	\param str
     */
    ATTR_MAYBE_UNUSED
    static void string_to_file(const std::string& filename, const std::string_view& str) {
        std::ofstream file;

        try {
            file.open(filename);
            file << str;
            file.close();
        } catch (...) {
            file.close();
            throw utils::exceptions::FileWriteException(filename);
        }
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
    ATTR_MAYBE_UNUSED ATTR_NODISCARD
    static auto file_to_bytes(const std::string& filename) {
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
    ATTR_MAYBE_UNUSED
    static void bytes_to_file(const std::string& filename, const uint8_t* buffer, size_t length) {
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

            static constexpr inline uint8_t bitmasks[] = { 255, 128, 192, 224, 240, 248, 252, 254 };

            UTILS_TRAITS_ADD_PADDING(uint8_t, 8)

        public:
            BitStream(uint8_t *b = nullptr, size_t s = 0, size_t p = 0, bool m = false)
                : buffer(b), size(s), position(p), managed(m) {}

            virtual ~BitStream(void) {
                if (this->managed) {
                    utils::memory::delete_array(this->buffer);
                }
            }

            inline uint8_t* get_buffer(void) {
                return this->buffer;
            }

            inline const uint8_t* get_buffer() const {
                return this->buffer;
            }

            inline uint8_t operator[](size_t idx) const {
                return this->buffer[idx];
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

                    utils::memory::realloc_array(this->buffer, this->size, new_size);
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

            template <
                typename Iterator,
                typename = typename std::enable_if_t<
                        utils::traits::is_iterator_v<Iterator>
                     && std::is_same_v<uint8_t, typename std::iterator_traits<Iterator>::value_type>>
            >
            BitStreamReader(Iterator start, Iterator end)
                : BitStream(nullptr, 0, 0, true)
            {
                this->resize(size_t(std::distance(start, end)));
                std::copy(start, end, this->get_buffer());
            }

            template<
                typename Container,
                typename = typename std::enable_if_t<utils::traits::is_iterable_v<Container>>
            >
            BitStreamReader(const Container& cont)
                : BitStreamReader(std::begin(cont), std::end(cont))
            {
                // Empty
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
                    auto invec = utils::io::file_to_bytes(filename);
                    reader.reset(utils::memory::new_var<BitStreamReader>(*invec));
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
                : BitStream(utils::memory::new_array<uint8_t>(s), s, 0, true)
            {
                // Empty
            }

            ~BitStreamWriter() {}

            /**
             * Write one bit into the bitstream.
             * @param [in] value The value to put into the bitstream.
             */
            void put_bit(int8_t value) {
                const size_t current_start_byte = this->position / 8u;
                const size_t bits_taken = this->position % 8;

                if (value) {
                    this->buffer[current_start_byte] |= 1 << (7 - bits_taken);
                } else {
                    this->buffer[current_start_byte] &= ~(1 << (7 - bits_taken));
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
                utils::io::bytes_to_file(filename,
                                         this->get_buffer(),
                                         this->get_last_byte_position());
            }
    };
}

#endif // UTILS_IO_HPP
