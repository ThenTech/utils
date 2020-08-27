#ifndef ALGO_FEISTEL_HPP
#define ALGO_FEISTEL_HPP

#include "../utils_memory.hpp"
#include "../utils_bits.hpp"
#include "../utils_logger.hpp"
#include "../utils_io.hpp"
#include "../utils_random.hpp"
#include "../utils_crc.hpp"

#include <cstdint>
#include <array>

namespace utils::crypto {

    // Simple weak hash, probably not secure
    struct SimpleHash {
        uint32_t operator()(uint32_t block, uint64_t key) {
            const auto crc = utils::CRC::Calculate(&block, 4, this->table);
            return utils::CRC::Calculate(&key, 8, this->table, crc);
        }

        private:
            static inline const utils::CRC::Table<uint32_t, 32> table{utils::CRC::CRC_32()};
    };

    /**
     *  Adapted from https://gist.github.com/edigaryev/1500464
     *  Requires 64-bit blocks in input data.
     */
    template <size_t rounds = 3, typename Hasher = SimpleHash>
    class FeistelCipher {
        private:
            static_assert (rounds > 0, "[FeistelCipher] Number of rounds must be at least 1!");

            std::array<uint64_t, rounds> keys;

            static inline constexpr std::array<uint64_t, 3> default_keys {
                0x203A85B108660000,
                0x000001B3AB6B6935,
                0x000000FF9A056000,
            };

            static inline constexpr CRC::Parameters<uint64_t, 64> CRC_64 = {
                0x42F0E1EBA9EA3693, 0x0000000000000000, 0x0000000000000000, false, false
            };

            void convert_keys(const uint64_t *init_key, const size_t length) {
                if (length == rounds) {
                    std::copy(init_key, init_key + length, this->keys.data());
                } else {
                    static const utils::CRC::Table<uint64_t, 64> table(CRC_64);

                    if (length < rounds) {
                        // Create extra values to fill rounds
                        std::copy(init_key, init_key + length, this->keys.data());

                        const uint64_t crc = utils::CRC::Calculate(init_key, length, table);
                        auto&& rnd_seed = std::seed_seq{crc, default_keys[0], default_keys[1], default_keys[2]};
                        constexpr uint64_t max = static_cast<uint64_t>(-1);

                        utils::random::RandomLocal r;
                        r.seed(rnd_seed);

                        for (size_t i = length; i < rounds; ++i) {
                            this->keys[i] = r.get(uint64_t{}, max);
                        }
                    } else {
                        // Consume extra values to fit rounds
                        std::copy(init_key, init_key + rounds, this->keys.data());
                        this->keys.back() = utils::CRC::Calculate(init_key + rounds - 1, length - rounds + 1, table);
                    }
                }
            }

            uint64_t encrypt(uint32_t left, uint32_t right) const {
                Hasher h{};

                for (size_t round = 0; round < rounds; ++round) {
                    const uint32_t prev_right = right;
                    right = left;
                    left  = h(left, this->keys[round]) ^ prev_right;
                }

                // Swap a last time
                return (uint64_t{right} << 32) | left;
            }

            uint64_t decrypt(uint32_t left, uint32_t right) const {
                Hasher h{};

                for (size_t round = rounds; round > 0; --round) {
                    const uint32_t prev_right = right;
                    right = left;
                    left  = h(left, this->keys[round-1]) ^ prev_right;
                }

                // Swap a last time
                return (uint64_t{right} << 32) | left;
            }

        public:
            FeistelCipher() {
                this->convert_keys(default_keys.data(), default_keys.size());
            }

            // TODO Use std::span here instead
            FeistelCipher(const std::array<uint64_t, rounds>& keys) {
                this->convert_keys(keys.data(), keys.size());
            }

            FeistelCipher(const uint64_t* keys, const size_t length) {
                this->convert_keys(keys, length);
            }

            FeistelCipher(const uint8_t *init_key, const size_t length) {
                const size_t len = std::max(length / sizeof(uint64_t), size_t(1ull));
                auto stream = utils::memory::new_unique_array<uint64_t>(len);
                std::memcpy(stream.get(), init_key, length);

                this->convert_keys(stream.get(), len);
            }

            ~FeistelCipher() {}

            const std::array<uint64_t, rounds>& get_state() const {
                return this->keys;
            }

            utils::memory::unique_t<utils::io::BitStreamWriter> encode(utils::io::BitStreamReader& reader) const {
                const size_t length          = reader.get_size_bits();
                const size_t original_length = reader.get_size();

                utils::memory::unique_t<utils::io::BitStreamWriter> writer;

                if (length == 0 || length % 64 > 0) {
                    // Nothing to encode or not in 64 bit blocks
                    writer.reset(nullptr);
                    return writer;
                }

                writer.reset(utils::memory::new_var<utils::io::BitStreamWriter>(original_length));

                reader.reset();
                while (reader.get_position() != length) {
                    const auto left  = reader.get(32);
                    const auto right = reader.get(32);
                    const uint64_t enc = this->encrypt(left, right);
                    writer->put(32, utils::bits::select_msb(enc, 32));
                    writer->put(32, utils::bits::select_lsb(enc, 32));
                }

                return writer;
            }

            utils::memory::unique_t<utils::io::BitStreamWriter> decode(utils::io::BitStreamReader& reader) const {
                const size_t length          = reader.get_size_bits();
                const size_t original_length = reader.get_size();

                utils::memory::unique_t<utils::io::BitStreamWriter> writer;

                if (length == 0 || length % 64 > 0) {
                    // Nothing to encode or not in 64 bit blocks
                    writer.reset(nullptr);
                    return writer;
                }

                writer.reset(utils::memory::new_var<utils::io::BitStreamWriter>(original_length));

                reader.reset();
                while (reader.get_position() != length) {
                    const auto left  = reader.get(32);
                    const auto right = reader.get(32);
                    const uint64_t dec = this->decrypt(left, right);
                    writer->put(32, utils::bits::select_msb(dec, 32));
                    writer->put(32, utils::bits::select_lsb(dec, 32));
                }

                return writer;
            }

            // TODO Use std::span here for key array instead
            static bool encode(const std::string& rawfile, const std::string& encfile,
                               const uint64_t* keys = default_keys.data(), const size_t length = default_keys.size())
            {
                try {
                    auto enc = utils::io::BitStreamReader::from_file(rawfile);

                    crypto::FeistelCipher<rounds> fc{keys, length};
                    auto writer = fc.encode(*enc);

                    if (writer) {
                        utils::io::bytes_to_file(encfile,
                                                 writer->get_buffer(),
                                                 writer->get_last_byte_position());
                    } else {
                        utils::Logger::Warn("[FeistelCipher] Nothing to encode! Check contents of '%s'\n", rawfile.c_str());
                        return false;
                    }

                    return true;
                } catch (utils::exceptions::FileReadException const& e) {
                    utils::Logger::Error(e.getMessage());
                }

                return false;
            }

            // TODO Use std::span here for key array instead
            static bool decode(const std::string& encfile, const std::string& decfile,
                               const uint64_t* keys = default_keys.data(), const size_t length = default_keys.size())
            {
                try {
                    auto enc = utils::io::BitStreamReader::from_file(encfile);

                    crypto::FeistelCipher<rounds> fc{keys, length};
                    auto writer = fc.decode(*enc);

                    if (writer) {
                        utils::io::bytes_to_file(decfile,
                                                 writer->get_buffer(),
                                                 writer->get_size());
                    } else {
                        utils::Logger::Warn("[FeistelCipher] Nothing to decode! Check contents of '%s'\n", encfile.c_str());
                        return false;
                    }

                    return true;
                } catch (utils::exceptions::FileReadException const& e) {
                    utils::Logger::Error(e.getMessage());
                }

                return false;
            }
    };
}

#endif // ALGO_FEISTEL_HPP
