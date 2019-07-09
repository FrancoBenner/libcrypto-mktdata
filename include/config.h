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

const char kPathSeparator =
#ifdef _WIN32
        '\\';
#else
        '/';
#endif

namespace coinbase {
    namespace config {
        /// @brief simple JSON-based configuration file format for library
        class Configuration {
        public:
            static const Configuration* load_default();

            Configuration(std::istream& in);

            /// @brief generic getter that extracts a dot-delimited key corresponding to the JSON path structure
            template<class T>
            std::experimental::optional<T> get(const std::string& key) const {
                auto key_parts = split(key, std::regex("\\."));
                std::string key_path;
                for (const auto &part : key_parts) {
                    key_path += "/";
                    key_path += part;
                }
                auto key_json_ptr = json::json_pointer(key_path);
                try {
                    auto val = this->parsed_json_.at(key_json_ptr);
                    return std::experimental::make_optional<T>(val);
                } catch (json::out_of_range& e) {
                    return std::experimental::nullopt;
                }
            }

            ~Configuration();
        private:
            json parsed_json_;

            /// @brief utility function to split a string on an arbitrary regular expression
            static std::vector<std::string> split(const std::string& str, const std::regex& on_regexp);
        };

        /// @brief helper class that can parse a URI string into a host, port and path, with appropriate defaulting
        class WebSocketURI {
        public:
            explicit WebSocketURI(const std::string& uri_txt);

            int get_port() const { return port_; }

            const std::string& get_host() const { return host_; }

            const std::string& get_path() const { return path_; }

            ~WebSocketURI();
        private:
            std::regex uri_regex_ = std::regex("(ws|wss):\\/\\/([^\\/#:]+)(:(\\d+))?(.+)?");

            int port_;
            std::string host_;
            std::string path_;
        };
    };
}

#endif //COINBASE_CONFIG_H
