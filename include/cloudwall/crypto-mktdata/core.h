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

#ifndef CRYPTO_MKTDATA_CORE_H
#define CRYPTO_MKTDATA_CORE_H

#include <ostream>

namespace cloudwall::core::marketdata {
    class Currency {
    public:
        explicit Currency(const std::string &ccy_code);

        [[nodiscard]] const std::string &get_ccy_code() const {
            return ccy_code_;
        }

        bool operator==(const Currency &other) const {
            return (this->get_ccy_code() == other.get_ccy_code());
        }

        ~Currency() = default;

    private:
        const std::string &ccy_code_;
    };

    class ProductId {
    public:
        ProductId(const Currency& quote_ccy, const Currency& base_ccy);

        [[nodiscard]] const Currency& get_base_ccy() const {
            return base_ccy_;
        }

        [[nodiscard]] const Currency& get_quote_ccy() const {
            return quote_ccy_;
        }

        bool operator == (const ProductId &other) const {
            return (this->get_base_ccy() == other.get_base_ccy())
                   && (this->get_quote_ccy() == other.get_quote_ccy());
        }

        ~ProductId() = default;
    private:
        const Currency& base_ccy_;

        const Currency& quote_ccy_;
    };

    class RawFeedMessage {
    public:
        explicit RawFeedMessage(const std::string& raw_json);

        [[nodiscard]] const std::string& get_raw_json() const {
            return raw_json_;
        }

        ~RawFeedMessage();
    private:
        const std::string& raw_json_;
    };

    using OnRawFeedMessageCallback = std::function<void(const RawFeedMessage&)>;
}

#endif //CRYPTO_MKTDATA_CORE_H
