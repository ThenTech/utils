#ifndef UTILS_INI_HPP
#define UTILS_INI_HPP

#include "utils_memory.hpp"
#include "utils_io.hpp"
#include "utils_string.hpp"
#include "utils_print.hpp"
#include "utils_misc.hpp"
#include "utils_traits.hpp"

#include <map>
#include <variant>
#include <sstream>


namespace utils::ini {
    struct Delimiters {
        static inline constexpr utils::print::delimiters_values<char> values = { "[", " = ", "]" };
    };

    template<typename TDelimiters = utils::ini::Delimiters>
    class ConfigReader {
        public:
            using Value    = std::variant<int, size_t, float, double, std::string, bool>;
            using Contents = std::map<std::string, Value>;

        private:
            bool read_from_file;
            const std::string filename;
            std::stringstream inifile;

            std::map<std::string, Contents> settings_map;

            void parse() {
                const size_t val_delim_size = std::strlen(TDelimiters::values.delimiter);
                std::string current_section = "";

                for (std::string line; std::getline(this->inifile, line); ) {
                    utils::string::ltrim(line);

                    // Continue if too small or comment
                    if (line.size() < 2 || line[0] == '#' || line[0] == ';') continue;

                    if (const size_t left_delim = line.find(TDelimiters::values.prefix);
                        left_delim == 0)
                    {
                        if (const size_t right_delim = line.find(TDelimiters::values.postfix, left_delim + 1);
                            right_delim != std::string::npos)
                        {
                            // Found both delimiters, add section
                            current_section = line.substr(left_delim + 1, right_delim - 1);
                            this->settings_map.emplace(current_section, Contents());
                        }
                    } else if (current_section.size() > 0) {
                        // Entered mapped settings
                        if (const size_t val_delim = line.find(TDelimiters::values.delimiter);
                            val_delim != std::string::npos)
                        {
                            std::string val = line.substr(val_delim + val_delim_size);
                            utils::string::rtrim(val);
                            this->settings_map[current_section]
                                    .emplace(line.substr(0, val_delim), val);
                        }
                    }
                }
            }

        public:
            ConfigReader()
                : read_from_file(false)
                , filename("")
            {
                // Empty
            }

            ConfigReader(const std::string& filename)
                : read_from_file(true)
                , filename(filename)
            {
                try {
                    auto contents = utils::io::readStringFromFile(this->filename);
                    this->inifile << *contents;
                    this->parse();
                } CATCH_AND_LOG_ERROR_TRACE(read_from_file = false;);
            }

            ConfigReader(const std::istream& input)
                : read_from_file(false)
                , filename("")
            {
                this->inifile << input.rdbuf();
                this->parse();
            }

            ConfigReader(ConfigReader&&) = delete;

            void save(std::string name = "") {
                if (name == "") {
                    if (this->read_from_file) {
                        name = this->filename;
                    } else {
                        throw utils::exceptions::FileWriteException(name);
                    }
                }

                // MAP TO STRING
                std::stringstream().swap(this->inifile);
                this->inifile << *this;

                try {
                    utils::io::writeStringToFile(name, this->inifile.str());
                } CATCH_AND_LOG_ERROR_TRACE();
            }

            template<class F>
            void ForEachSection(F&& callback) {
                static_assert(std::is_invocable_v<F, const std::string&>, "ForEachSection: Callable function required.");

                for (const auto& [section, mapped_values] : this->settings_map) {
                    UNUSED(mapped_values);
                    std::invoke(std::forward<F>(callback), section);
                }
            }

            template<class F>
            void ForEachSectionKey(const std::string& section, F&& callback) {
                static_assert(std::is_invocable_v<F, const std::string&, const Value&>, "ForEachSectionKey: Callable function required.");

                if (const auto it = this->settings_map.find(section);
                    it != this->settings_map.end())
                {
                    for (const auto& [key, value] : it->second) {
                        std::invoke(std::forward<F>(callback), key, value);
                    }
                }
            }

            inline size_t SectionSize() const {
                return this->settings_map.size();
            }

            inline size_t SectionKeySize(const std::string& section) const {
                return this->settings_map.at(section).size();
            }

            inline bool hasSection(const std::string& section) const {
                return this->settings_map.count(section) == 1;
            }

            inline bool operator[](const std::string& section) const {
                return this->hasSection(section);
            }

            inline bool hasSectionKey(const std::string& section, const std::string& key) const {
                return this->hasSection(section)
                    && this->settings_map.at(section).count(key) == 1;
            }

            inline bool operator()(const std::string& section, const std::string& key) const {
                return this->hasSectionKey(section, key);
            }

            void CreateSection(const std::string& section) {
                if (const auto it = this->settings_map.find(section);
                    it == this->settings_map.end())
                {
                    // Does not exist yet
                    this->settings_map.emplace(section, Contents());
                }
            }

            void RemoveSection(const std::string& section) {
                if (const auto it = this->settings_map.find(section);
                    it != this->settings_map.end())
                {
                    // Exists
                    this->settings_map.erase(it);
                }
            }

            void CreateSectionKey(const std::string& section, const std::string& key) {
                if (const auto it = this->settings_map.find(section);
                    it == this->settings_map.end())
                {
                    // Does not exist yet
                    this->settings_map.emplace(section, Contents());
                }

                this->settings_map.at(section).emplace(key, Value());
            }

            void RemoveSectionKey(const std::string& section, const std::string& key, bool delete_section_if_empty = false) {
                if (const auto it = this->settings_map.find(section);
                    it != this->settings_map.end())
                {
                    // Exists
                    it->second.erase(key);
                }

                if (delete_section_if_empty && this->SectionKeySize(section) == 0) {
                    this->RemoveSection(section);
                }
            }

            template<
                typename T,
                typename std::enable_if<utils::traits::is_variant_member_v<T, Value>, int>::type = 0
            >
            void SetValue(const std::string& section, const std::string& key, T&& val) {
                if (const auto it = this->settings_map.find(section);
                    it == this->settings_map.end())
                {
                    // Does not exist yet
                    this->settings_map.emplace(section, Contents());
                }

                if (const auto it = this->settings_map.at(section).find(key);
                    it == this->settings_map.at(section).end())
                {
                    // Does not exist yet
                    this->settings_map.at(section).emplace(key, Value(val));
                } else {
                    this->settings_map.at(section).at(key) = Value(val);
                }
            }

            template<
                typename T,
                typename std::enable_if<utils::traits::is_variant_member_v<T, Value>, int>::type = 0
            >
            T GetValue(const std::string& section, const std::string& key, const T default_value = T()) const {
                if (const auto it = this->settings_map.find(section);
                    it != this->settings_map.end())
                {
                    if (const auto sett_it = it->second.find(key);
                        sett_it != it->second.end())
                    {
                        // map[section][key] exists
                        const auto& val = this->settings_map.at(section).at(key);

                        if (std::holds_alternative<T>(val)) {
                            // T is available
                            return std::get<T>(val);
                        } else {
                            std::stringstream ss;
                            std::visit([&](auto&& arg){ ss << arg; }, val);

                            // Print to string or cast to value
                            if constexpr(std::is_same_v<T, std::string>) {
                                return ss.str();
                            } else if constexpr(std::is_same_v<T, bool>) {
                                std::string str = ss.str();
                                utils::string::strToLower(str);

                                if (str == "true" || str == "yes"
                                 || str == "on"   || str == "1")
                                {
                                    return true;
                                }
                                if (str == "false" || str == "no"
                                 || str == "off"   || str == "0")
                                {
                                    return false;
                                }
                            } else {
                                return utils::misc::lexical_cast<T>(ss.str().c_str());
                            }
                        }
                    } else {
                        // Possibly return default_value instead?
                        throw utils::exceptions::KeyDoesNotExistException("ConfigReader[" + section + "]", key);
                    }
                } else {
                    // Possibly return default_value instead?
                    throw utils::exceptions::KeyDoesNotExistException("ConfigReader", section);
                }

                return default_value;
            }

            template<typename TChar, typename TCharTraits, typename TDelimiters_ = TDelimiters>
            friend std::basic_ostream<TChar, TCharTraits>&
            operator<<(
                std::basic_ostream<TChar, TCharTraits>& stream,
                const utils::ini::ConfigReader<TDelimiters_>& cfgreader)
            {
                for (auto [section, mapped_values] : cfgreader.settings_map) {
                    if (mapped_values.size() > 0) {
                        stream << TDelimiters_::values.prefix
                               << section
                               << TDelimiters_::values.postfix
                               << std::endl;

                        for (auto [key, value] : mapped_values) {
                            stream << key
                                   << TDelimiters_::values.delimiter;
                            std::visit([&](auto&& arg){ stream << std::boolalpha << arg; }, value);
                            stream << std::endl;
                        }

                        stream << std::endl;
                    }
                }

                return stream;
            }
    };
}

#endif // UTILS_INI_HPP
