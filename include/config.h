// Copyright 2019 Kyle F. Downey
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef COINBASE_CONFIG_H
#define COINBASE_CONFIG_H

#include <nlohmann/json.hpp>
#include <experimental/optional>
#include <iostream>
#include <regex>
#include <string>

using nlohmann::json;

namespace coinbase {
    /// @brief simple JSON-based configuration file format for library
    namespace config {
        class Configuration {
        public:
            Configuration(std::istream& in);

            template<class T>
            std::experimental::optional<T> get(const std::string& key) {
                auto key_parts = split(key, std::regex("\\."));
                std::string key_path;
                for (const auto &part : key_parts) {
                    key_path += "/";
                    key_path += part;
                }
                auto key_json_ptr = json::json_pointer(key_path);
                try {
                    auto val = this->parsed_json.at(key_json_ptr);
                    return std::experimental::make_optional<T>(val);
                } catch (json::out_of_range& e) {
                    return std::experimental::nullopt;
                }
            }

            ~Configuration();
        private:
            json parsed_json;

            /// @brief utility function to split a string on an arbitrary regular expression
            std::vector<std::string> split(const std::string& str, const std::regex& on_regexp);
        };
    };
}

#endif //COINBASE_CONFIG_H
