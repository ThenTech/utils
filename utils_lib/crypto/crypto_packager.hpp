#ifndef CRYPTO_PACKAGER_HPP
#define CRYPTO_PACKAGER_HPP

#include "../utils_bits.hpp"
#include "../utils_io.hpp"
#include "../utils_memory.hpp"
#include "../utils_json.hpp"
#include "../utils_string.hpp"
#include "../algo/algo_huffman.hpp"
#include "../crypto/crypto_aes.hpp"

namespace utils::crypto {
    struct IPackageStrategy {
        virtual ~IPackageStrategy();
    };

    struct PackageJSON : public IPackageStrategy {
        utils::io::BitStreamWriter Pack(const utils::json& data) {
            const auto bson    = utils::json::to_ubjson(data);
            const auto encoded = utils::string::to_base64(bson.data(), bson.size());

            utils::io::BitStreamWriter out(encoded.size());
            std::copy(encoded.begin(), encoded.end(), out.get_buffer());

            return out;
        }

        utils::json Unpack(utils::io::BitStreamReader& reader) {
            const auto decoded = utils::string::from_base64(reader.get_buffer(), reader.get_size());
            return utils::json::from_ubjson(decoded);
        }
    };

    struct HMCompress : public IPackageStrategy {
        utils::io::BitStreamWriter Pack(utils::io::BitStreamReader& reader) {
            utils::algo::Huffman hm;
            auto writer = hm.encode(reader);

            if (writer) {
                utils::io::BitStreamWriter out(writer->get_last_byte_position());
                std::copy_n(writer->get_buffer(), out.get_size(), out.get_buffer());
                return out;
            } else {
                return {nullptr, 0};
            }
        }

        utils::io::BitStreamReader Unpack(utils::io::BitStreamReader& reader) {
            algo::Huffman hm;
            auto writer = hm.decode(reader);

            if (writer) {
                return { writer->get_buffer(),
                         writer->get_buffer() + writer->get_size()};
            } else {
                return {nullptr, 0};
            }
        }
    };

    template<size_t KeySize = 256, size_t KeyBytes = KeySize / utils::bits::size_of<uint8_t>()>
    struct EncipherAES : public IPackageStrategy {
        static inline constexpr size_t len_bits = utils::bits::size_of<uint32_t>();

        utils::io::BitStreamWriter Pack(utils::io::BitStreamReader& reader, const std::array<uint8_t, KeyBytes>& key) {
            utils::crypto::AES aes(KeySize);
            uint32_t out_len = 0;
            const auto enc = aes.EncryptECB(reader.get_buffer(), reader.get_size(), key.data(), out_len);

            utils::io::BitStreamWriter out(out_len + len_bits);
            out.put(len_bits, out_len);
            std::copy_n(enc.get(), out_len, out.get_buffer() + len_bits / 8);

            return out;
        }

        utils::io::BitStreamReader Unpack(utils::io::BitStreamReader& reader, const std::array<uint8_t, KeyBytes>& key) {
            utils::crypto::AES aes(KeySize);
            uint32_t out_len = reader.get(len_bits);
            auto dec = aes.DecryptECB(reader.get_buffer() + len_bits / 8, reader.get_size(), key.data());
            return {dec.get(), dec.get() + out_len};
        }
    };
}

#endif // CRYPTO_PACKAGER_HPP
