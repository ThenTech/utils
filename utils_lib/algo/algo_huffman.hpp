#ifndef HUFFMAN_HPP
#define HUFFMAN_HPP

#include "../utils_memory.hpp"
#include "../utils_logger.hpp"
#include "../utils_io.hpp"

#include <cstdint>
#include <numeric>
#include <unordered_map>
#include <queue>
#include <vector>

namespace utils::algo {

    /**
     *  @brief  Huffman Node class
     */
    template<class T=uint8_t>
    class Node {
        public:
            const T       data;     ///< The actual data in this node.
            const size_t  freq;     ///< The frequency at which this data occured.
            Node         *left;     ///< A reference to child on the left ('0')
            Node         *right;    ///< A reference to child on the right ('1')

            /**
             *  @brief  Default ctor
             *
             *  @param  data
             *      Initial data in this node.
             *  @param  freq
             *      Initial frequency of this data element.
             *  @param  left
             *      A reference to the left child.
             *  @param  right
             *      A reference to the right child.
             */
            Node(const T& data, size_t freq=1u, Node *left=nullptr, Node *right=nullptr)
                : data(data), freq(freq), left(left), right(right)
            {
                // Empty
            }

            /**
             *  @brief  Default dtor
             *          Deleting children will implicitly delete entire tree
             *          by recursive dtor calls on every Node.
             */
            ~Node(void) {
                utils::memory::delete_var(left);
                utils::memory::delete_var(right);
            }

            /**
             *  @brief  Returns true if this Node has no children,
             *          and therefor is a leaf.
             */
            inline bool isLeaf(void) const {
                return this->left == nullptr && this->right == nullptr;
            }

            /**
             *  @brief  Comparison operator.
             *          Nodes with lower frequency has a higher priority.
             *
             *  @param  first
             *      The first Node to compare with.
             *  @param  second
             *      The second Node to compare with.
             *  @return Returns true if current Node has higher frequency.
             */
            struct comparator {
                inline bool operator()(const Node<T> * const first, const Node<T> * const second) {
                    return first->freq > second->freq;
                }
            };

            /**
             *  @brief  Print a tree structure starting from the given Node.
             *  @param  node
             *      The Node to start from.
             *  @param  s
             *      A string representation of the tree path ('0' for left and '1' for right)
             */
            static void printTree(const algo::Node<T> * const node, std::string s = "") {
                if (node == nullptr) {
                    return;
                }

                if (node->isLeaf()) {
                    utils::Logger::Writef(s + " => %X" + utils::Logger::CRLF, node->data);
                    return;
                }

                printTree(node->left , s + "0");
                printTree(node->right, s + "1");
            }
    };

    /**
     *  Data struct for Huffman dictionary entries.
     */
    struct Codeword {
        uint32_t word;
        uint32_t len;
    };

    /**
     *  @brief Huffman class
     */
    template<class T=uint8_t>
    class Huffman {
        public:
            using KeyPair = std::pair<T, Codeword>;

        private:
            algo::Node<T> *tree_root;

            std::unordered_map<T, Codeword> dict;

            /**
             *  @brief  Add the given settings to the output stream according to the amount of bits
             *          specified in the Huffman class.
             *  @param  length
             *      The length of the sequence that will follow this header.
             *      If the length is 0, only one '0' bit will be written.
             *  @param  bit_length
             *      The amount of bits needed for every data element in the sequence following this header.
             *      Keys always use KEY_BITS as length, and values use bit_length, which is different for each group.
             *      This is done to minimize the amoutn of bits needed to save the Huffman dictionary.
             *  @param  writer
             *      The outputstream to write to.
             */
            static void add_huffman_dict_header(uint32_t length, uint32_t bit_length, utils::io::BitStreamWriter& writer) {
                if (length > 0) {
                    writer.put(algo::Huffman<T>::DICT_HDR_HAS_ITEMS_BITS + algo::Huffman<T>::DICT_HDR_SEQ_LENGTH_BITS,
                               0x80 | (length & 0x7F)); // MSB is HAS_ITEMS setting + 7 bits length
                    writer.put(algo::Huffman<T>::DICT_HDR_ITEM_BITS,
                               bit_length & 0xF);       // 4 bits for bit length of every dict item
                } else {
                    writer.put_bit(0);
                }
            }

            /**
             *  @brief  Read a dictionary header from the inputstream and set the given variables.
             *
             *  @param  reader
             *      The inputstream to read from.
             *  @param  length
             *      The length of the sequence that will follow this header (will be set).
             *  @param  bit_length
             *      The amount of bits for every value element in the following sequence (will be set).
             *
             *  @return Returns true if there is data after this header. (first bit was set)
             */
            static bool read_huffman_dict_header(utils::io::BitStreamReader& reader, uint32_t& length, uint32_t& bit_length) {
                if (reader.get_bit()) {
                    length     = reader.get(algo::Huffman<T>::DICT_HDR_SEQ_LENGTH_BITS);
                    bit_length = reader.get(algo::Huffman<T>::DICT_HDR_ITEM_BITS);
                    return true;
                }

                return false;
            }

            /**
             *  @brief  Traverse Huffman tree starting from node and
             *          add codes for leafs to the dictionary.
             *  @param  node
             *      The starting node.
             *  @param  stream
             *      The current stream of bits for a path in the tree.
             */
            void buildDict(const algo::Node<T> * const node, std::vector<bool> stream) {
                if (node == nullptr) {
                    return;
                }

                // Check if leaf
                if (node->isLeaf()) {
                    this->dict[node->data] = Codeword {
                        // Concatenate the bits in stream to a value
                        utils::bits::to_binary<uint32_t>(stream),
                        // Get the amount of bits
                        uint32_t(stream.size())
                    };

                    return;
                }

                std::vector<bool> lstream(stream);
                lstream.emplace_back(false);   // Go left  => '0'
                stream.emplace_back(true);     // Go right => '1'

                this->buildDict(node->left , lstream);
                this->buildDict(node->right, stream);
            }

            /**
             *  @brief  Read the dictionary from the given stream to build a Huffman tree structure.
             *          Clear the previous tree and overwrite with the data from the dict.
             *
             *          Optionally also save dict internally, but only tree is needed for
             *          proper decoding.
             *
             *          If first bit was '0', no key: val sequence follows and reader
             *          already contains uncompressed data.
             *
             *  @param  reader
             *      The stream to read from.
             */
            void buildTree(utils::io::BitStreamReader& reader) {
                uint32_t dseq_len = 0u, dbit_len = 0u;

                utils::memory::delete_var(this->tree_root);
                this->tree_root = utils::memory::new_var<algo::Node<T>>(-1);
                this->dict.clear();

                // While header is followed by sequence
                while (this->read_huffman_dict_header(reader, dseq_len, dbit_len)) {
                    while (dseq_len--) {
                        // For each element, read {key: val}
                        const KeyPair entry {
                            T(reader.get(algo::Huffman<T>::KEY_BITS)),
                            Codeword{ reader.get(dbit_len), dbit_len }
                        };

                        #if 0  // Optionally add to dict, but not necessary since decoding only needs tree.
                            dict.insert(entry);
                            // util::Logger::WriteLn(utils::string::format("%02X: %8X (%d bits)", entry.first, entry.second.word, entry.second.len));
                        #endif
                        this->treeAddLeaf(entry);
                    }
                }
            }

            /**
             *  @brief  Add a single <T, Codeword> leaf to the current tree.
             *
             *  @param  pair
             *      The <T, Codeword> pair to add.
             *      Follow Codeword.word from MSB to LSB and create new Nodes on the way,
             *      ending withe a Node that has the data T.
             */
            void treeAddLeaf(const KeyPair& pair) {
                const size_t mask = (1 << (pair.second.len - 1));  // Mask the pair.second.len'th bit
                      size_t dirs = pair.second.word;              // The directions to follow in the tree

                algo::Node<T> *current = this->tree_root;

                // Grow the tree according to the dirs path, starting from MSB, except for last dir
                for (size_t bits = pair.second.len; --bits; dirs <<= 1) {
                    // Follow the direction and create a dummy Node if none exists
                    if (dirs & mask) {
                        if (current->right == nullptr)
                            current->right = utils::memory::new_var<algo::Node<T>>(-1);
                        current = current->right;
                    } else {
                        if (current->left == nullptr)
                            current->left = utils::memory::new_var<algo::Node<T>>(-1);
                        current = current->left;
                    }
                }

                // Create a new Node at the correct position (the last one in dirs)
                if (dirs & mask) {
                    current->right = utils::memory::new_var<algo::Node<T>>(pair.first);
                } else {
                    current->left  = utils::memory::new_var<algo::Node<T>>(pair.first);
                }
            }

            /**
             *  @brief  Traverse Huffman tree starting from node and
             *          decode symbols according to the dictionary.
             *  @param  node
             *      The starting node.
             *  @param  reader
             *      The bytestream to read from.
             */
            void decode(utils::io::BitStreamReader& reader, utils::io::BitStreamWriter& writer) {
                if (this->tree_root == nullptr) {
                    return;
                }

                algo::Node<T> *current = this->tree_root;

                while (!current->isLeaf()) {
                    // Read next bit and go left or right untill a leaf is reached
                    current = (reader.get_bit() ? current->right : current->left);
                }

                writer.put(algo::Huffman<T>::KEY_BITS, current->data);
            }

            /**
             *  @brief  A comparator to sort Codeword pairs by bit length.
             */
            struct CodewordComparator {
                inline bool operator()(const KeyPair& first, const KeyPair& second) {
                    return first.second.len > second.second.len;
                }
            };

        public:
            /**
             *  @brief  Default ctor
             */
            Huffman(void) : tree_root(nullptr) {
                // Empty
            }

            /**
             *  @brief  Default dtor
             */
            ~Huffman(void) {
                utils::memory::delete_var(this->tree_root);
            }

            /**
             *  @brief  Encode bits of length sizeof(T) with Huffman encoding and
             *          write the Huffman dict and the encoded data to an outputstream.
             *
             *  @param  reader
             *      The bytestream to read from.
             *  @return Returns a new bitstream with the encoded data.
             */
            utils::memory::unique_t<utils::io::BitStreamWriter> encode(utils::io::BitStreamReader& reader) {
                const size_t length          = reader.get_size_bits();
                const size_t original_length = reader.get_size();

                utils::memory::unique_t<utils::io::BitStreamWriter> writer;

                // Calculate frequencies
                std::unordered_map<T, uint32_t> freqs;

                reader.reset();
                while(reader.get_position() != length) {
                    const T word = T(reader.get(algo::Huffman<T>::KEY_BITS));
                    freqs[word]++;
                }

                if (freqs.empty()) {
                    // Nothing to encode?
                    writer.reset(nullptr);
                    return writer;
                }

                // Create priority queue to sort tree with Nodes with data from frequency
                std::priority_queue<algo::Node<T>*, std::vector<algo::Node<T>*>, typename algo::Node<T>::comparator> pq;

                for (const auto& [data, freq]: freqs) {
                    pq.emplace(utils::memory::new_var<algo::Node<T>>(data, freq));
                    // utils::Logger::Writef("%02X: %d\n", data, freq);
                }

                while (pq.size() > 1) {
                    // Empty out queue and build leaves, starting with lowest freq
                    // Result is a single Node with references to other Nodes in tree structure.
                    algo::Node<T> *left  = pq.top(); pq.pop();
                    algo::Node<T> *right = pq.top(); pq.pop();

                    pq.emplace(utils::memory::new_var<algo::Node<T>>(-1, left->freq + right->freq, left, right));
                }

                // Huffman tree root
                this->tree_root = pq.top();

                // Create dictionary by tree traversal
                this->buildDict(this->tree_root, {});

                // Create new list with dict elements sorted by bit length for saving to stream
                std::vector<KeyPair> sorted_dict(this->dict.begin(), this->dict.end());

                // Sort the dictionary by value bit length
                std::sort(sorted_dict.begin(), sorted_dict.end(), algo::Huffman<T>::CodewordComparator());

                // Determine frequencies of each bit length with {bit_length: freq}
                std::unordered_map<uint32_t, uint32_t> bit_freqs;
                for (const auto& [value, word] : sorted_dict) {
                    UNUSED(value);
                    bit_freqs[word.len]++;
                }

                // Calculate total needed length for dict
                size_t h_dict_total_length = (algo::Huffman<T>::KEY_BITS * this->dict.size())  // Amount of bits needed for keys
                                           + ((algo::Huffman<T>::DICT_HDR_HAS_ITEMS_BITS + algo::Huffman<T>::DICT_HDR_ITEM_BITS + algo::Huffman<T>::DICT_HDR_SEQ_LENGTH_BITS)
                                              * bit_freqs.size())                             // Amount of bits for each header
                                           + 1;                                               // Stop bit
                for (const auto& [bit_length, freq] : bit_freqs) {
                    h_dict_total_length += bit_length * freq;  // Amount of bits for each header group
                }

                utils::Logger::Info("[Huffman] Table overhead with %d entries: %.1f bytes.",
                                    this->dict.size(), float(h_dict_total_length) / 8.0f);

                // Save the Huffman dictionary to a stream
                writer.reset(utils::memory::new_var<utils::io::BitStreamWriter>((h_dict_total_length + length) / 8 + 1));
                uint32_t seq_len = 0u, bit_len = 0u;

                // Add headers for each group of same length key:val pairs
                // and write them to the stream
                for (const auto& [value, word] : sorted_dict) {
                    if (seq_len == 0) {
                        // New group
                        bit_len = word.len;
                        seq_len = bit_freqs[bit_len];
                        this->add_huffman_dict_header(seq_len, bit_len, *writer);
                    }

                    writer->put(algo::Huffman<T>::KEY_BITS, value);  // Put Key
                    writer->put(bit_len, word.word);                 // Put Val
                    seq_len--;
                }

                this->add_huffman_dict_header(0, 0, *writer);  // Stop bit

                // Store length in bytes of source
                // FIXME capped at 65k due to LEN_BITS being *only* 16
                //       Maybe 2 additional bits to indicate length of [0b00: 16, 0b01: 32: 0b10: 48, 0b11: 64]?
                writer->put(algo::Huffman<T>::LEN_BITS, uint16_t(original_length));

                // Encode
                reader.reset();
                while(reader.get_position() < length) {
                    const T word = T(reader.get(algo::Huffman<T>::KEY_BITS));
                    const auto& pair = this->dict[word];
                    writer->put(pair.len, pair.word);
                }

                const size_t total_length = writer->get_last_byte_position();

                utils::Logger::Info("[Huffman]           Input file size: %8d bytes", original_length);
                utils::Logger::Info("[Huffman]           Compressed size: %8d bytes  => Ratio: %.2f%%",
                                      total_length,
                                      float(total_length) / original_length * 100.0f);

                if (original_length < total_length) {
                    utils::Logger::Warn("[Huffman] No extra compression achieved, reverting stream to encoded.");

                    writer.reset(utils::memory::new_var<utils::io::BitStreamWriter>(original_length));
                    writer->put_bit(0);

                    reader.reset();
                    while(reader.get_position() < length) {
                        writer->put(algo::Huffman<T>::KEY_BITS, reader.get(algo::Huffman<T>::KEY_BITS));
                    }
                }

                return writer;
            }

            /**
             *  @brief  Read the Huffman dict from the stream and
             *          write the decoded data to an outputstream.
             *
             *  @param  reader
             *      The bytestream to read from.
             *  @return Returns true if current Node has higher frequency.
             */
            utils::memory::unique_t<utils::io::BitStreamReader> decode(utils::io::BitStreamReader& reader) {
                utils::memory::unique_t<utils::io::BitStreamReader> result;

                if (reader.get_size() == 0) {
                    // Nothing to decode?
                    result.reset(nullptr);
                    return result;
                }

                this->buildTree(reader);

                const size_t raw_bits  = reader.get_size_bits();
                      size_t data_bits = raw_bits - reader.get_position();

                if (this->tree_root->isLeaf()) {
                    // No tree was build => No Huffman used, just use passthrough of buffer by setting pointer
                    const size_t data_bytes = utils::bits::round_to_byte(data_bits - 8);

                    auto writer = utils::memory::new_unique_var<utils::io::BitStreamWriter>(data_bytes);

                    while (reader.get_position() < raw_bits-8) {
                        writer->put(algo::Huffman<T>::KEY_BITS, reader.get(algo::Huffman<T>::KEY_BITS));
                    }

                    result.reset(utils::memory::new_var<utils::io::BitStreamReader>(writer->get_buffer(),
                                                                                    writer->get_last_byte_position()));

                    writer->set_managed(false);
                    result->set_managed(true);
                    result->set_position(writer->get_position());

                    utils::Logger::Warn("[Huffman] No Huffman table present in file. Skipping decompression.");
                } else {
                    // Get length in bytes of source
                    const size_t data_bytes = reader.get(algo::Huffman<T>::LEN_BITS);

                    // Consume all other data, bit by bit and traverse Huffman tree to find word
                    auto writer = utils::memory::new_unique_var<utils::io::BitStreamWriter>(data_bytes);

                    while (reader.get_position() < raw_bits && writer->get_last_byte_position() < data_bytes) {
                        this->decode(reader, *writer);

                        if (writer->get_position() + 16 > data_bits) {
                            // Resize buffer if decompression reaches buffer size.
                            data_bits = writer->resize() * 8u;
                        }
                    }

                    const size_t original_length = reader.get_size();
                    const size_t total_length    = writer->get_last_byte_position();

                    result.reset(utils::memory::new_var<utils::io::BitStreamReader>(writer->get_buffer(),
                                                                                     total_length));

                    // Transfer ownership of buffer from writer to result stream
                    writer->set_managed(false);
                    result->set_managed(true);

                    utils::Logger::Info("[Huffman]           Input file size: %8d bytes", original_length);
                    utils::Logger::Info("[Huffman]         Decompressed size: %8d bytes  => Ratio: %.2f%%",
                                          total_length,
                                          float(total_length) / original_length * 100.0f);
                }

                return result;
            }

            /**
             * @brief encode
             * @param rawfile
             * @param encfile
             * @return
             */
            static bool encode(const std::string& rawfile, const std::string& encfile) {
                try {
                    auto enc = utils::io::BitStreamReader::from_file(rawfile);

                    algo::Huffman<T> hm;
                    auto writer = hm.encode(*enc);

                    if (writer) {
                        utils::io::bytes_to_file(encfile,
                                                   writer->get_buffer(),
                                                   writer->get_last_byte_position());
                    } else {
                        utils::Logger::Warn("[Huffman] Nothing to encode! Check contents of '%s'" + utils::Logger::CRLF, rawfile.c_str());
                        return false;
                    }

                    return true;
                } catch (utils::exceptions::FileReadException const& e) {
                    utils::Logger::Error(e.getMessage());
                }

                return false;
            }

            /**
             * @brief decode
             * @param encfile
             * @param decfile
             * @return
             */
            static bool decode(const std::string& encfile, const std::string& decfile) {
                try {
                    auto enc = utils::io::BitStreamReader::from_file(encfile);

                    algo::Huffman<T> hm;
                    auto writer = hm.decode(*enc);

                    if (writer) {
                        utils::io::bytes_to_file(decfile,
                                                   writer->get_buffer(),
                                                   writer->get_size());
                    } else {
                        utils::Logger::Warn("[Huffman] Nothing to decode! Check contents of '%s'" + utils::Logger::CRLF, encfile.c_str());
                        return false;
                    }

                    return true;
                } catch (utils::exceptions::FileReadException const& e) {
                    utils::Logger::Error(e.getMessage());
                }

                return false;
            }

            void printDict(void)  {
                utils::Logger::Info("[Huffman] Dictionary:");

                for (const auto& pair : this->dict) {
                    utils::Logger::Writef("%02X: %8X (%d bits)\n",
                                          pair.first, pair.second.word,pair.second.len);
                }
            }

            void printTree(void) {
                utils::Logger::Info("[Huffman] Tree:");
                algo::Node<T>::printTree(this->tree_root);
            }

            static constexpr inline size_t KEY_BITS = utils::bits::size_of<T>();  ///< Bit length for keys in Huffman dict
            static constexpr inline size_t LEN_BITS = 16ull;                      ///< Bit length to store byte length of source (65k max)

            static constexpr inline size_t DICT_HDR_HAS_ITEMS_BITS  = 1ull;  ///< Whether there are dictionary items following (bit length)
            static constexpr inline size_t DICT_HDR_SEQ_LENGTH_BITS = 7ull;  ///< Amount of bits to represent the length of following items
            static constexpr inline size_t DICT_HDR_ITEM_BITS       = 4ull;  ///< Amount of bits to represent the length of following items
    };
}

#endif // HUFFMAN_HPP
